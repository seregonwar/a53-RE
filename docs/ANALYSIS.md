# `a53.elf` evidence report

## Classification

`a53.elf` is an ELF64 little-endian, statically linked AArch64 executable.
Its DWARF producer string is `clang version 6.0.0`.

It is a mixed-language firmware:

| Original language | Compilation units | Evidence |
| --- | ---: | --- |
| C (C99) | 48 | DWARF language tag and paths such as `src/loader/el3/boot.c` |
| C++ | 56 | DWARF language tag, `.cpp` units, and Itanium C++ symbols in namespace `A53` |
| AArch64 assembly | 9 | `.S` units such as `src/loader/el3/el3_vector.S` |

Representative C++ units include the MM and IO controllers under
`src/controller/*`. Representative C loader units include
`src/loader/el3/boot.c`, `mmu.c`, and `gic.c`.

## Reference identity

- File SHA-256: `2b245baa9dec6d7f3e99a9d9ea8584d22bbb5df3f1b332e2bbf7ed239890837c`
- File size: 5,800,424 bytes
- Program entry: `0x0`
- Loadable segments: 10

The final rebuild gate is whole-file SHA-256 equality, not merely successful
compilation or matching exported symbols.

## Recovery evidence retained in the ELF

The ELF is unstripped and retains `.debug_info`, `.debug_line`, `.debug_loc`,
`.debug_ranges`, `.debug_pubnames`, `.debug_pubtypes`, `.symtab`, and `.strtab`.
This gives the recovery process much more than symbol names: compilation-unit
paths, source lines, function ranges, DWARF types, most signatures, local names
and locations, and C++ linkage names are still available.

Ghidra's import reports 113 compilation units, 191,836 DWARF DIEs, and 3,135
DWARF-imported data types. The post-import type inventory contains 5,400 data
types (2,271 composite layouts). The per-function source map and coverage
metrics are written to `recovered/metadata/` and `recovered/coverage.json`.

## What is still required for an identical source rebuild

DWARF does not contain the original token stream or build recipe. In particular,
it cannot restore comments, macro bodies, every inline expansion, conditional
compile selection, exact Clang 6 flags, input static libraries, or linker
scripts. Exact reproduction therefore proceeds in this order:

1. review and promote the semantic artifacts into canonical C/C++/assembly;
2. reconstruct data layouts, headers, macros, and startup/linker scripts;
3. obtain an AArch64 Clang 6-compatible toolchain and match each section's
   placement and alignment;
4. compare each candidate against the reference with `make compare` until the
   whole file hash matches.

The current artifacts are deliberately not presented as a successful rebuild.
