#!/usr/bin/env python3
"""
A53-RE Gadget Scanner — Automated ROP/JOP gadget finder

Scans the binary (binary-files/a53.elf) for useful ROP/JOP gadgets in the EL3 loader
sections. Outputs a catalog of candidate gadgets with their addresses and operation.

Usage:
  python3 gadget_scanner.py [--verbose]
"""

import struct
import sys
from pathlib import Path
from dataclasses import dataclass
from typing import Optional

try:
    from capstone import Cs, CS_ARCH_ARM64, CS_MODE_ARM
    HAS_CAPSTONE = True
except ImportError:
    HAS_CAPSTONE = False
    print("[-] capstone not installed. Install with: pip install capstone")
    sys.exit(1)


@dataclass
class Gadget:
    address: int
    instructions: list
    terminates_with: str  # "ret", "eret", "br", "blr"
    category: str  # "stack_pivot", "sys_reg", "mem_access", "branch", "arith"


class GadgetScanner:
    """Scan EL3 text sections for ROP/JOP gadgets."""

    # EL3 text sections to scan
    SECTIONS = [
        (0x00100000, 0x00117000, "EL3 Loader"),
        (0x00000000, 0x00010000, "Reset Vector"),
    ]

    # Interesting instruction patterns
    PATTERNS = {
        "ret": {
            "mnemonic": "ret",
            "category": "branch",
            "description": "Return gadget"
        },
        "eret": {
            "mnemonic": "eret",
            "category": "branch",
            "description": "Exception return gadget"
        },
        "msr": {
            "mnemonic": "msr",
            "category": "sys_reg",
            "description": "System register write"
        },
        "mrs": {
            "mnemonic": "mrs",
            "category": "sys_reg",
            "description": "System register read"
        },
        "br": {
            "mnemonic": "br",
            "category": "branch",
            "description": "Indirect branch"
        },
        "blr": {
            "mnemonic": "blr",
            "category": "branch",
            "description": "Indirect call"
        },
        "str_sp": {
            "mnemonic": "str",
            "category": "mem_access",
            "description": "Store to memory (sp-relative)",
            "extra_check": lambda insn: "sp" in insn.op_str
        },
        "ldp_ret": {
            "mnemonic": "ldp",
            "category": "stack_pivot",
            "description": "Load pair (potential stack restore)"
        },
    }

    def __init__(self, elf_path: str):
        self.elf_path = Path(elf_path)
        self.data = self.elf_path.read_bytes()
        self.md = Cs(CS_ARCH_ARM64, CS_MODE_ARM)
        self.md.detail = True

    def extract_section(self, vaddr: int, size: int) -> Optional[bytes]:
        """Extract section bytes from ELF by virtual address (assumes raw==virtual for SRAM)."""
        # Simplified: assume the ELF is loaded at virtual==file offset
        # For the a53.elf, sections are at known VAs
        for phdr_offset, phdr_vaddr, phdr_filesz, phdr_offset_file in self._get_phdrs():
            if phdr_vaddr <= vaddr and vaddr + size <= phdr_vaddr + phdr_filesz:
                file_offset = phdr_offset_file + (vaddr - phdr_vaddr)
                return self.data[file_offset:file_offset + size]
        return None

    def _get_phdrs(self):
        """Parse ELF program headers."""
        if self.data[:4] != b'\x7fELF':
            return []
        # 64-bit ELF header
        phoff = struct.unpack_from("<Q", self.data, 0x20)[0]
        phentsize = struct.unpack_from("<H", self.data, 0x36)[0]
        phnum = struct.unpack_from("<H", self.data, 0x38)[0]

        for i in range(phnum):
            offset = phoff + i * phentsize
            p_type = struct.unpack_from("<I", self.data, offset)[0]
            if p_type != 1:  # PT_LOAD
                continue
            p_offset = struct.unpack_from("<Q", self.data, offset + 8)[0]
            p_vaddr = struct.unpack_from("<Q", self.data, offset + 16)[0]
            p_filesz = struct.unpack_from("<Q", self.data, offset + 32)[0]
            yield p_offset, p_vaddr, p_filesz, p_offset

    def scan(self, verbose: bool = False) -> list[Gadget]:
        """Scan all sections for terminating gadgets."""
        gadgets = []
        seen = set()

        for vaddr, size, name in self.SECTIONS:
            code = self.extract_section(vaddr, size)
            if not code:
                print(f"[-] Cannot extract section {name} at 0x{vaddr:x}")
                continue

            if verbose:
                print(f"[*] Scanning {name} (0x{vaddr:x}-0x{vaddr+size:x}, {len(code)} bytes)")

            for insn in self.md.disasm(code, vaddr):
                addr = insn.address
                mnem = insn.mnemonic
                op = insn.op_str

                # Check for terminating instructions (gadget ends)
                if mnem in ("ret", "eret"):
                    # Walk backwards to find useful prefix
                    prefix_insns = self._get_gadget_prefix(code, addr - vaddr, vaddr)
                    prefix_str = "; ".join(
                        f"{i.mnemonic} {i.op_str}" for i in prefix_insns[-4:]
                    )

                    gadget_id = (addr, mnem, prefix_str)
                    if gadget_id in seen:
                        continue
                    seen.add(gadget_id)

                    category = "branch"
                    if any("sp" in i.op_str and i.mnemonic == "mov" for i in prefix_insns):
                        category = "stack_pivot"
                    elif any(i.mnemonic in ("msr", "mrs") for i in prefix_insns):
                        category = "sys_reg"
                    elif any(i.mnemonic in ("str", "stp") for i in prefix_insns):
                        category = "mem_access"

                    gadgets.append(Gadget(
                        address=addr,
                        instructions=prefix_insns + [insn],
                        terminates_with=mnem,
                        category=category
                    ))

        return gadgets

    def _get_gadget_prefix(self, code: bytes, offset: int, vaddr: int) -> list:
        """Walk backwards from offset to find useful preceding instructions."""
        prefix = []
        # Look back up to 8 instructions (32 bytes)
        for back in range(4, 36, 4):
            if offset - back < 0:
                break
            chunk = code[offset - back:offset - back + 4]
            try:
                insns = list(self.md.disasm(chunk, vaddr + offset - back))
                if insns:
                    insn = insns[0]
                    if insn.mnemonic in ("ret", "eret", "b", "bl", "br", "blr"):
                        break  # Another terminator — stop
                    if insn.address + 4 == vaddr + offset - back + 4:
                        prefix.insert(0, insn)
            except:
                break
        return prefix

    def report(self, gadgets: list[Gadget]):
        """Print gadget catalog."""
        categories = {}
        for g in gadgets:
            cat = g.category
            if cat not in categories:
                categories[cat] = []
            categories[cat].append(g)

        print(f"\n{'='*80}")
        print(f"Gadget Scanner Results: {len(gadgets)} total gadgets found")
        print(f"{'='*80}")

        for cat, g_list in sorted(categories.items()):
            print(f"\n─── {cat.upper()} ({len(g_list)} gadgets) ───")
            for g in g_list[:10]:  # Show top 10 per category
                # Show last 3 instructions
                last3 = g.instructions[-3:]
                insn_str = "; ".join(
                    f"{i.mnemonic} {i.op_str}" for i in last3
                )
                print(f"  0x{g.address:08x}: {insn_str}")
            if len(g_list) > 10:
                print(f"  ... and {len(g_list) - 10} more")

        # Key finding summary
        print(f"\n─── KEY FINDINGS ───")
        stack_pivots = [g for g in gadgets if g.category == "stack_pivot"]
        sys_regs = [g for g in gadgets if g.category == "sys_reg"]
        erets = [g for g in gadgets if g.terminates_with == "eret"]

        print(f"  Stack pivot gadgets: {len(stack_pivots)}")
        for g in stack_pivots:
            insn_str = "; ".join(f"{i.mnemonic} {i.op_str}" for i in g.instructions[-3:])
            print(f"    0x{g.address:08x}: {insn_str}")

        print(f"  System register gadgets: {len(sys_regs)}")
        for g in sys_regs:
            insn_str = "; ".join(f"{i.mnemonic} {i.op_str}" for i in g.instructions[-3:])
            print(f"    0x{g.address:08x}: {insn_str}")

        print(f"  ERET gadgets: {len(erets)}")
        for g in erets:
            print(f"    0x{g.address:08x}")


def main():
    import argparse
    parser = argparse.ArgumentParser(description="A53-RE Gadget Scanner")
    parser.add_argument("--elf", default="binary-files/a53.elf", help="Path to a53.elf")
    parser.add_argument("--verbose", "-v", action="store_true", help="Verbose output")
    args = parser.parse_args()

    scanner = GadgetScanner(args.elf)
    gadgets = scanner.scan(verbose=args.verbose)
    scanner.report(gadgets)


if __name__ == "__main__":
    main()
