#!/usr/bin/env python3
"""Compile lossless reference sections and prove their section contents match a53.elf."""

from __future__ import annotations

import argparse
import json
import struct
import subprocess
import tempfile
from pathlib import Path


def section(path: Path, wanted: str) -> bytes:
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    shoff, shentsize, shnum, shstrndx = header[6], header[11], header[12], header[13]
    headers = [struct.unpack_from("<IIQQQQIIQQ", data, shoff + index * shentsize) for index in range(shnum)]
    string_header = headers[shstrndx]
    strings = data[string_header[4] : string_header[4] + string_header[5]]
    for name_offset, _, _, _, offset, size, _, _, _, _ in headers:
        end = strings.find(b"\0", name_offset)
        if strings[name_offset:end].decode() == wanted:
            return data[offset : offset + size]
    raise ValueError(f"section not found: {wanted}")


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", type=Path)
    parser.add_argument("sections", type=Path)
    parser.add_argument("--clang", default="clang")
    args = parser.parse_args()

    manifest = json.loads((args.sections / "manifest.json").read_text())
    failures = []
    with tempfile.TemporaryDirectory(prefix="a53-lossless-") as directory:
        work = Path(directory)
        for record in manifest:
            source = Path(record["file"])
            object_file = work / (source.stem + ".o")
            subprocess.run([args.clang, "--target=aarch64-none-elf", "-c", str(source), "-o", str(object_file)], check=True)
            expected = section(args.elf, record["name"])
            actual = section(object_file, record["name"])
            equal = actual == expected
            print(f"{record['name']}: {'OK' if equal else 'MISMATCH'} ({len(actual)} bytes)")
            if not equal:
                failures.append(record["name"])
    if failures:
        raise SystemExit(f"lossless section mismatch: {', '.join(failures)}")


if __name__ == "__main__":
    main()
