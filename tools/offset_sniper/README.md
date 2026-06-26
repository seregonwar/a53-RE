# Ghostpad VDA Probe

Read-only PS4/PS5 diagnostic payload for collecting firmware-specific data before
writing any VDA/libScePad patch.

It does **not** patch memory. It only:

- finds `SceShellCore` / `SceShellUI` pids;
- resolves selected `libScePad`, `libSceMbus`, and `libkernel` symbols;
- reads small byte windows around resolved functions;
- prints FNV-1a fingerprints and prologue bytes;
- scans `scePadVirtualDeviceAddDevice` for candidate call/branch patch sites;
- ranks nearby cave/padding candidates using the EAP scanner's entropy/scoring style;
- saves the report to `/data/ghostpad/vda_probe_report.txt` and `/mnt/usb0/vda_probe_report.txt` when possible;
- serves the report once deployed on TCP port `6975`.

## Build and deploy

PS4:

```sh
cd tools/vda_probe
PS4_PAYLOAD_SDK=/path/to/ps4-payload-sdk make clean all
PS4_PAYLOAD_SDK=/path/to/ps4-payload-sdk make deploy PS4_HOST=<ps4-ip>
```

PS5:

```sh
cd tools/vda_probe
PS5_PAYLOAD_SDK=/path/to/ps5-payload-sdk make clean all
PS5_PAYLOAD_SDK=/path/to/ps5-payload-sdk make deploy PS5_HOST=<ps5-ip>
```

## Retrieve the report

The payload keeps a small TCP server open for about three minutes:

```sh
nc <console-ip> 6975 > vda-probe-report.txt
```

The same report is also mirrored to klog in shorter lines and saved locally:

```text
/data/ghostpad/vda_probe_report.txt
/mnt/usb0/vda_probe_report.txt
```

## How to use the report

Use the report to build a firmware-specific patch manifest. A safe patch should
match at least:

- platform (`ORBIS` / `PROSPERO`);
- target process;
- module handle/base-adjacent fingerprints;
- exact symbol address;
- prologue bytes;
- 256-byte and 4K FNV fingerprints;
- candidate pattern offsets.

If any fingerprint or prologue differs, do not patch automatically.

## EAP scanner adaptation

This version reuses the useful architecture from the older EAP scanner: bounded
read-only discovery, ranked candidates, persistent report files, USB mirroring,
and notification fallback.  It intentionally does **not** copy the EAP-specific
key dumping logic and does not print or write secrets.

The `VDA_CAVE_CANDIDATE` rows are not permission to patch. They are only inputs
for a firmware-specific manifest. A patcher should still require exact matching
of platform, symbol address, prologue bytes, 256-byte hash, 4K hash, and selected
candidate offsets.
