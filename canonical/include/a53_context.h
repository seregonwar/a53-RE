#ifndef A53_CONTEXT_H
#define A53_CONTEXT_H

/*
 * Umbrella include for all A53 canonical headers.
 * 
 * Include order matters -- sub-headers may depend on types from earlier ones.
 * Prefer including only the specific a53/<header>.h you need in new code.
 */

/* Foundation */
#include "a53_abi.h"
#include "a53/types.h"

/* MMIO primitives (needed by msi, syshub) */
#include "a53/mmio.h"

/* String/memory runtime */
#include "a53/string.h"

/* Loader core (dev_context, debug_status, putchar, boot, peripherals) */
#include "a53/loader.h"

/* MSI and parameter block */
#include "a53/msi.h"

/* GIC interrupt controller */
#include "a53/gic.h"

/* Syshub IOMMU */
#include "a53/syshub.h"

/* MMU and EL0 support */
#include "a53/mmu.h"

/* DECI protocol (SHM, target, deci5s, SDBGP) */
#include "a53/deci.h"

/* Runtime helpers (aarch64 decode, cache ops, PMU) */
#include "a53/runtime.h"

#endif
