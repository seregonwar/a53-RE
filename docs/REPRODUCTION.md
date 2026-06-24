# Reproduction status

## Evidence-backed results

The repository has two complementary representations of the firmware.

1. `recovered/` is the semantic recovery: 1,858 Ghidra-decompiled functions,
   all placed in a source/header/assembly path using DWARF function DIEs or,
   where the DIE omits `decl_file`, a non-overlapping compilation-unit range.
   The export also contains 5,400 recovered Ghidra data types, including 2,271
   composite layouts.
2. `lossless/sections/` is the bit-preserving representation: nine allocated
   PROGBITS sections totaling 590,648 bytes, emitted as AArch64 assembly or
   data directives.

`make verify-assembly` recompiles the nine original `.S` compilation units and
proves their 4,025 emitted instruction words match the reference. `make
verify-lossless` recompiles every lossless section and proves every emitted
section equals the original bytes.

The canonical C promotion has its own narrower proof gate. `make
verify-canonical-exact` currently recompiles and exactly matches
`get_dev_context`, `spc_begin`, `spc_putchar`, and
`dev_context_init_for_el3`. The latter also demonstrates that DWARF-derived
types plus explicit AArch64 register constraints can replace a lossless
assembly implementation without changing bytes.

`make build-lossless` links the lossless sections with
`linker/a53_layout.ld`. Its generated `build/a53.loadable.elf` has all ten
runtime `PT_LOAD` mappings equal to the reference: VMA, physical address,
flags, alignment, initialized byte content, file size and memory size.

## What remains before whole-file equality

The lossless loadable image is deliberately smaller than the reference ELF. It
does not regenerate the original DWARF, symbol table, string tables or the
original file offsets of later load segments; hence its whole-file SHA-256 is
not equal. That distinction matters: equality of loaded firmware state is
already proven, while equality of the complete debugging container still needs
an exact Clang 6 build recipe and a source reconstruction that regenerates the
same DWARF.

The canonical path is therefore to replace one lossless section at a time with
reviewed C/C++/assembly under `canonical/`, comparing each result at its VMA,
without weakening the `PT_LOAD` equality gate.
