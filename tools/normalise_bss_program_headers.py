#!/usr/bin/env python3
"""Match zero-initialized PT_LOAD file sizes to the reference after LLD linking.

LLD may retain bytes for a NOLOAD-only segment in the file although the
reference ELF encodes the same memory as p_filesz=0, p_memsz>0. This tool only
changes p_filesz for a segment whose VMA, flags and memsz agree with a
reference segment having zero file size; it never touches load bytes.
"""

from __future__ import annotations

import argparse
import struct
from pathlib import Path


def program_headers(data: bytes):
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    return header[5], header[9], header[10]


def load_headers(data: bytes):
    offset, entry_size, count = program_headers(data)
    for index in range(count):
        position = offset + index * entry_size
        values = struct.unpack_from("<IIQQQQQQ", data, position)
        if values[0] == 1:
            yield position, values


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("candidate", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    reference = args.reference.read_bytes()
    candidate = bytearray(args.candidate.read_bytes())
    reference_bss = {
        (header[3], header[1], header[6]): header
        for _, header in load_headers(reference)
        if header[5] == 0 and header[6] > 0
    }
    patched = 0
    for position, header in load_headers(candidate):
        key = (header[3], header[1], header[6])
        if key in reference_bss and header[5] != 0:
            struct.pack_into("<Q", candidate, position + 32, 0)
            patched += 1
    args.output.write_bytes(candidate)
    print(f"normalised {patched} BSS PT_LOAD headers")


if __name__ == "__main__":
    main()
