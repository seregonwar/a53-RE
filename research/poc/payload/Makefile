# PoC Chain Payload Makefile
# Requires ps5-payload-sdk: set PS5_PAYLOAD_SDK environment variable
#
# Build:   PS5_PAYLOAD_SDK=/path/to/sdk make
# Deploy:  make deploy-nc PS5_HOST=192.168.x.x
# Clean:   make clean

PS5_HOST ?= 192.168.1.42
PS5_PORT ?= 9021
# Default to `nc` (works on BSD/Linux/OpenBSD). On macOS hosts the bundled
# GNU nc won't accept `-N`; override with `make deploy-nc DEPLOY=nc.openbsd`.
DEPLOY   ?= nc

.PHONY: all clean test deploy deploy-nc deploy-i

ifdef PS5_PAYLOAD_SDK
    include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
else
    $(error PS5_PAYLOAD_SDK is undefined. Set it to the ps5-payload-sdk path. Run `make doctor` for toolchain sanity check.)
endif

# LLVM_CONFIG auto-detect with brew fallback (mirrors zftpd's prospero.mk guard).
# prospero.mk requires LLVM_CONFIG; on macOS the SDK wrapper cannot auto-find
# Homebrew's llvm-config, so this block removes that footgun and emits a
# readable error if LLVM is genuinely missing.
LLVM_CONFIG ?=
ifeq ($(LLVM_CONFIG),)
    LLVM_CONFIG := $(shell command -v llvm-config-21 2>/dev/null || \
                              command -v llvm-config-20 2>/dev/null || \
                              command -v llvm-config-19 2>/dev/null || \
                              command -v llvm-config-18 2>/dev/null || \
                              command -v llvm-config-17 2>/dev/null || \
                              command -v llvm-config-16 2>/dev/null || \
                              command -v llvm-config-15 2>/dev/null || \
                              command -v llvm-config 2>/dev/null)
endif
ifeq ($(LLVM_CONFIG),)
    LLVM_CONFIG := $(shell if command -v brew >/dev/null 2>&1; then \
                                   p=$$(brew --prefix llvm@20 2>/dev/null); \
                                   if [ -x "$$p/bin/llvm-config" ]; then echo "$$p/bin/llvm-config"; fi; \
                                   p=$$(brew --prefix llvm 2>/dev/null); \
                                   if [ -x "$$p/bin/llvm-config" ]; then echo "$$p/bin/llvm-config"; fi; \
                                fi)
endif
ifeq ($(LLVM_CONFIG),)
    $(error llvm-config non trovato: installa LLVM (es. `brew install llvm`) e assicurati che llvm-config sia nel PATH oppure passa LLVM_CONFIG=/percorso/a/llvm-config, oppure brew install lld per il linker pair)
endif
export LLVM_CONFIG

# macOS Homebrew's llvm@* formula does NOT bundle ld.lld (the standalone
# `brew install lld` formula ships it). prospero-lld invokes
#   $(llvm-config --bindir)/ld.lld
# so it requires ld.lld inside the LLVM bindir. We do NOT auto-symlink
# at parse time: `$(shell ln -sf ...)` would write to Homebrew's
# /opt/homebrew/Cellar on every `make help`/`make -n` invocation and
# trigger `brew doctor` complaints.
#
# Instead:
#   - `make install-toolchain`     Idempotent: runs `brew install lld` if
#                                  needed, then symlinks ld.lld into $(LLVM_BINDIR)
#   - `make uninstall-toolchain`   Removes the symlink (brew doctor rollback)
#   - Build-time guard             `$(ld_lld_guard)` fails the link step
#                                  with a one-line pointer to install-toolchain
LLVM_BINDIR := $(shell $(LLVM_CONFIG) --bindir 2>/dev/null)
# Shell-side test at link time (not parse-time `:= $(shell ...)` — that
# would cache the answer before any prerequisite installs can populate
# the symlink, breaking `make install-toolchain all`). Evaluates against
# current filesystem state every recipe invocation, so the chain
# `install-toolchain -> $(ELF)` works in a single make invocation.
ld_lld_guard = @if [ ! -e "$(LLVM_BINDIR)/ld.lld" ]; then \
	echo "error: $(LLVM_BINDIR)/ld.lld missing or unbrowsable" >&2; \
	echo "       run 'make install-toolchain' first (or 'brew install lld' + retry)" >&2; \
	exit 1; \
	fi

.PHONY: install-toolchain uninstall-toolchain

# Idempotent link of the standalone lld's ld.lld into the LLVM bindir
# that prospero-lld looks in. Runs `brew install lld` first if missing.
install-toolchain:
	@command -v brew >/dev/null 2>&1 || { echo "error: brew not found in PATH" >&2; exit 1; }
	@L=$$(brew --prefix lld 2>/dev/null)/bin/ld.lld; \
	if [ ! -e "$$L" ]; then \
		echo "[payload] 'brew install lld' — running"; \
		brew install lld; \
		L=$$(brew --prefix lld 2>/dev/null)/bin/ld.lld; \
	fi; \
	if [ -e "$$L" ]; then \
		ln -sf "$$L" "$(LLVM_BINDIR)/ld.lld"; \
		echo "[payload] symlinked $$L -> $(LLVM_BINDIR)/ld.lld"; \
	else \
		echo "error: ld.lld still missing after 'brew install lld' — check manually" >&2; \
		exit 1; \
	fi

# Reverse of install-toolchain: removes the symlink so brew doctor won't
# complain. Safe to re-run; no-op if not a symlink.
uninstall-toolchain:
	@if [ -L "$(LLVM_BINDIR)/ld.lld" ]; then \
		rm -v "$(LLVM_BINDIR)/ld.lld"; \
	else \
		echo "[payload] $(LLVM_BINDIR)/ld.lld is not a symlink — leaving alone"; \
	fi

ELF := poc_chain.elf
SRCS := main.c poc.c pal_privilege.c
OBJS := $(SRCS:.c=.o)

# Toolchain (prospero.mk) already sets CC, CFLAGS, LDFLAGS, LDLIBS for
# the PS5 target. We only add our local include path so that the toolchain
# flags drive everything else. Use override to extend safely.
# -fno-lto keeps the .o files as plain ELF (no LLVM bitcode) so the
# clang driver's default --lto=full injection doesn't reject them.
override CFLAGS += -I. -fno-lto

# The prospero-clang and prospero-lld wrappers (via -Wl,...) auto-inject
# the canonical PS5 SDK flag set the elfldr saga needs:
#   -target x86_64-sie-ps5, -isysroot/--sysroot, crt1.o/crti.o/crtn.o
#     auto-link on clang-20+, -lc/-lSceLibcInternal/-lSceNet/-lkernel_web
#   -m elf_x86_64, -pie, -T $(PS5_PAYLOAD_SDK)/ldscripts/elf_x86_64.x,
#     --eh-frame-hdr, -z max-page-size=0x4000, --hash-style=gnu
# Re-stating them here either duplicates them or (worse) clobbers them.
# For example, manually specifying -m elf_x86_64_fbsd would override
# the wrapper's elf_x86_64 and produce an EM header elfldr rejects
# (it expects the SDK's exact binfmt), and --default-script=...
# pointing at target/lib/main.script would override the wrapper's
# ldscripts/elf_x86_64.x (the wrong file in any case).
#
# We only carry -fno-lto (kept explicit at link too in case CFLAGS gets
# overridden by future edits). -v prints the actual lld invocation to
# stderr — operator can confirm the SDK driver is doing the right thing
# without detouring through build_sdk.sh.
#
# If the resulting ELF still fails elfldr's pt_mmap (cf. 6 rounds of
# EINVAL), fall back to running `bash build_sdk.sh` for a known-good
# baseline while debugging — do not re-edit this block without first
# diffing the produced poc_chain.elf against the build_sdk.sh baseline.
# Gate the noisy `-v` (prints the full lld subcommand dump) behind
# `make LINK_VERBOSE=1`. Default is silent — the build only emits the
# ELF, subcommands stay out of stderr unless explicitly asked for.
LINK_VERBOSE ?= 0
ifeq ($(LINK_VERBOSE),1)
    ELF_PRODUCTION_FLAGS = -fno-lto -v
else
    ELF_PRODUCTION_FLAGS = -fno-lto
endif

all: install-toolchain $(ELF)

# Clang driver automatically wraps $(OBJS) in crt1.o/crti.o/crtbegin.o
# and appends crtend.o/crtn.o via the SDK sysroot. Do not specify them
# manually — manual ordering of crt*.o was the source of 6 rounds of
# elfldr pt_mmap EINVAL with our previous direct-lld invocations.
$(ELF): $(OBJS)
	$(ld_lld_guard)
	$(CC) $(ELF_PRODUCTION_FLAGS) -o $@ $^

%.o: %.c poc.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(ELF) $(OBJS)

# Deploy variants — each emits an explicit error message pointing at the
# fix (the legacy `test` shortcut still works but is documented as such).
.PHONY: deploy deploy-nc deploy-i test

# Legacy short alias used in earlier docs/scripts.
test: deploy-nc

# Default deploy: any user-specified $(DEPLOY) tool. Falls through to nc by default.
deploy: $(ELF)
	@echo "Deploying $(ELF) to $(PS5_HOST):$(PS5_PORT) via $(DEPLOY)..."
	@command -v $(DEPLOY) >/dev/null 2>&1 || { \
		echo "error: '$(DEPLOY)' non trovato nel PATH (install nc / brew install netcat / override DEPLOY=...)" >&2; exit 1; }
	$(DEPLOY) -N $(PS5_HOST) $(PS5_PORT) < $(ELF)

# BSD/OpenBSD nc path (most Linux distros / `brew install netcat`).
deploy-nc: $(ELF)
	@command -v $(DEPLOY) >/dev/null 2>&1 || { \
		echo "error: '$(DEPLOY)' non trovato (installa netcat: brew install netcat)" >&2; exit 1; }
	@echo "Sending $(ELF) to $(PS5_HOST):$(PS5_PORT) via nc..."
	$(DEPLOY) -N $(PS5_HOST) $(PS5_PORT) < $(ELF)

# HEN/prospero-deploy interactive path (no shell pipe needed).
deploy-i: $(ELF)
	@command -v prospero-deploy >/dev/null 2>&1 || \
		{ echo "error: prospero-deploy non trovato (is PS5_PAYLOAD_SDK on PATH?)" >&2; exit 1; }
	$(PS5_DEPLOY) -h $(PS5_HOST) -p $(PS5_PORT) -i $(ELF)

.PHONY: all clean test deploy-nc deploy-i
