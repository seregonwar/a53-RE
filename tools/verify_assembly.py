#!/usr/bin/env python3
"""Compile recovered `.S` units and prove their emitted instruction bytes match the ELF."""

from __future__ import annotations

import argparse
import json
import re
import struct
import subprocess
import tempfile
from pathlib import Path


def load_segments(path: Path) -> list[tuple[int, int, bytes]]:
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    phoff, phentsize, phnum = header[5], header[9], header[10]
    result = []
    for position in range(phoff, phoff + phentsize * phnum, phentsize):
        kind, _, offset, vaddr, _, filesz, _, _ = struct.unpack_from("<IIQQQQQQ", data, position)
        if kind == 1 and filesz:
            result.append((vaddr, vaddr + filesz, data[offset : offset + filesz]))
    return result


def memory_at(segments: list[tuple[int, int, bytes]], address: int, size: int) -> bytes:
    for start, end, data in segments:
        if start <= address and address + size <= end:
            return data[address - start : address - start + size]
    raise ValueError(f"range absent from PT_LOAD: 0x{address:x}+0x{size:x}")


def section(path: Path, wanted: str) -> bytes:
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    shoff, shentsize, shnum, shstrndx = header[6], header[11], header[12], header[13]
    headers = [struct.unpack_from("<IIQQQQIIQQ", data, shoff + index * shentsize) for index in range(shnum)]
    string_header = headers[shstrndx]
    strings = data[string_header[4] : string_header[4] + string_header[5]]
    for name_offset, _, _, _, offset, size, _, _, _, _ in headers:
        end = strings.find(b"\0", name_offset)
        name = strings[name_offset:end].decode()
        if name == wanted:
            return data[offset : offset + size]
    raise ValueError(f"section not found: {wanted}")


def section_name(source: str) -> str:
    return ".text.a53." + re.sub(r"[^A-Za-z0-9_.]", "_", source.removeprefix("src/"))


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", type=Path)
    parser.add_argument("index", type=Path)
    parser.add_argument("recovered", type=Path)
    parser.add_argument("--clang", default="clang")
    args = parser.parse_args()

    units = json.loads((args.index / "compile_units.json").read_text())
    ranges = json.loads((args.index / "compile_unit_ranges.json").read_text())
    ranges_by_source = {record["source"]: record["ranges"] for record in ranges}
    segments = load_segments(args.elf)
    failures = []
    with tempfile.TemporaryDirectory(prefix="a53-asm-") as directory:
        work = Path(directory)
        for unit in units:
            if unit["language"] != "AArch64 assembly":
                continue
            source = unit["path"]
            source_ranges = ranges_by_source[source]
            base = min(low for low, _ in source_ranges)
            expected = bytearray(max(high for _, high in source_ranges) - base)
            for low, high in source_ranges:
                expected[low - base : high - base] = memory_at(segments, low, high - low)
            object_file = work / (Path(source).stem + ".o")
            subprocess.run(
                [args.clang, "--target=aarch64-none-elf", "-c", str(args.recovered / source), "-o", str(object_file)],
                check=True,
            )
            actual = section(object_file, section_name(source))
            equal = actual == bytes(expected)
            print(f"{source}: {'OK' if equal else 'MISMATCH'} ({len(actual)} bytes)")
            if not equal:
                failures.append(source)
    if failures:
        raise SystemExit(f"assembly mismatch: {', '.join(failures)}")


if __name__ == "__main__":
    main()
