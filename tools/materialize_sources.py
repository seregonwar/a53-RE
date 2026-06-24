#!/usr/bin/env python3
"""Place Ghidra function artifacts beneath their original DWARF compilation units."""

from __future__ import annotations

import argparse
import json
import shutil
from collections import Counter, defaultdict
from pathlib import Path


def load_json(path: Path):
    return json.loads(path.read_text())


def load_jsonl(path: Path):
    return [json.loads(line) for line in path.read_text().splitlines() if line.strip()]


def normalise_entry(value: str) -> str:
    return f"0x{int(value, 16):x}"


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument("index", type=Path, help="directory written by export_dwarf_index.py")
    parser.add_argument("ghidra", type=Path, help="directory written by ExportA53.java")
    parser.add_argument("output", type=Path)
    args = parser.parse_args()

    units = load_json(args.index / "compile_units.json")
    dwarf_functions = load_json(args.index / "dwarf_functions.json")
    compilation_unit_ranges = load_json(args.index / "compile_unit_ranges.json")
    ghidra_functions = load_jsonl(args.ghidra / "functions.jsonl")
    ghidra_by_entry = {normalise_entry(record["entry"]): record for record in ghidra_functions}
    source_by_entry = {normalise_entry(record["entry"]): record for record in dwarf_functions}

    grouped: dict[str, list[dict]] = defaultdict(list)
    original_source_by_path: dict[str, str] = {}
    unassigned: list[dict] = []
    direct_mappings = 0
    range_mappings = 0
    for entry, record in sorted(ghidra_by_entry.items()):
        source = source_by_entry.get(entry)
        if source is None:
            address = int(entry, 16)
            candidates = {
                item["source"]
                for item in compilation_unit_ranges
                if any(low <= address < high for low, high in item["ranges"])
            }
            if len(candidates) == 1:
                mapped_path = next(iter(candidates))
                source = next(item for item in compilation_unit_ranges if item["source"] == mapped_path)
                range_mappings += 1
            else:
                unassigned.append(record)
                continue
        else:
            direct_mappings += 1
        grouped[source["source"]].append(record)
        original_source_by_path[source["source"]] = source["original_source"]

    indexed_paths = {unit["path"] for unit in units}
    for path in sorted(set(grouped) - indexed_paths):
        suffix = Path(path).suffix.lower()
        language = ".h (header)" if suffix in {".h", ".hpp"} else "referenced source"
        units.append(
            {
                "path": path,
                "original_path": original_source_by_path[path],
                "language": language,
            }
        )

    args.output.mkdir(parents=True, exist_ok=True)
    written = 0
    for unit in units:
        relative = Path(unit["path"])
        if relative.is_absolute() or ".." in relative.parts:
            raise ValueError(f"unsafe recovered path: {unit['path']}")
        destination = args.output / relative
        destination.parent.mkdir(parents=True, exist_ok=True)
        header = (
            "/*\n"
            " * A53-RE recovered compilation unit.\n"
            f" * Original path: {unit['original_path']}\n"
            f" * Language recorded in DWARF: {unit['language']}\n"
            " * Function bodies below are generated semantic recovery artifacts.\n"
            " * They are intentionally kept separate from hand-validated source.\n"
            " */\n\n"
        )
        snippets: list[str] = []
        for record in grouped.get(unit["path"], []):
            source_path = args.ghidra / record["file"]
            snippets.append(source_path.read_text())
        if not snippets:
            snippets.append("/* No independently addressable function body was recovered for this unit. */\n")
        destination.write_text(header + "\n".join(snippets))
        written += 1

    unassigned_dir = args.output / "_unassigned"
    unassigned_dir.mkdir(exist_ok=True)
    for record in unassigned:
        (unassigned_dir / Path(record["file"]).name).write_text((args.ghidra / record["file"]).read_text())

    coverage = {
        "compile_units_written": written,
        "ghidra_functions": len(ghidra_functions),
        "functions_mapped_to_dwarf_source": len(ghidra_functions) - len(unassigned),
        "functions_mapped_by_function_die": direct_mappings,
        "functions_mapped_by_cu_range": range_mappings,
        "unassigned_functions": len(unassigned),
        "functions_per_unit": dict(sorted((path, len(items)) for path, items in grouped.items())),
    }
    (args.output / "coverage.json").write_text(json.dumps(coverage, indent=2) + "\n")
    metadata = args.output / "metadata"
    metadata.mkdir(exist_ok=True)
    (metadata / "compile_units.json").write_text(json.dumps(units, indent=2) + "\n")
    (metadata / "dwarf_functions.json").write_text(json.dumps(dwarf_functions, indent=2) + "\n")
    type_index = args.ghidra / "types.jsonl"
    if type_index.exists():
        shutil.copyfile(type_index, metadata / "types.jsonl")
    print(json.dumps(coverage, sort_keys=True))


if __name__ == "__main__":
    main()
