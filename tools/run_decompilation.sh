#!/bin/sh
set -eu

ROOT=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
ELF=${1:-"$ROOT/a53.elf"}
GHIDRA_HOME=${GHIDRA_HOME:-"$HOME/.cache/ghidra/ghidra_11.1.2_PUBLIC"}
# Ghidra 11.1.x requires a Java 17 runtime.  Do not inherit JAVA_HOME here:
# callers often use a newer JVM for unrelated projects, and that has caused
# Ghidra's native launcher to crash on this host.  GHIDRA_JAVA_HOME remains
# an explicit opt-in override for a known-good Java 17 installation.
GHIDRA_JAVA_HOME=${GHIDRA_JAVA_HOME:-}
if [ -z "$GHIDRA_JAVA_HOME" ]; then
    GHIDRA_JAVA_HOME=$(/usr/libexec/java_home -v 17 2>/dev/null || true)
fi
JAVA_HOME=$GHIDRA_JAVA_HOME
export JAVA_HOME

if [ ! -f "$ELF" ]; then
    echo "ELF not found: $ELF" >&2
    exit 1
fi
if [ ! -x "$GHIDRA_HOME/support/analyzeHeadless" ]; then
    echo "Ghidra 11.1.2 not found at $GHIDRA_HOME" >&2
    exit 1
fi
if [ -z "$JAVA_HOME" ] || [ ! -x "$JAVA_HOME/bin/java" ]; then
    echo "A Java 17 runtime is required; set GHIDRA_JAVA_HOME to its home." >&2
    exit 1
fi
JAVA_VERSION=$($JAVA_HOME/bin/java -version 2>&1 | sed -n '1p')
case "$JAVA_VERSION" in
    *'version "17.'*|*'version "17"'*) ;;
    *)
        echo "Refusing unsupported Ghidra JVM: $JAVA_VERSION" >&2
        echo "Set GHIDRA_JAVA_HOME to a Java 17 runtime." >&2
        exit 1
        ;;
esac
printf 'Using Ghidra JVM: %s (%s)\n' "$JAVA_HOME" "$JAVA_VERSION"

OUT="$ROOT/out"
RECOVERED="$ROOT/recovered"
mkdir -p "$ROOT/.ghidra" "$OUT/dwarf" "$OUT/ghidra" "$RECOVERED"
python3 "$ROOT/tools/export_dwarf_index.py" "$ELF" "$OUT/dwarf"

"$GHIDRA_HOME/support/analyzeHeadless" "$ROOT/.ghidra" a53 \
    -import "$ELF" -overwrite -max-cpu 4 \
    -scriptPath "$ROOT/ghidra_scripts" \
    -postScript ExportA53.java "$OUT/ghidra" \
    -postScript ExportA53Types.java "$OUT/ghidra" \
    -log "$OUT/ghidra-analysis.log"

python3 "$ROOT/tools/materialize_sources.py" "$OUT/dwarf" "$OUT/ghidra" "$RECOVERED"
python3 "$ROOT/tools/export_assembly.py" "$ELF" "$OUT/dwarf" "$RECOVERED"
