#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ELF=${1:-"$ROOT/a53.elf"}
BUILD="$ROOT/build/lossless"
mkdir -p "$BUILD"

for source in "$ROOT"/lossless/sections/*.S; do
    base=$(basename "$source" .S)
    clang --target=aarch64-none-elf -c "$source" -o "$BUILD/$base.o"
done

ld.lld -m aarch64elf -z max-page-size=0x10000 -T "$ROOT/linker/a53_layout.ld" \
    "$BUILD"/*.o -o "$BUILD/a53.loadable.unpatched.elf"
python3 "$ROOT/tools/normalise_bss_program_headers.py" "$ELF" \
    "$BUILD/a53.loadable.unpatched.elf" "$ROOT/build/a53.loadable.elf"
python3 "$ROOT/tools/verify_reference.py" "$ELF" "$ROOT/build/a53.loadable.elf" --require runtime
