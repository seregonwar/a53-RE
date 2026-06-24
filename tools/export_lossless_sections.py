#!/usr/bin/env python3
"""Generate source-level, byte-exact assembly for every allocated ELF section.

This is a lossless reference layer, intentionally separate from the semantic
C/C++ recovery. It makes an auditable, compilable representation of code and
initialized data while canonical source is reconstructed.
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

SHF_ALLOC = 0x2
SHF_EXECINSTR = 0x4
SHT_PROGBITS = 1


def elf_sections(path: Path):
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    shoff, shentsize, shnum, shstrndx = header[6], header[11], header[12], header[13]
    headers = [struct.unpack_from("<IIQQQQIIQQ", data, shoff + index * shentsize) for index in range(shnum)]
    strings_header = headers[shstrndx]
    strings = data[strings_header[4] : strings_header[4] + strings_header[5]]
    for name_offset, kind, flags, address, offset, size, _, _, align, _ in headers:
        end = strings.find(b"\0", name_offset)
        name = strings[name_offset:end].decode()
        if kind == SHT_PROGBITS and flags & SHF_ALLOC and size:
            yield {
                "name": name,
                "flags": flags,
                "address": address,
                "size": size,
                "align": align,
                "data": data[offset : offset + size],
            }


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


def output_name(section: str) -> str:
    return re.sub(r"[^A-Za-z0-9_.-]", "_", section.lstrip(".")) + ".S"


def section_attributes(flags: int) -> str:
    return "ax" if flags & SHF_EXECINSTR else "aw"


def emit_section(
    section: dict, destination: Path, labels: dict[int, list[str]], global_symbols: set[tuple[int, str]]
) -> dict:
    name = section["name"]
    address = section["address"]
    data = section["data"]
    executable = bool(section["flags"] & SHF_EXECINSTR)
    emitted_labels: dict[int, list[str]] = {}
    seen_labels: set[str] = set()
    duplicate_labels: dict[int, list[str]] = {}
    for entry in sorted(labels):
        if not address <= entry < address + len(data):
            continue
        for label in labels[entry]:
            if label in seen_labels:
                duplicate_labels.setdefault(entry, []).append(label)
            else:
                emitted_labels.setdefault(entry, []).append(label)
                seen_labels.add(label)
    relevant_labels = {
        label
        for entry, labels_at_address in emitted_labels.items()
        for label in labels_at_address
        if (entry, label) in global_symbols
    }
    lines = [
        "/* A53-RE lossless reference section; bytes are authoritative. */",
        f"/* original section {name}, VMA 0x{address:08x}, {len(data)} bytes */",
        ".arch armv8-a",
        f'.section {name}, "{section_attributes(section["flags"])}", %progbits',
        f".p2align {max(0, section['align'].bit_length() - 1)}",
    ]
    lines.extend(f".globl {label}" for label in sorted(relevant_labels))
    disassembler = Cs(CS_ARCH_ARM64, CS_MODE_LITTLE_ENDIAN)
    if executable:
        for offset in range(0, len(data) - len(data) % 4, 4):
            current = address + offset
            lines.extend(f"{label}:" for label in emitted_labels.get(current, []))
            for label in duplicate_labels.get(current, []):
                lines.append(f"/* duplicate local symbol at this address: {label} */")
            word = struct.unpack_from("<I", data, offset)[0]
            decoded = next(disassembler.disasm(data[offset : offset + 4], current), None)
            text = "<undecoded>" if decoded is None else f"{decoded.mnemonic} {decoded.op_str}".rstrip()
            lines.append(f"    .inst 0x{word:08x} /* 0x{current:08x}: {text} */")
        for offset in range(len(data) - len(data) % 4, len(data)):
            lines.append(f"    .byte 0x{data[offset]:02x}")
    else:
        offset = 0
        while offset < len(data):
            current = address + offset
            lines.extend(f"{label}:" for label in emitted_labels.get(current, []))
            for label in duplicate_labels.get(current, []):
                lines.append(f"/* duplicate local symbol at this address: {label} */")
            next_labels = [entry - current for entry in labels if current < entry < address + len(data)]
            chunk_size = min(16, len(data) - offset, min(next_labels, default=16))
            chunk = data[offset : offset + chunk_size]
            lines.append("    .byte " + ", ".join(f"0x{value:02x}" for value in chunk))
            offset += chunk_size
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text("\n".join(lines) + "\n")
    return {key: section[key] for key in ("name", "flags", "address", "size", "align")} | {"file": str(destination)}


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    labels, globals_ = symbols(args.elf)
    manifest = []
    for section in elf_sections(args.elf):
        destination = args.output / output_name(section["name"])
        manifest.append(emit_section(section, destination, labels, globals_))
    (args.output / "manifest.json").write_text(json.dumps(manifest, indent=2) + "\n")
    print(json.dumps(manifest, indent=2))


if __name__ == "__main__":
    main()
