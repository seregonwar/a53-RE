# A53 firmware source recovery

This repository is a reproducible source-recovery project for `a53.elf`.
It is intentionally evidence-first: the original firmware retains full DWARF
debug data, so file names, C/C++ classification, function names, signatures,
types, source locations, and memory layout are recovered before producing any
pseudocode.

## What the input establishes

- ELF64 little-endian, AArch64, static executable.
- Built with `clang version 6.0.0`.
- 113 compilation units: 48 C files, 56 C++ files, and 9 AArch64 assembly files.
- The original tree was rooted at `a53/src/`; it includes an EL3 loader and C++
  MM/IO controller implementations.

This is therefore a mixed C/C++/assembly firmware, not a C-only program.

## Recovery workflow

`make analyze` uses the installed Ghidra 11.1.2 release under Java 17, then:

1. extracts a DWARF compile-unit/function index to `out/dwarf/`;
2. exports a semantic C-like body for every Ghidra function to `out/ghidra/`;
3. materialises those artifacts beneath their original DWARF paths in
   `recovered/src/` and writes coverage data.

The generated C/C++ files are deliberately labelled **recovery artifacts**:
they are suitable for review and iterative correction, but do not claim to be
the original compilable source. Decompiler output alone cannot recover erased
macro definitions, comments, precise headers, inline code, compiler flags, or
linker scripts.

## Exact rebuild acceptance criterion

Use `make verify` to record the reference hash and `make compare
CANDIDATE=build/a53.elf` for the actual acceptance test. A rebuild only counts
as exact when the complete ELF hash matches; the script also reports each
`PT_LOAD` segment comparison to localise differences first.

Achieving this requires reconstruction of the original Clang 6 toolchain,
linker script, all startup assembly, headers/macros, static libraries, and
build flags. This repository does not substitute a copied reference ELF for a
source build.

For the current evidence-backed reproduction status, including the separately
verified lossless runtime image, see [docs/REPRODUCTION.md](docs/REPRODUCTION.md).
