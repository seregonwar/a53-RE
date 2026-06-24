#!/usr/bin/env python3
"""Extract a stable source and function index from the DWARF kept in a53.elf."""

from __future__ import annotations

import argparse
import json
import posixpath
import re
import subprocess
from collections import Counter
from pathlib import Path


def normalise_source_path(value: str) -> str:
    """Map the original Windows build path to a path below the recovered src tree."""
    path = value.replace("\\", "/")
    path = re.sub(r"/+", "/", path)
    path = posixpath.normpath(path)
    lower = path.lower()
    source_marker = "/mp4/a53/src/"
    position = lower.find(source_marker)
    if position >= 0:
        return "src/" + path[position + len(source_marker) :]
    mp4_marker = "/mp4/"
    position = lower.find(mp4_marker)
    if position >= 0:
        return "external/" + path[position + len(mp4_marker) :]
    return "external/unclassified/" + path.replace(":", "_").lstrip("/")


def command(*args: str) -> str:
    return subprocess.check_output(args, text=True, stderr=subprocess.STDOUT)


def source_units(elf: Path) -> list[dict[str, str]]:
    producer = command("dwarfdump", "-P", str(elf))
    units: list[dict[str, str]] = []
    for raw_path in re.findall(r"^\s*\d+:\s+'([^']+)'", producer, re.MULTILINE):
        suffix = Path(raw_path.replace("\\", "/")).suffix.lower()
        language = {".c": "C", ".cpp": "C++", ".s": "AArch64 assembly"}.get(suffix, "unknown")
        units.append({"original_path": raw_path, "path": normalise_source_path(raw_path), "language": language})
    return units


def function_index(elf: Path) -> list[dict[str, object]]:
    # Dense DWARF output puts one DIE on one physical line, making this parser deliberately
    # conservative: only records independently addressable subprogram DIEs with a source path.
    info = command("dwarfdump", "-i", "-d", "-e", str(elf))
    result: list[dict[str, object]] = []
    seen_entries: set[int] = set()
    for line in info.splitlines():
        if "<subprogram>" not in line:
            continue
        address = re.search(r"low_pc<0x([0-9a-fA-F]+)>", line)
        source = re.search(r"decl_file<0x[0-9a-fA-F]+\s+([^>]+)>", line)
        if address is None or source is None:
            continue
        entry = int(address.group(1), 16)
        if entry in seen_entries:
            continue
        seen_entries.add(entry)
        name = re.search(r"(?:^|\s)name<([^>]+)>", line)
        if name is None:
            name = re.search(r"linkage_name<([^>]+)>", line)
        if name is None:
            name = re.search(r"(?:specification|abstract_origin)<[^>]+> Refers to: ([^>]+)", line)
        result.append(
            {
                "entry": f"0x{entry:x}",
                "name": name.group(1) if name else "<unnamed>",
                "source": normalise_source_path(source.group(1)),
                "original_source": source.group(1),
                "mapping": "function_dwarf",
            }
        )
    return result


def compilation_unit_ranges(elf: Path) -> list[dict[str, object]]:
    """Recover CU address intervals for symbols that have no per-function source DIE."""
    info = command("dwarfdump", "-i", "-d", "-e", str(elf))
    result: list[dict[str, object]] = []
    for line in info.splitlines():
        if "<compile_unit>" not in line:
            continue
        name = re.search(r"\sname<([^>]+)>", line)
        if name is None:
            continue
        ranges = [
            [int(low, 16), int(high, 16)]
            for low, high in re.findall(r"range entry 0x([0-9a-fA-F]+) 0x([0-9a-fA-F]+)", line)
            if int(low, 16) < int(high, 16)
        ]
        if not ranges:
            low = re.search(r"low_pc<0x([0-9a-fA-F]+)>", line)
            high = re.search(r"high_pc<0x([0-9a-fA-F]+)>", line)
            if low is not None and high is not None and int(low.group(1), 16) < int(high.group(1), 16):
                ranges.append([int(low.group(1), 16), int(high.group(1), 16)])
        if ranges:
            result.append(
                {
                    "source": normalise_source_path(name.group(1)),
                    "original_source": name.group(1),
                    "ranges": ranges,
                }
            )
    return result


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("elf", type=Path)
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    args.output.mkdir(parents=True, exist_ok=True)
    units = source_units(args.elf)
    functions = function_index(args.elf)
    ranges = compilation_unit_ranges(args.elf)
    language_counts = Counter(unit["language"] for unit in units)

    (args.output / "compile_units.json").write_text(json.dumps(units, indent=2) + "\n")
    (args.output / "dwarf_functions.json").write_text(json.dumps(functions, indent=2) + "\n")
    (args.output / "compile_unit_ranges.json").write_text(json.dumps(ranges, indent=2) + "\n")
    summary = {
        "input": str(args.elf),
        "compile_units": len(units),
        "functions_with_source": len(functions),
        "compilation_units_with_ranges": len(ranges),
        "languages": dict(sorted(language_counts.items())),
    }
    (args.output / "summary.json").write_text(json.dumps(summary, indent=2) + "\n")
    print(json.dumps(summary, sort_keys=True))


if __name__ == "__main__":
    main()
