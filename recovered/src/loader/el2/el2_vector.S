/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/el2/el2_vector.S
 * Section virtual-address base: 0x00124000
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_el2_el2_vector.S, "ax", %progbits
.p2align 2
.globl __loader_dev_bss_begin
.globl __loader_dev_bss_end
.globl __loader_dev_data_begin
.globl __loader_dev_data_end
.globl __loader_el2_begin
.globl _loader_el2_vector

/* original range [0x00124000, 0x001247f8) */
.org 0x0
EL2_VBAR:
__loader_dev_bss_begin:
__loader_dev_bss_end:
__loader_dev_data_begin:
__loader_dev_data_end:
__loader_el2_begin:
_loader_el2_vector:
    .inst 0xd4040003 /* 0x00124000: smc #0x2000 */
    .inst 0xd51bd052 /* 0x00124004: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124008: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012400c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124010: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124014: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124018: mov x0, x18 */
    .inst 0xd2800001 /* 0x0012401c: mov x1, #0 */
    .inst 0xd53bd052 /* 0x00124020: mrs x18, tpidr_el0 */
    .inst 0x140001e1 /* 0x00124024: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124028: udf #0 */
    .inst 0x00000000 /* 0x0012402c: udf #0 */
    .inst 0x00000000 /* 0x00124030: udf #0 */
    .inst 0x00000000 /* 0x00124034: udf #0 */
    .inst 0x00000000 /* 0x00124038: udf #0 */
    .inst 0x00000000 /* 0x0012403c: udf #0 */
    .inst 0x00000000 /* 0x00124040: udf #0 */
    .inst 0x00000000 /* 0x00124044: udf #0 */
    .inst 0x00000000 /* 0x00124048: udf #0 */
    .inst 0x00000000 /* 0x0012404c: udf #0 */
    .inst 0x00000000 /* 0x00124050: udf #0 */
    .inst 0x00000000 /* 0x00124054: udf #0 */
    .inst 0x00000000 /* 0x00124058: udf #0 */
    .inst 0x00000000 /* 0x0012405c: udf #0 */
    .inst 0x00000000 /* 0x00124060: udf #0 */
    .inst 0x00000000 /* 0x00124064: udf #0 */
    .inst 0x00000000 /* 0x00124068: udf #0 */
    .inst 0x00000000 /* 0x0012406c: udf #0 */
    .inst 0x00000000 /* 0x00124070: udf #0 */
    .inst 0x00000000 /* 0x00124074: udf #0 */
    .inst 0x00000000 /* 0x00124078: udf #0 */
    .inst 0x00000000 /* 0x0012407c: udf #0 */
    .inst 0xd4041003 /* 0x00124080: smc #0x2080 */
    .inst 0xd51bd052 /* 0x00124084: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124088: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012408c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124090: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124094: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124098: mov x0, x18 */
    .inst 0xd2801001 /* 0x0012409c: mov x1, #0x80 */
    .inst 0xd53bd052 /* 0x001240a0: mrs x18, tpidr_el0 */
    .inst 0x140001c1 /* 0x001240a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001240a8: udf #0 */
    .inst 0x00000000 /* 0x001240ac: udf #0 */
    .inst 0x00000000 /* 0x001240b0: udf #0 */
    .inst 0x00000000 /* 0x001240b4: udf #0 */
    .inst 0x00000000 /* 0x001240b8: udf #0 */
    .inst 0x00000000 /* 0x001240bc: udf #0 */
    .inst 0x00000000 /* 0x001240c0: udf #0 */
    .inst 0x00000000 /* 0x001240c4: udf #0 */
    .inst 0x00000000 /* 0x001240c8: udf #0 */
    .inst 0x00000000 /* 0x001240cc: udf #0 */
    .inst 0x00000000 /* 0x001240d0: udf #0 */
    .inst 0x00000000 /* 0x001240d4: udf #0 */
    .inst 0x00000000 /* 0x001240d8: udf #0 */
    .inst 0x00000000 /* 0x001240dc: udf #0 */
    .inst 0x00000000 /* 0x001240e0: udf #0 */
    .inst 0x00000000 /* 0x001240e4: udf #0 */
    .inst 0x00000000 /* 0x001240e8: udf #0 */
    .inst 0x00000000 /* 0x001240ec: udf #0 */
    .inst 0x00000000 /* 0x001240f0: udf #0 */
    .inst 0x00000000 /* 0x001240f4: udf #0 */
    .inst 0x00000000 /* 0x001240f8: udf #0 */
    .inst 0x00000000 /* 0x001240fc: udf #0 */
    .inst 0xd4042003 /* 0x00124100: smc #0x2100 */
    .inst 0xd51bd052 /* 0x00124104: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124108: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012410c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124110: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124114: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124118: mov x0, x18 */
    .inst 0xd2802001 /* 0x0012411c: mov x1, #0x100 */
    .inst 0xd53bd052 /* 0x00124120: mrs x18, tpidr_el0 */
    .inst 0x140001a1 /* 0x00124124: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124128: udf #0 */
    .inst 0x00000000 /* 0x0012412c: udf #0 */
    .inst 0x00000000 /* 0x00124130: udf #0 */
    .inst 0x00000000 /* 0x00124134: udf #0 */
    .inst 0x00000000 /* 0x00124138: udf #0 */
    .inst 0x00000000 /* 0x0012413c: udf #0 */
    .inst 0x00000000 /* 0x00124140: udf #0 */
    .inst 0x00000000 /* 0x00124144: udf #0 */
    .inst 0x00000000 /* 0x00124148: udf #0 */
    .inst 0x00000000 /* 0x0012414c: udf #0 */
    .inst 0x00000000 /* 0x00124150: udf #0 */
    .inst 0x00000000 /* 0x00124154: udf #0 */
    .inst 0x00000000 /* 0x00124158: udf #0 */
    .inst 0x00000000 /* 0x0012415c: udf #0 */
    .inst 0x00000000 /* 0x00124160: udf #0 */
    .inst 0x00000000 /* 0x00124164: udf #0 */
    .inst 0x00000000 /* 0x00124168: udf #0 */
    .inst 0x00000000 /* 0x0012416c: udf #0 */
    .inst 0x00000000 /* 0x00124170: udf #0 */
    .inst 0x00000000 /* 0x00124174: udf #0 */
    .inst 0x00000000 /* 0x00124178: udf #0 */
    .inst 0x00000000 /* 0x0012417c: udf #0 */
    .inst 0xd4043003 /* 0x00124180: smc #0x2180 */
    .inst 0xd51bd052 /* 0x00124184: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124188: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012418c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124190: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124194: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124198: mov x0, x18 */
    .inst 0xd2803001 /* 0x0012419c: mov x1, #0x180 */
    .inst 0xd53bd052 /* 0x001241a0: mrs x18, tpidr_el0 */
    .inst 0x14000181 /* 0x001241a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001241a8: udf #0 */
    .inst 0x00000000 /* 0x001241ac: udf #0 */
    .inst 0x00000000 /* 0x001241b0: udf #0 */
    .inst 0x00000000 /* 0x001241b4: udf #0 */
    .inst 0x00000000 /* 0x001241b8: udf #0 */
    .inst 0x00000000 /* 0x001241bc: udf #0 */
    .inst 0x00000000 /* 0x001241c0: udf #0 */
    .inst 0x00000000 /* 0x001241c4: udf #0 */
    .inst 0x00000000 /* 0x001241c8: udf #0 */
    .inst 0x00000000 /* 0x001241cc: udf #0 */
    .inst 0x00000000 /* 0x001241d0: udf #0 */
    .inst 0x00000000 /* 0x001241d4: udf #0 */
    .inst 0x00000000 /* 0x001241d8: udf #0 */
    .inst 0x00000000 /* 0x001241dc: udf #0 */
    .inst 0x00000000 /* 0x001241e0: udf #0 */
    .inst 0x00000000 /* 0x001241e4: udf #0 */
    .inst 0x00000000 /* 0x001241e8: udf #0 */
    .inst 0x00000000 /* 0x001241ec: udf #0 */
    .inst 0x00000000 /* 0x001241f0: udf #0 */
    .inst 0x00000000 /* 0x001241f4: udf #0 */
    .inst 0x00000000 /* 0x001241f8: udf #0 */
    .inst 0x00000000 /* 0x001241fc: udf #0 */
    .inst 0xd4044003 /* 0x00124200: smc #0x2200 */
    .inst 0xd51bd052 /* 0x00124204: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124208: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012420c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124210: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124214: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124218: mov x0, x18 */
    .inst 0xd2804001 /* 0x0012421c: mov x1, #0x200 */
    .inst 0xd53bd052 /* 0x00124220: mrs x18, tpidr_el0 */
    .inst 0x14000161 /* 0x00124224: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124228: udf #0 */
    .inst 0x00000000 /* 0x0012422c: udf #0 */
    .inst 0x00000000 /* 0x00124230: udf #0 */
    .inst 0x00000000 /* 0x00124234: udf #0 */
    .inst 0x00000000 /* 0x00124238: udf #0 */
    .inst 0x00000000 /* 0x0012423c: udf #0 */
    .inst 0x00000000 /* 0x00124240: udf #0 */
    .inst 0x00000000 /* 0x00124244: udf #0 */
    .inst 0x00000000 /* 0x00124248: udf #0 */
    .inst 0x00000000 /* 0x0012424c: udf #0 */
    .inst 0x00000000 /* 0x00124250: udf #0 */
    .inst 0x00000000 /* 0x00124254: udf #0 */
    .inst 0x00000000 /* 0x00124258: udf #0 */
    .inst 0x00000000 /* 0x0012425c: udf #0 */
    .inst 0x00000000 /* 0x00124260: udf #0 */
    .inst 0x00000000 /* 0x00124264: udf #0 */
    .inst 0x00000000 /* 0x00124268: udf #0 */
    .inst 0x00000000 /* 0x0012426c: udf #0 */
    .inst 0x00000000 /* 0x00124270: udf #0 */
    .inst 0x00000000 /* 0x00124274: udf #0 */
    .inst 0x00000000 /* 0x00124278: udf #0 */
    .inst 0x00000000 /* 0x0012427c: udf #0 */
    .inst 0xd4045003 /* 0x00124280: smc #0x2280 */
    .inst 0xd51bd052 /* 0x00124284: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124288: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012428c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124290: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124294: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124298: mov x0, x18 */
    .inst 0xd2805001 /* 0x0012429c: mov x1, #0x280 */
    .inst 0xd53bd052 /* 0x001242a0: mrs x18, tpidr_el0 */
    .inst 0x14000141 /* 0x001242a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001242a8: udf #0 */
    .inst 0x00000000 /* 0x001242ac: udf #0 */
    .inst 0x00000000 /* 0x001242b0: udf #0 */
    .inst 0x00000000 /* 0x001242b4: udf #0 */
    .inst 0x00000000 /* 0x001242b8: udf #0 */
    .inst 0x00000000 /* 0x001242bc: udf #0 */
    .inst 0x00000000 /* 0x001242c0: udf #0 */
    .inst 0x00000000 /* 0x001242c4: udf #0 */
    .inst 0x00000000 /* 0x001242c8: udf #0 */
    .inst 0x00000000 /* 0x001242cc: udf #0 */
    .inst 0x00000000 /* 0x001242d0: udf #0 */
    .inst 0x00000000 /* 0x001242d4: udf #0 */
    .inst 0x00000000 /* 0x001242d8: udf #0 */
    .inst 0x00000000 /* 0x001242dc: udf #0 */
    .inst 0x00000000 /* 0x001242e0: udf #0 */
    .inst 0x00000000 /* 0x001242e4: udf #0 */
    .inst 0x00000000 /* 0x001242e8: udf #0 */
    .inst 0x00000000 /* 0x001242ec: udf #0 */
    .inst 0x00000000 /* 0x001242f0: udf #0 */
    .inst 0x00000000 /* 0x001242f4: udf #0 */
    .inst 0x00000000 /* 0x001242f8: udf #0 */
    .inst 0x00000000 /* 0x001242fc: udf #0 */
    .inst 0xd4046003 /* 0x00124300: smc #0x2300 */
    .inst 0xd51bd052 /* 0x00124304: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124308: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012430c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124310: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124314: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124318: mov x0, x18 */
    .inst 0xd2806001 /* 0x0012431c: mov x1, #0x300 */
    .inst 0xd53bd052 /* 0x00124320: mrs x18, tpidr_el0 */
    .inst 0x14000121 /* 0x00124324: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124328: udf #0 */
    .inst 0x00000000 /* 0x0012432c: udf #0 */
    .inst 0x00000000 /* 0x00124330: udf #0 */
    .inst 0x00000000 /* 0x00124334: udf #0 */
    .inst 0x00000000 /* 0x00124338: udf #0 */
    .inst 0x00000000 /* 0x0012433c: udf #0 */
    .inst 0x00000000 /* 0x00124340: udf #0 */
    .inst 0x00000000 /* 0x00124344: udf #0 */
    .inst 0x00000000 /* 0x00124348: udf #0 */
    .inst 0x00000000 /* 0x0012434c: udf #0 */
    .inst 0x00000000 /* 0x00124350: udf #0 */
    .inst 0x00000000 /* 0x00124354: udf #0 */
    .inst 0x00000000 /* 0x00124358: udf #0 */
    .inst 0x00000000 /* 0x0012435c: udf #0 */
    .inst 0x00000000 /* 0x00124360: udf #0 */
    .inst 0x00000000 /* 0x00124364: udf #0 */
    .inst 0x00000000 /* 0x00124368: udf #0 */
    .inst 0x00000000 /* 0x0012436c: udf #0 */
    .inst 0x00000000 /* 0x00124370: udf #0 */
    .inst 0x00000000 /* 0x00124374: udf #0 */
    .inst 0x00000000 /* 0x00124378: udf #0 */
    .inst 0x00000000 /* 0x0012437c: udf #0 */
    .inst 0xd4047003 /* 0x00124380: smc #0x2380 */
    .inst 0xd51bd052 /* 0x00124384: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124388: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012438c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124390: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124394: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124398: mov x0, x18 */
    .inst 0xd2807001 /* 0x0012439c: mov x1, #0x380 */
    .inst 0xd53bd052 /* 0x001243a0: mrs x18, tpidr_el0 */
    .inst 0x14000101 /* 0x001243a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001243a8: udf #0 */
    .inst 0x00000000 /* 0x001243ac: udf #0 */
    .inst 0x00000000 /* 0x001243b0: udf #0 */
    .inst 0x00000000 /* 0x001243b4: udf #0 */
    .inst 0x00000000 /* 0x001243b8: udf #0 */
    .inst 0x00000000 /* 0x001243bc: udf #0 */
    .inst 0x00000000 /* 0x001243c0: udf #0 */
    .inst 0x00000000 /* 0x001243c4: udf #0 */
    .inst 0x00000000 /* 0x001243c8: udf #0 */
    .inst 0x00000000 /* 0x001243cc: udf #0 */
    .inst 0x00000000 /* 0x001243d0: udf #0 */
    .inst 0x00000000 /* 0x001243d4: udf #0 */
    .inst 0x00000000 /* 0x001243d8: udf #0 */
    .inst 0x00000000 /* 0x001243dc: udf #0 */
    .inst 0x00000000 /* 0x001243e0: udf #0 */
    .inst 0x00000000 /* 0x001243e4: udf #0 */
    .inst 0x00000000 /* 0x001243e8: udf #0 */
    .inst 0x00000000 /* 0x001243ec: udf #0 */
    .inst 0x00000000 /* 0x001243f0: udf #0 */
    .inst 0x00000000 /* 0x001243f4: udf #0 */
    .inst 0x00000000 /* 0x001243f8: udf #0 */
    .inst 0x00000000 /* 0x001243fc: udf #0 */
    .inst 0xd4048003 /* 0x00124400: smc #0x2400 */
    .inst 0xd51bd052 /* 0x00124404: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124408: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012440c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124410: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124414: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124418: mov x0, x18 */
    .inst 0xd2808001 /* 0x0012441c: mov x1, #0x400 */
    .inst 0xd53bd052 /* 0x00124420: mrs x18, tpidr_el0 */
    .inst 0x140000e1 /* 0x00124424: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124428: udf #0 */
    .inst 0x00000000 /* 0x0012442c: udf #0 */
    .inst 0x00000000 /* 0x00124430: udf #0 */
    .inst 0x00000000 /* 0x00124434: udf #0 */
    .inst 0x00000000 /* 0x00124438: udf #0 */
    .inst 0x00000000 /* 0x0012443c: udf #0 */
    .inst 0x00000000 /* 0x00124440: udf #0 */
    .inst 0x00000000 /* 0x00124444: udf #0 */
    .inst 0x00000000 /* 0x00124448: udf #0 */
    .inst 0x00000000 /* 0x0012444c: udf #0 */
    .inst 0x00000000 /* 0x00124450: udf #0 */
    .inst 0x00000000 /* 0x00124454: udf #0 */
    .inst 0x00000000 /* 0x00124458: udf #0 */
    .inst 0x00000000 /* 0x0012445c: udf #0 */
    .inst 0x00000000 /* 0x00124460: udf #0 */
    .inst 0x00000000 /* 0x00124464: udf #0 */
    .inst 0x00000000 /* 0x00124468: udf #0 */
    .inst 0x00000000 /* 0x0012446c: udf #0 */
    .inst 0x00000000 /* 0x00124470: udf #0 */
    .inst 0x00000000 /* 0x00124474: udf #0 */
    .inst 0x00000000 /* 0x00124478: udf #0 */
    .inst 0x00000000 /* 0x0012447c: udf #0 */
    .inst 0xd4049003 /* 0x00124480: smc #0x2480 */
    .inst 0xd51bd052 /* 0x00124484: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124488: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012448c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124490: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124494: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124498: mov x0, x18 */
    .inst 0xd2809001 /* 0x0012449c: mov x1, #0x480 */
    .inst 0xd53bd052 /* 0x001244a0: mrs x18, tpidr_el0 */
    .inst 0x140000c1 /* 0x001244a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001244a8: udf #0 */
    .inst 0x00000000 /* 0x001244ac: udf #0 */
    .inst 0x00000000 /* 0x001244b0: udf #0 */
    .inst 0x00000000 /* 0x001244b4: udf #0 */
    .inst 0x00000000 /* 0x001244b8: udf #0 */
    .inst 0x00000000 /* 0x001244bc: udf #0 */
    .inst 0x00000000 /* 0x001244c0: udf #0 */
    .inst 0x00000000 /* 0x001244c4: udf #0 */
    .inst 0x00000000 /* 0x001244c8: udf #0 */
    .inst 0x00000000 /* 0x001244cc: udf #0 */
    .inst 0x00000000 /* 0x001244d0: udf #0 */
    .inst 0x00000000 /* 0x001244d4: udf #0 */
    .inst 0x00000000 /* 0x001244d8: udf #0 */
    .inst 0x00000000 /* 0x001244dc: udf #0 */
    .inst 0x00000000 /* 0x001244e0: udf #0 */
    .inst 0x00000000 /* 0x001244e4: udf #0 */
    .inst 0x00000000 /* 0x001244e8: udf #0 */
    .inst 0x00000000 /* 0x001244ec: udf #0 */
    .inst 0x00000000 /* 0x001244f0: udf #0 */
    .inst 0x00000000 /* 0x001244f4: udf #0 */
    .inst 0x00000000 /* 0x001244f8: udf #0 */
    .inst 0x00000000 /* 0x001244fc: udf #0 */
    .inst 0xd404a003 /* 0x00124500: smc #0x2500 */
    .inst 0xd51bd052 /* 0x00124504: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124508: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012450c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124510: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124514: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124518: mov x0, x18 */
    .inst 0xd280a001 /* 0x0012451c: mov x1, #0x500 */
    .inst 0xd53bd052 /* 0x00124520: mrs x18, tpidr_el0 */
    .inst 0x140000a1 /* 0x00124524: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124528: udf #0 */
    .inst 0x00000000 /* 0x0012452c: udf #0 */
    .inst 0x00000000 /* 0x00124530: udf #0 */
    .inst 0x00000000 /* 0x00124534: udf #0 */
    .inst 0x00000000 /* 0x00124538: udf #0 */
    .inst 0x00000000 /* 0x0012453c: udf #0 */
    .inst 0x00000000 /* 0x00124540: udf #0 */
    .inst 0x00000000 /* 0x00124544: udf #0 */
    .inst 0x00000000 /* 0x00124548: udf #0 */
    .inst 0x00000000 /* 0x0012454c: udf #0 */
    .inst 0x00000000 /* 0x00124550: udf #0 */
    .inst 0x00000000 /* 0x00124554: udf #0 */
    .inst 0x00000000 /* 0x00124558: udf #0 */
    .inst 0x00000000 /* 0x0012455c: udf #0 */
    .inst 0x00000000 /* 0x00124560: udf #0 */
    .inst 0x00000000 /* 0x00124564: udf #0 */
    .inst 0x00000000 /* 0x00124568: udf #0 */
    .inst 0x00000000 /* 0x0012456c: udf #0 */
    .inst 0x00000000 /* 0x00124570: udf #0 */
    .inst 0x00000000 /* 0x00124574: udf #0 */
    .inst 0x00000000 /* 0x00124578: udf #0 */
    .inst 0x00000000 /* 0x0012457c: udf #0 */
    .inst 0xd404b003 /* 0x00124580: smc #0x2580 */
    .inst 0xd51bd052 /* 0x00124584: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124588: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012458c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124590: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124594: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124598: mov x0, x18 */
    .inst 0xd280b001 /* 0x0012459c: mov x1, #0x580 */
    .inst 0xd53bd052 /* 0x001245a0: mrs x18, tpidr_el0 */
    .inst 0x14000081 /* 0x001245a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001245a8: udf #0 */
    .inst 0x00000000 /* 0x001245ac: udf #0 */
    .inst 0x00000000 /* 0x001245b0: udf #0 */
    .inst 0x00000000 /* 0x001245b4: udf #0 */
    .inst 0x00000000 /* 0x001245b8: udf #0 */
    .inst 0x00000000 /* 0x001245bc: udf #0 */
    .inst 0x00000000 /* 0x001245c0: udf #0 */
    .inst 0x00000000 /* 0x001245c4: udf #0 */
    .inst 0x00000000 /* 0x001245c8: udf #0 */
    .inst 0x00000000 /* 0x001245cc: udf #0 */
    .inst 0x00000000 /* 0x001245d0: udf #0 */
    .inst 0x00000000 /* 0x001245d4: udf #0 */
    .inst 0x00000000 /* 0x001245d8: udf #0 */
    .inst 0x00000000 /* 0x001245dc: udf #0 */
    .inst 0x00000000 /* 0x001245e0: udf #0 */
    .inst 0x00000000 /* 0x001245e4: udf #0 */
    .inst 0x00000000 /* 0x001245e8: udf #0 */
    .inst 0x00000000 /* 0x001245ec: udf #0 */
    .inst 0x00000000 /* 0x001245f0: udf #0 */
    .inst 0x00000000 /* 0x001245f4: udf #0 */
    .inst 0x00000000 /* 0x001245f8: udf #0 */
    .inst 0x00000000 /* 0x001245fc: udf #0 */
    .inst 0xd404c003 /* 0x00124600: smc #0x2600 */
    .inst 0xd51bd052 /* 0x00124604: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124608: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012460c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124610: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124614: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124618: mov x0, x18 */
    .inst 0xd280c001 /* 0x0012461c: mov x1, #0x600 */
    .inst 0xd53bd052 /* 0x00124620: mrs x18, tpidr_el0 */
    .inst 0x14000061 /* 0x00124624: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124628: udf #0 */
    .inst 0x00000000 /* 0x0012462c: udf #0 */
    .inst 0x00000000 /* 0x00124630: udf #0 */
    .inst 0x00000000 /* 0x00124634: udf #0 */
    .inst 0x00000000 /* 0x00124638: udf #0 */
    .inst 0x00000000 /* 0x0012463c: udf #0 */
    .inst 0x00000000 /* 0x00124640: udf #0 */
    .inst 0x00000000 /* 0x00124644: udf #0 */
    .inst 0x00000000 /* 0x00124648: udf #0 */
    .inst 0x00000000 /* 0x0012464c: udf #0 */
    .inst 0x00000000 /* 0x00124650: udf #0 */
    .inst 0x00000000 /* 0x00124654: udf #0 */
    .inst 0x00000000 /* 0x00124658: udf #0 */
    .inst 0x00000000 /* 0x0012465c: udf #0 */
    .inst 0x00000000 /* 0x00124660: udf #0 */
    .inst 0x00000000 /* 0x00124664: udf #0 */
    .inst 0x00000000 /* 0x00124668: udf #0 */
    .inst 0x00000000 /* 0x0012466c: udf #0 */
    .inst 0x00000000 /* 0x00124670: udf #0 */
    .inst 0x00000000 /* 0x00124674: udf #0 */
    .inst 0x00000000 /* 0x00124678: udf #0 */
    .inst 0x00000000 /* 0x0012467c: udf #0 */
    .inst 0xd404d003 /* 0x00124680: smc #0x2680 */
    .inst 0xd51bd052 /* 0x00124684: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124688: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012468c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124690: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124694: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124698: mov x0, x18 */
    .inst 0xd280d001 /* 0x0012469c: mov x1, #0x680 */
    .inst 0xd53bd052 /* 0x001246a0: mrs x18, tpidr_el0 */
    .inst 0x14000041 /* 0x001246a4: b #0x1247a8 */
    .inst 0x00000000 /* 0x001246a8: udf #0 */
    .inst 0x00000000 /* 0x001246ac: udf #0 */
    .inst 0x00000000 /* 0x001246b0: udf #0 */
    .inst 0x00000000 /* 0x001246b4: udf #0 */
    .inst 0x00000000 /* 0x001246b8: udf #0 */
    .inst 0x00000000 /* 0x001246bc: udf #0 */
    .inst 0x00000000 /* 0x001246c0: udf #0 */
    .inst 0x00000000 /* 0x001246c4: udf #0 */
    .inst 0x00000000 /* 0x001246c8: udf #0 */
    .inst 0x00000000 /* 0x001246cc: udf #0 */
    .inst 0x00000000 /* 0x001246d0: udf #0 */
    .inst 0x00000000 /* 0x001246d4: udf #0 */
    .inst 0x00000000 /* 0x001246d8: udf #0 */
    .inst 0x00000000 /* 0x001246dc: udf #0 */
    .inst 0x00000000 /* 0x001246e0: udf #0 */
    .inst 0x00000000 /* 0x001246e4: udf #0 */
    .inst 0x00000000 /* 0x001246e8: udf #0 */
    .inst 0x00000000 /* 0x001246ec: udf #0 */
    .inst 0x00000000 /* 0x001246f0: udf #0 */
    .inst 0x00000000 /* 0x001246f4: udf #0 */
    .inst 0x00000000 /* 0x001246f8: udf #0 */
    .inst 0x00000000 /* 0x001246fc: udf #0 */
    .inst 0xd404e003 /* 0x00124700: smc #0x2700 */
    .inst 0xd51bd052 /* 0x00124704: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124708: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012470c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124710: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124714: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124718: mov x0, x18 */
    .inst 0xd280e001 /* 0x0012471c: mov x1, #0x700 */
    .inst 0xd53bd052 /* 0x00124720: mrs x18, tpidr_el0 */
    .inst 0x14000021 /* 0x00124724: b #0x1247a8 */
    .inst 0x00000000 /* 0x00124728: udf #0 */
    .inst 0x00000000 /* 0x0012472c: udf #0 */
    .inst 0x00000000 /* 0x00124730: udf #0 */
    .inst 0x00000000 /* 0x00124734: udf #0 */
    .inst 0x00000000 /* 0x00124738: udf #0 */
    .inst 0x00000000 /* 0x0012473c: udf #0 */
    .inst 0x00000000 /* 0x00124740: udf #0 */
    .inst 0x00000000 /* 0x00124744: udf #0 */
    .inst 0x00000000 /* 0x00124748: udf #0 */
    .inst 0x00000000 /* 0x0012474c: udf #0 */
    .inst 0x00000000 /* 0x00124750: udf #0 */
    .inst 0x00000000 /* 0x00124754: udf #0 */
    .inst 0x00000000 /* 0x00124758: udf #0 */
    .inst 0x00000000 /* 0x0012475c: udf #0 */
    .inst 0x00000000 /* 0x00124760: udf #0 */
    .inst 0x00000000 /* 0x00124764: udf #0 */
    .inst 0x00000000 /* 0x00124768: udf #0 */
    .inst 0x00000000 /* 0x0012476c: udf #0 */
    .inst 0x00000000 /* 0x00124770: udf #0 */
    .inst 0x00000000 /* 0x00124774: udf #0 */
    .inst 0x00000000 /* 0x00124778: udf #0 */
    .inst 0x00000000 /* 0x0012477c: udf #0 */
    .inst 0xd404f003 /* 0x00124780: smc #0x2780 */
    .inst 0xd51bd052 /* 0x00124784: msr tpidr_el0, x18 */
    .inst 0xd53cd052 /* 0x00124788: mrs x18, tpidr_el2 */
    .inst 0xf9400252 /* 0x0012478c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00124790: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00124794: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00124798: mov x0, x18 */
    .inst 0xd280f001 /* 0x0012479c: mov x1, #0x780 */
    .inst 0xd53bd052 /* 0x001247a0: mrs x18, tpidr_el0 */
    .inst 0x14000001 /* 0x001247a4: b #0x1247a8 */
_el2_vector:
    .inst 0xd4000043 /* 0x001247a8: smc #2 */
    .inst 0xf9000401 /* 0x001247ac: str x1, [x0, #8] */
    .inst 0xd5384241 /* 0x001247b0: mrs x1, currentel */
    .inst 0xf9008c01 /* 0x001247b4: str x1, [x0, #0x118] */
    .inst 0xf900c001 /* 0x001247b8: str x1, [x0, #0x180] */
    .inst 0xd5384241 /* 0x001247bc: mrs x1, currentel */
    .inst 0xf900c401 /* 0x001247c0: str x1, [x0, #0x188] */
    .inst 0xf9400801 /* 0x001247c4: ldr x1, [x0, #0x10] */
    .inst 0xf9010401 /* 0x001247c8: str x1, [x0, #0x208] */
    .inst 0xf9400c01 /* 0x001247cc: ldr x1, [x0, #0x18] */
    .inst 0xf9010801 /* 0x001247d0: str x1, [x0, #0x210] */
    .inst 0xd2804301 /* 0x001247d4: mov x1, #0x218 */
    .inst 0xf9010c01 /* 0x001247d8: str x1, [x0, #0x218] */
    .inst 0xd53c4001 /* 0x001247dc: mrs x1, spsr_el2 */
    .inst 0xf900c801 /* 0x001247e0: str x1, [x0, #0x190] */
    .inst 0xd53c5201 /* 0x001247e4: mrs x1, esr_el2 */
    .inst 0xf900cc01 /* 0x001247e8: str x1, [x0, #0x198] */
    .inst 0xd53c4021 /* 0x001247ec: mrs x1, elr_el2 */
    .inst 0xf9010001 /* 0x001247f0: str x1, [x0, #0x200] */
    .inst 0xd4000043 /* 0x001247f4: smc #2 */
