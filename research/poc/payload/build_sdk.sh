#!/usr/bin/env bash
# Build poc_chain.elf via ps5-payload-sdk sysroot.
#
# Uses Homebrew LLVM (llvm@20 or llvm@18) — Apple's clang fork strips out
# the sie-ps5 target, but LLVM 15+ has full upstream support including
# the LLD emulation. The SDK's bin/clang wrapper expects to find
# llvm-config-N on PATH; we set LLVM_CONFIG explicitly and bypass the
# wrapper for the link stage (which fails because the wrapper falls
# through to Apple's lld).
#
# Usage:
#   PS5_PAYLOAD_SDK=/path/to/sdk  bash build_sdk.sh
#   PATH=/usr/local/opt/llvm@20/bin:$PATH  bash build_sdk.sh
#
# Output:
#   research/poc/payload/poc_chain.elf  (x86_64-sie-ps5 ELF)
#   research/poc/build/{poc_sdk.o, main_sdk.o, pal_privilege_sdk.o}

set -euo pipefail

PS5_PAYLOAD_SDK="${PS5_PAYLOAD_SDK:-$(realpath "$(dirname "$0")/../../../external/ps5-payload-sdk")}"
SRCDIR="$(realpath "$(dirname "$0")")"

# Auto-detect brew LLVM (prefer llvm@20 → 18 → 16).
detect_clang() {
    for v in 20 18 16; do
        local cand="/opt/homebrew/opt/llvm@${v}/bin/clang"
        if [ -x "$cand" ]; then
            echo "$cand"
            return 0
        fi
    done
    # Fallback: PATH lookup
    if command -v clang >/dev/null 2>&1 && clang --target x86_64-sie-ps5 -E -x c - < /dev/null >/dev/null 2>&1; then
        command -v clang
        return 0
    fi
    return 1
}

detect_lld() {
    for v in 20 18 16; do
        local cand="/opt/homebrew/opt/llvm@${v}/bin/ld.lld"
        if [ -x "$cand" ]; then
            echo "$cand"
            return 0
        fi
    done
    if command -v ld.lld >/dev/null 2>&1; then
        command -v ld.lld
        return 0
    fi
    return 1
}

CLANG="${CLANG:-$(detect_clang || true)}"
LLD="${LLD:-$(detect_lld || true)}"

if [ -z "$CLANG" ] || [ -z "$LLD" ]; then
    echo "FATAL: Homebrew LLVM (llvm@20 preferred) is required to target x86_64-sie-ps5." >&2
    echo "  brew install llvm@20" >&2
    echo "  PATH=/opt/homebrew/opt/llvm@20/bin:\$PATH  bash build_sdk.sh" >&2
    exit 1
fi

# Wire LLVM_CONFIG so the SDK's bin/clang wrapper (when invoked) finds bindir.
LLVM_BIN_DIR="$(dirname "$CLANG")"
export PATH="$LLVM_BIN_DIR:$PATH"
export LLVM_CONFIG="$LLVM_BIN_DIR/llvm-config"

TARGET="x86_64-sie-ps5"
SYSROOT="$PS5_PAYLOAD_SDK"
INCLUDE_DIR="$PS5_PAYLOAD_SDK/target/include"
LIB_DIR1="$PS5_PAYLOAD_SDK/target/lib"
LIB_DIR2="$PS5_PAYLOAD_SDK/target/user/homebrew/lib"
CRT1="$LIB_DIR1/crt1.o"
ELF="$SRCDIR/poc_chain.elf"
OBJ_DIR="$(realpath "$(dirname "$0")/../build")"

mkdir -p "$OBJ_DIR"

echo "PS5 SDK    : $PS5_PAYLOAD_SDK"
echo "Clang      : $CLANG ($("$CLANG" --version | head -1))"
echo "LLD        : $LLD ($("$LLD" --version | head -1))"
echo "Target     : $TARGET"
echo "Output     : $ELF"
echo ""

# Probe the toolchain (fail fast with a readable error if user's clang is Apple)
echo 'int __probe_main(void){return 0;}' | \
    "$CLANG" --target="$TARGET" --sysroot="$SYSROOT" -isystem "$INCLUDE_DIR" \
        -x c -c - -o /tmp/probe_sdk.o 2>/tmp/probe_err || {
    echo "FATAL: clang cannot target $TARGET with SDK sysroot." >&2
    cat /tmp/probe_err >&2
    exit 1
}

# Note: -fuse-ld is unsupported on clang --target=x86_64-sie-ps5
# ("clang: error: unsupported option '-fuse-ld' for target"), so we
# invoke lld directly only at the link step below.

echo "=== Compile poc.c ==="
"$CLANG" --target="$TARGET" --sysroot="$SYSROOT" -isystem "$INCLUDE_DIR" \
    -O2 -g -Wall -Wextra -std=c11 \
    -I"$SRCDIR" \
    -c "$SRCDIR/poc.c" -o "$OBJ_DIR/poc_sdk.o"

echo "=== Compile main.c ==="
"$CLANG" --target="$TARGET" --sysroot="$SYSROOT" -isystem "$INCLUDE_DIR" \
    -O2 -g -Wall -Wextra -std=c11 \
    -I"$SRCDIR" \
    -c "$SRCDIR/main.c" -o "$OBJ_DIR/main_sdk.o"

echo "=== Compile pal_privilege.c ==="
"$CLANG" --target="$TARGET" --sysroot="$SYSROOT" -isystem "$INCLUDE_DIR" \
    -O2 -g -Wall -Wextra -std=c11 \
    -I"$SRCDIR" \
    -c "$SRCDIR/pal_privilege.c" -o "$OBJ_DIR/pal_privilege_sdk.o"

# IMPORTANT: pass --no-pie (do NOT pass -pie).
# The SDK sample (samples/hello_world/Makefile) invokes clang with
# "$(CC) $(CFLAGS) -o $@ $^" — no -pie — and produces an ET_EXEC ELF.
# We had been passing -pie to lld, producing an ET_DYN ELF; the on-console
# crash was a SIGSEGV "user read instruction, protection violation" with
# RIP and branch trace inside the payload's mapped range (consistent with
# the loader mishandling a PIE bit, the .text tail landing in a non-exec
# PT_LOAD, or the loader refusing PROT_EXEC on relocatable pages). Hardening
# flags (-z noexecstack / -z relro / -z now) were intentionally NOT added
# here: PS5's FreeBSD-derived libc may not honor -z now at load time, and
# any speculative flag risks a different failure class. Re-add them only
# after on-console regression confirms the chain still runs.
# Why we use the clang driver (NOT direct lld) for the link step:
#
# - Earlier rounds manually passed `--no-pie`, `-z noseparate-code`,
#   `-z separate-loadable-segments`, `-z max-page-size=0x4000`,
#   `-z common-page-size=0x4000`, `--image-base=0x4000` to lld and
#   elfldr STILL rejected with `pt_mmap: Invalid argument`. Without
#   the upstream clang driver's x86_64-sie-ps5 target spec, we were
#   guessing at which combination elfldr accepts.
# - The SDK's samples build cleanly via
#   `$(CC) $(CFLAGS) -o elf main.c`, which goes through the clang
#   driver. That driver injects the canonical lld flags for sie-ps5:
#     -m elf_x86_64_fbsd --default-script main.script --lto=full
#     -pie --eh-frame-hdr --build-id=uuid -z now -z rodynamic
#     -z common-page-size=0x4000 -z max-page-size=0x4000
#     plus crt1.o / crti.o / crtbegin.o / crtend.o / crtn.o from
#     $SDK/target/lib.
# - To inherit those defaults, we use `$CLANG --target=$TARGET` for
#   the link step. The driver invokes a target-named linker
#   (`prospero-lld`); we satisfy it with a local symlink to brew lld.
#
# The symlink lives in a per-build $OBJ_DIR/sdk-tools dir so rebuilds
# don't leak between runs.
SDK_TOOL_DIR="$OBJ_DIR/sdk-tools"
rm -rf "$SDK_TOOL_DIR"
mkdir -p "$SDK_TOOL_DIR"
# Wrapper SCRIPT (not a symlink): brew ships ld.lld as a symlink to the
# multi-driver `lld` binary, which self-identifies by argv[0] basename
# and refuses to run unless invoked via one of {ld.lld, ld64.lld,
# lld-link, wasm-ld}. A naked `prospero-lld -> ld.lld -> lld` chain
# gets bash to deref to lld and exec with argv[0] set in a way the
# self-id check rejects ("lld is a generic driver..."). A real script
# that `exec "$LLD" "$@"` replaces the script's own process with the
# actual ld.lld binary, whose argv[0] basename matches the self-id
# check's accepted set.
cat >"$SDK_TOOL_DIR/prospero-lld" <<EOF
#!/usr/bin/env bash
exec "$LLD" "\$@"
EOF
chmod +x "$SDK_TOOL_DIR/prospero-lld"

echo "=== Link ELF via clang driver (inherits sie-ps5 target defaults) ==="
PATH="$SDK_TOOL_DIR:$LLVM_BIN_DIR:$PATH" \
    "$CLANG" --target="$TARGET" --sysroot="$SYSROOT" \
        -L"$LIB_DIR1" -L"$LIB_DIR2" \
        "$OBJ_DIR/poc_sdk.o" "$OBJ_DIR/main_sdk.o" "$OBJ_DIR/pal_privilege_sdk.o" \
        -lc -lSceLibcInternal -lSceNet -lkernel_web \
        -o "$ELF"

echo ""
echo "=== Done ==="
file "$ELF"
ls -la "$ELF"
echo ""
echo "Symbols (poc_*):"
nm -D "$ELF" 2>/dev/null | grep -i 'poc_' | head -20 || nm "$ELF" | grep 'poc_' | head -20
