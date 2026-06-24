# Canonical source promotion area

Only hand-reviewed C/C++/assembly belongs here. `recovered/` is deliberately
kept as generated evidence and must not be mistaken for source proven to build.

Promotion criteria for a function or type are:

1. its signature and layout agree with `recovered/metadata/types.jsonl`;
2. its generated code is matched to the reference bytes at its original VMA;
3. its section assignment agrees with `linker/a53_layout.ld`;
4. it has no unresolved placeholder types, guessed field offsets, or Ghidra
   undefined aliases.

The current first tranche is listed in `manifest.json`. It is deliberately
semantic-first: it compiles freestanding with the available compiler, while
byte-level matching remains gated on recovering Clang 6.0.0 rather than
rewriting the recovered C to accommodate a newer compiler's control-flow
optimizations.

`verified_functions.json` is stricter: it lists only functions whose canonical
C currently compiles to the exact reference bytes. Run `make
verify-canonical-exact` to enforce that proof.
