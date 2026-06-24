ELF ?= a53.elf

.PHONY: analyze metadata verify verify-assembly export-lossless verify-lossless build-lossless check-canonical test-canonical verify-canonical-exact compare

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
