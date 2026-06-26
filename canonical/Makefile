# Canonical source syntax and ABI check
# Target: make check-canonical
# Uses host clang (Apple Silicon = aarch64 native) with -fsyntax-only

HOST_OS  := $(shell uname -s)
# Apple clang rejects unknown GCC-style warning flags silently; -Wno-unknown-warning-option
# suppresses the diagnostic for any flag we might add in the future that Apple clang
# doesn't recognise. Linux/BSD clang accepts it transparently.
EXTRA_WARNINGS_QUIET := $(if $(filter Darwin,$(HOST_OS)),-Wno-unknown-warning-option,)

CC       := clang
CXX      := clang++
CFLAGS   := -fsyntax-only -Wall -Wextra $(EXTRA_WARNINGS_QUIET) -ffreestanding -fno-builtin \
            -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function \
            -Wno-unused-but-set-variable -Wno-sign-compare \
            -I canonical/include -std=c11
CXXFLAGS := -fsyntax-only -Wall -Wextra $(EXTRA_WARNINGS_QUIET) -ffreestanding -fno-builtin \
            -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function \
            -Wno-unused-but-set-variable -Wno-sign-compare \
            -I canonical/include -std=c++11

# Find all source files
C_SRCS   := $(shell find canonical/src -name '*.c' 2>/dev/null)
CXX_SRCS := $(shell find canonical/src -name '*.cpp' 2>/dev/null)

.PHONY: check-canonical clean

check-canonical:
	@echo "=== A53 Canonical: Syntax & ABI Check ==="
	@echo "C sources:     $(words $(C_SRCS))"
	@echo "C++ sources:   $(words $(CXX_SRCS))"
	@echo ""
	@failures=0; total=0; \
	for f in $(C_SRCS); do \
		total=$$((total + 1)); \
		if $(CC) $(CFLAGS) "$$f" >/dev/null 2>&1; then \
			echo "  OK  $$f"; \
		else \
			echo "  FAIL $$f"; \
			$(CC) $(CFLAGS) "$$f" 2>&1 | while IFS= read -r line; do echo "       $$line"; done; \
			failures=$$((failures + 1)); \
		fi; \
	done; \
	for f in $(CXX_SRCS); do \
		total=$$((total + 1)); \
		if $(CXX) $(CXXFLAGS) "$$f" >/dev/null 2>&1; then \
			echo "  OK  $$f"; \
		else \
			echo "  FAIL $$f"; \
			$(CXX) $(CXXFLAGS) "$$f" 2>&1 | while IFS= read -r line; do echo "       $$line"; done; \
			failures=$$((failures + 1)); \
		fi; \
	done; \
	echo ""; \
	if [ $$failures -eq 0 ]; then \
		echo "=== RESULT: $$total/$$total files passed ==="; \
	else \
		echo "=== RESULT: $$((total - failures))/$$total passed, $$failures FAILED ==="; \
		exit 1; \
	fi

clean:
	@rm -f canonical/src/**/*.syntax_ok
