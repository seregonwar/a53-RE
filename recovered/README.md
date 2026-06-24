# Recovered source artifacts

This directory is generated from DWARF preserved in `a53.elf` plus Ghidra's
AArch64 decompiler. It preserves the source-tree partition before manual
rewriting.

- `src/` contains all 113 compilation units recorded by DWARF: C, C++, and
  AArch64 assembly.
- `external/` contains inline/header-defined functions located outside
  `a53/src/` in the original build tree.
- `_unassigned/` is retained for future imports; the current export maps all
  1,858 recovered function starts using either an exact function DIE or a
  non-overlapping DWARF compilation-unit address range.
- `metadata/` contains the compile-unit, range and function-to-source indexes.
- `coverage.json` records the assignment method for every recovered function.

These are **semantic recovery artifacts**, not yet the original preprocessed
source. Promote reviewed functions into a separate canonical source tree only
after reconstructing headers, macros, linker scripts, and the original Clang 6
code-generation constraints.
