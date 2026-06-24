#!/usr/bin/env python3
"""Compare one relocatable-object symbol with a byte range in the reference ELF."""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


def elf(path: Path):
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    if header[0][:4] != b"\x7fELF":
        raise ValueError(f"not ELF: {path}")
    return data, header


def headers(data: bytes, header):
    shoff, shentsize, shnum = header[6], header[11], header[12]
    return [struct.unpack_from("<IIQQQQIIQQ", data, shoff + index * shentsize) for index in range(shnum)]


def symbol_bytes(path: Path, wanted: str) -> bytes:
    data, header = elf(path)
    sections = headers(data, header)
    for section in sections:
        name_offset, kind, _, _, offset, size, link, _, _, entry_size = section
        if kind != 2:  # SHT_SYMTAB
            continue
        strings = sections[link]
        string_data = data[strings[4] : strings[4] + strings[5]]
        symbols = []
        for position in range(offset, offset + size, entry_size):
            name, info, _, section_index, value, symbol_size = struct.unpack_from("<IBBHQQ", data, position)
            end = string_data.find(b"\0", name)
            symbol_name = string_data[name:end].decode()
            symbols.append((symbol_name, section_index, value, symbol_size))
        for symbol_name, section_index, value, symbol_size in symbols:
            if symbol_name != wanted or section_index == 0:
                continue
            if symbol_size == 0:
                following = [other_value for _, other_section, other_value, _ in symbols if other_section == section_index and other_value > value]
                symbol_size = min(following, default=sections[section_index][5]) - value
            section_data = sections[section_index]
            return data[section_data[4] + value : section_data[4] + value + symbol_size]
    raise ValueError(f"symbol not found: {wanted}")


def reference_bytes(path: Path, address: int, size: int) -> bytes:
    data, header = elf(path)
    phoff, phentsize, phnum = header[5], header[9], header[10]
    for index in range(phnum):
        kind, _, offset, vaddr, _, filesz, _, _ = struct.unpack_from("<IIQQQQQQ", data, phoff + index * phentsize)
        if kind == 1 and vaddr <= address and address + size <= vaddr + filesz:
            start = offset + address - vaddr
            return data[start : start + size]
    raise ValueError(f"reference address not file-backed: 0x{address:x}+0x{size:x}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("object", type=Path)
    parser.add_argument("symbol")
    parser.add_argument("address", type=lambda value: int(value, 0))
    parser.add_argument("size", type=lambda value: int(value, 0))
    args = parser.parse_args()

    actual = symbol_bytes(args.object, args.symbol)
    expected = reference_bytes(args.reference, args.address, args.size)
    equal = actual == expected
    print(f"symbol: {args.symbol}")
    print(f"object bytes: {actual.hex()}")
    print(f"reference bytes: {expected.hex()}")
    print(f"exact equality: {equal}")
    if not equal:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
