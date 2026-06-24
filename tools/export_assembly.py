#!/usr/bin/env python3
"""Emit valid AArch64 `.S` recovery files for the original assembly CUs.

Each instruction is emitted as `.inst`, preserving its original four-byte word.
The disassembly in the trailing comment is for review only; it is not used to
rebuild the instruction. A linker script must still place each section at its
recorded virtual address for an exact image.
"""

from __future__ import annotations

import argparse
import json
import re
import struct
import subprocess
from collections import defaultdict
from pathlib import Path

from capstone import CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN, Cs


def load_segments(path: Path) -> list[tuple[int, int, bytes]]:
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    if header[0][:4] != b"\x7fELF" or header[2] != 183:
        raise ValueError(f"expected an AArch64 ELF: {path}")
    phoff, phentsize, phnum = header[5], header[9], header[10]
    segments: list[tuple[int, int, bytes]] = []
    for position in range(phoff, phoff + phentsize * phnum, phentsize):
        kind, _, offset, vaddr, _, filesz, _, _ = struct.unpack_from("<IIQQQQQQ", data, position)
        if kind == 1 and filesz:
            segments.append((vaddr, vaddr + filesz, data[offset : offset + filesz]))
    return segments


def memory_at(segments: list[tuple[int, int, bytes]], address: int, size: int) -> bytes:
    for start, end, data in segments:
        if start <= address and address + size <= end:
            return data[address - start : address - start + size]
    raise ValueError(f"address range absent from PT_LOAD data: 0x{address:x}+0x{size:x}")


def symbols(elf: Path) -> tuple[dict[int, list[str]], set[tuple[int, str]]]:
    output = subprocess.check_output(["nm", "-nm", str(elf)], text=True, stderr=subprocess.DEVNULL)
    labels: dict[int, list[str]] = defaultdict(list)
    global_symbols: set[tuple[int, str]] = set()
    valid = re.compile(r"^[A-Za-z_.$][A-Za-z0-9_.$]*$")
    for line in output.splitlines():
        columns = line.split(maxsplit=2)
        if len(columns) != 3 or not re.fullmatch(r"[0-9a-fA-F]+", columns[0]):
            continue
        address, symbol_type, name = columns
        if valid.fullmatch(name):
            labels[int(address, 16)].append(name)
            if symbol_type.isupper():
                global_symbols.add((int(address, 16), name))
    return labels, global_symbols


def safe_section_name(source: str) -> str:
    return re.sub(r"[^A-Za-z0-9_.]", "_", source.removeprefix("src/"))


def emit_source(
    destination: Path,
    source: str,
    ranges: list[list[int]],
    segments: list[tuple[int, int, bytes]],
    labels: dict[int, list[str]],
    global_symbols: set[tuple[int, str]],
) -> int:
    base = min(low for low, _ in ranges)
    disassembler = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    lines = [
        "/*",
        " * A53-RE instruction-exact assembly recovery artifact.",
        f" * Original compilation unit: {source}",
        f" * Section virtual-address base: 0x{base:08x}",
        " * `.inst` is authoritative; comments are Capstone review disassembly.",
        " */",
        ".arch armv8-a",
        f'.section .text.a53.{safe_section_name(source)}, "ax", %progbits',
        ".p2align 2",
    ]
    exported = sorted(
        label
        for low, high in ranges
        for address in labels
        if low <= address < high
        for label in labels[address]
        if (address, label) in global_symbols
    )
    lines.extend(f".globl {label}" for label in exported)
    instructions = 0
    for low, high in sorted(ranges):
        lines.append(f"\n/* original range [0x{low:08x}, 0x{high:08x}) */")
        lines.append(f".org 0x{low - base:x}")
        raw = memory_at(segments, low, high - low)
        for offset in range(0, len(raw) - len(raw) % 4, 4):
            address = low + offset
            for label in labels.get(address, []):
                lines.append(f"{label}:")
            word = struct.unpack_from("<I", raw, offset)[0]
            decoded = next(disassembler.disasm(raw[offset : offset + 4], address), None)
            text = "<undecoded>" if decoded is None else f"{decoded.mnemonic} {decoded.op_str}".rstrip()
            lines.append(f"    .inst 0x{word:08x} /* 0x{address:08x}: {text} */")
            instructions += 1
        for offset in range(len(raw) - len(raw) % 4, len(raw)):
            lines.append(f"    .byte 0x{raw[offset]:02x}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text("\n".join(lines) + "\n")
    return instructions


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", type=Path)
    parser.add_argument("index", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    units = json.loads((args.index / "compile_units.json").read_text())
    ranges = json.loads((args.index / "compile_unit_ranges.json").read_text())
    range_by_source = {record["source"]: record["ranges"] for record in ranges}
    segments = load_segments(args.elf)
    label_map, global_symbols = symbols(args.elf)
    manifest = []
    for unit in units:
        if unit["language"] != "AArch64 assembly":
            continue
        source_ranges = range_by_source.get(unit["path"], [])
        if not source_ranges:
            manifest.append({"source": unit["path"], "ranges": 0, "instructions": 0, "status": "no DWARF range"})
            continue
        instructions = emit_source(
            args.output / unit["path"], unit["path"], source_ranges, segments, label_map, global_symbols
        )
        manifest.append(
            {"source": unit["path"], "ranges": len(source_ranges), "instructions": instructions, "status": "exported"}
        )
    metadata = args.output / "metadata"
    metadata.mkdir(exist_ok=True)
    (metadata / "assembly_manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(json.dumps(manifest, indent=2))


if __name__ == "__main__":
    main()
