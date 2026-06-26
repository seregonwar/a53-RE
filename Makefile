ELF ?= a53.elf

# Host detection — only used by `doctor` and CFLAGS guards; the rest of the
# analysis pipeline is host-agnostic (Python + scripts).
HOST_OS := $(shell uname -s)

.PHONY: analyze metadata verify verify-assembly export-lossless verify-lossless build-lossless check-canonical test-canonical verify-canonical-exact compare doctor doctor-ps5 help

analyze:
	./tools/run_decompilation.sh "$(ELF)"

metadata:
	python3 tools/export_dwarf_index.py "$(ELF)" out/dwarf

verify:
	python3 tools/verify_reference.py "$(ELF)"

verify-assembly:
	python3 tools/verify_assembly.py "$(ELF)" out/dwarf recovered

export-lossless:
	python3 tools/export_lossless_sections.py "$(ELF)" lossless/sections

verify-lossless:
	python3 tools/verify_lossless_sections.py "$(ELF)" lossless/sections

build-lossless:
	./tools/build_lossless.sh "$(ELF)"

check-canonical:
	@for source in $$(find canonical/src -type f -name '*.c' | sort); do \
		clang --target=aarch64-none-elf -ffreestanding -fno-builtin -fno-stack-protector -Oz \
			-fsyntax-only -Icanonical/include "$$source"; \
	done

test-canonical:
	@mkdir -p build/canonical
	clang -std=c11 -Wall -Wextra -Werror -fno-builtin -Icanonical/include \
		canonical/tests/loader_dev_test.c canonical/src/loader/dev/bzero.c canonical/src/loader/dev/ctype.c \
		canonical/src/loader/dev/memcpy.c canonical/src/loader/dev/strncpy.c canonical/src/loader/dev/strnlen.c \
		-o build/canonical/loader_dev_test
	build/canonical/loader_dev_test

verify-canonical-exact:
	python3 tools/verify_canonical_exact.py "$(ELF)" canonical tools/compare_function_bytes.py

compare:
	@test -n "$(CANDIDATE)" || (echo "Use: make compare CANDIDATE=path/to/a53.elf" >&2; exit 2)
	python3 tools/verify_reference.py "$(ELF)" "$(CANDIDATE)" --require file

# ============================================================================
# doctor — toolchain sanity check (mirrors zftpd's doctor-ps4 style).
# Print OK / N/A for each prerequisite. Catches brew/LLVM_CONFIG/lld mismatches
# at the source instead of after a 2-minute rebuild that crashes the link.
# ============================================================================
doctor: doctor-ps5

doctor-ps5:
	@echo "=== Toolchain prerequisites ==="
	@echo "host os:            $(HOST_OS)"
	@printf "python3:            "; command -v python3 >/dev/null 2>&1 && echo "OK ($$(command -v python3))" || echo "MISSING"
	@printf "clang:              "; command -v clang >/dev/null 2>&1 && echo "OK ($$(command -v clang))" || echo "MISSING"
	@printf "auto-detect llvm-config:\n"
	@for c in llvm-config-21 llvm-config-20 llvm-config-19 llvm-config-18 llvm-config-17 llvm-config-16 llvm-config-15 llvm-config; do \
		if command -v $$c >/dev/null 2>&1; then echo "    OK  $$c ($$(command -v $$c))"; break; fi; \
	done
	@if command -v brew >/dev/null 2>&1; then \
		p=$$(brew --prefix llvm@20 2>/dev/null); \
		[ -x "$$p/bin/llvm-config" ] && echo "    OK  brew llvm@20 ($$p/bin/llvm-config)" || echo "    N/A brew llvm@20 not installed"; \
		p=$$(brew --prefix lld 2>/dev/null); \
		[ -x "$$p/bin/ld.lld" ] && echo "    OK  brew lld ($$p/bin/ld.lld)" || echo "    N/A brew lld not installed"; \
	else \
		echo "    N/A brew not on PATH"; \
	fi
	@printf "ld.lld on PATH:     "; command -v ld.lld >/dev/null 2>&1 && echo "OK ($$(command -v ld.lld))" || echo "MISSING"
	@printf "ps5-payload-sdk:    "; [ -d external/ps5-payload-sdk ] && echo "OK (external/ps5-payload-sdk)" || echo "MISSING (expected submodule at external/ps5-payload-sdk)"
	@printf "netcat (nc):        "; command -v nc >/dev/null 2>&1 && echo "OK ($$(command -v nc))" || echo "N/A (only needed for `make test`)"
	@echo ""
	@echo "Tip on macOS: brew install llvm lld  (then export PATH=\"/opt/homebrew/opt/llvm/bin:/opt/homebrew/opt/lld/bin:\$$PATH\")"

help:
	@echo "a53-RE build/verification orchestrator"
	@echo ""
	@echo "Targets:"
	@echo "  doctor            Toolchain sanity check (start here)"
	@echo "  check-canonical   Syntax + ABI check on canonical/  (49 expected)"
	@echo "  test-canonical    Link + run the loader_dev_test binary"
	@echo "  analyze           Run decompilation via tools/run_decompilation.sh"
	@echo "  metadata          Export DWARF index to out/dwarf/"
	@echo "  verify            Run a53.elf against the canonical reference"
	@echo "  verify-assembly   Per-function assembly verify (canonical vs recovered)"
	@echo "  export-lossless   Build the lossless/ tree from a53.elf"
	@echo "  verify-lossless   Round-trip the lossless/ tree"
	@echo "  build-lossless    Rebuild the lossless sections via build_lossless.sh"
	@echo "  compare CANDIDATE=path/to/a53.elf   Two-ELF byte comparison"
	@echo ""
	@echo "Sub-Makefiles:"
	@echo "  make -C canonical                   Syntax check canonical/"
	@echo "  make -f research/poc/build/Makefile Host cross-compile sanity"
	@echo "  PS5_PAYLOAD_SDK=... make -C research/poc/payload   Build the PS5 payload"
