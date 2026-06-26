# SPDX-License-Identifier: GPL-3.0-or-later
# Ghostpad VDA Probe - read-only PS4/PS5 diagnostic payload

PS4_HOST ?= ps4
PS5_HOST ?= ps5
PORT     ?= 9021
TARGET   ?= vda_probe.elf

ifdef PS4_PAYLOAD_SDK
    include $(PS4_PAYLOAD_SDK)/toolchain/orbis.mk
    CFLAGS += -D__ORBIS__
    DEPLOY := $(PS4_DEPLOY)
    HOST   := $(PS4_HOST)
else ifdef PS5_PAYLOAD_SDK
    include $(PS5_PAYLOAD_SDK)/toolchain/prospero.mk
    CFLAGS += -D__PROSPERO__
    DEPLOY := $(PS5_PAYLOAD_SDK)/bin/prospero-deploy
    HOST   := $(PS5_HOST)
else
    $(error Neither PS4_PAYLOAD_SDK nor PS5_PAYLOAD_SDK is defined)
endif

CFLAGS  += -Wall -Wextra -O2 -g -fPIC -fno-stack-protector $(EXTRA_CFLAGS)
LDFLAGS += -lpthread

.PHONY: all clean deploy test

all: $(TARGET)

$(TARGET): vda_probe.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f $(TARGET)

deploy test: $(TARGET)
	$(DEPLOY) -h $(HOST) -p $(PORT) $<
