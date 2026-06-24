# Lossless section source

The `.S` files in `sections/` encode every byte of each allocated PROGBITS
section from `a53.elf`. Executable data is emitted as instruction words; data
is emitted as `.byte`. This layer is a byte-preserving assembly representation,
not a substitute for the semantic C/C++ recovered under `recovered/`.

`make verify-lossless` recompiles every file and compares each object section
with the corresponding section in the reference ELF. It is a strict code/data
preservation gate while the high-level canonical implementation evolves.

`make build-lossless` links those sections using the recovered layout. Its
acceptance criterion is equality of all ten runtime `PT_LOAD` mappings and
bytes; it does **not** claim whole-file identity, since the original debug and
symbol tables are not regenerated yet.
