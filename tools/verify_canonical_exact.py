#!/usr/bin/env python3
"""Compile canonical functions and enforce byte equality for verified entries only."""

from __future__ import annotations

import argparse
import json
import subprocess
import tempfile
from pathlib import Path


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("reference", type=Path)
    parser.add_argument("canonical", type=Path)
    parser.add_argument("comparator", type=Path)
    parser.add_argument("--clang", default="clang")
    args = parser.parse_args()

    entries = json.loads((args.canonical / "verified_functions.json").read_text())
    flags = [
        "--target=aarch64-none-elf",
        "-ffreestanding",
        "-fno-builtin",
        "-fno-stack-protector",
        "-Oz",
        f"-I{args.canonical / 'include'}",
    ]
    objects: dict[str, Path] = {}
    with tempfile.TemporaryDirectory(prefix="a53-canonical-") as directory:
        work = Path(directory)
        for entry in entries:
            source = entry["source"]
            if source not in objects:
                output = work / (Path(source).stem + ".o")
                subprocess.run([args.clang, *flags, "-c", str(args.canonical / source), "-o", str(output)], check=True)
                objects[source] = output
            subprocess.run(
                [
                    "python3",
                    str(args.comparator),
                    str(args.reference),
                    str(objects[source]),
                    entry["symbol"],
                    entry["address"],
                    entry["size"],
                ],
                check=True,
            )


if __name__ == "__main__":
    main()
