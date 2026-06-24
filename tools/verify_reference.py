#!/usr/bin/env python3
"""Report exact-ELF and PT_LOAD segment equality for a proposed rebuilt image."""

from __future__ import annotations

import argparse
import hashlib
import struct
from pathlib import Path


def load_segments(path: Path):
    data = path.read_bytes()
    header = struct.unpack_from("<16sHHIQQQIHHHHHH", data)
    if header[0][:4] != b"\x7fELF":
        raise ValueError(f"not an ELF file: {path}")
    phoff, phentsize, phnum = header[5], header[9], header[10]
    segments = []
    for offset in range(phoff, phoff + phentsize * phnum, phentsize):
        kind, flags, file_offset, vaddr, _, filesz, memsz, align = struct.unpack_from(
            "<IIQQQQQQ", data, offset
        )
        if kind == 1:  # PT_LOAD
            segments.append(
                {
                    "offset": file_offset,
                    "vaddr": vaddr,
                    "paddr": _,
                    "flags": flags,
                    "align": align,
                    "filesz": filesz,
                    "memsz": memsz,
                    "data": data[file_offset : file_offset + filesz],
                }
            )
    return data, segments


def digest(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("candidate", type=Path, nargs="?")
    parser.add_argument("--require", choices=("report", "runtime", "file"), default="report")
    args = parser.parse_args()

    reference, reference_segments = load_segments(args.reference)
    print(f"reference sha256: {digest(reference)}")
    print(f"reference bytes: {len(reference)}")
    print(f"reference PT_LOAD segments: {len(reference_segments)}")
    if args.candidate is None:
        return

    candidate, candidate_segments = load_segments(args.candidate)
    print(f"candidate sha256: {digest(candidate)}")
    file_equal = reference == candidate
    same_count = len(reference_segments) == len(candidate_segments)
    runtime_equal = same_count
    print(f"exact file equality: {file_equal}")
    print(f"PT_LOAD count equality: {same_count}")
    for index, (left, right) in enumerate(zip(reference_segments, candidate_segments)):
        runtime_fields = ("vaddr", "paddr", "flags", "align", "filesz", "memsz", "data")
        runtime_same = all(left[field] == right[field] for field in runtime_fields)
        runtime_equal = runtime_equal and runtime_same
        print(f"PT_LOAD[{index}] runtime equality: {runtime_same}")
        print(f"PT_LOAD[{index}] file offset equality: {left['offset'] == right['offset']}")
    if args.require == "runtime" and not runtime_equal:
        raise SystemExit("runtime PT_LOAD equality requirement failed")
    if args.require == "file" and not file_equal:
        raise SystemExit("whole-file equality requirement failed")


if __name__ == "__main__":
    main()
