/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/el1/el1_vector.S
 * Section virtual-address base: 0x00125000
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_el1_el1_vector.S, "ax", %progbits
.p2align 2
.globl __loader_el1_begin
.globl _loader_el1_vector

/* original range [0x00125000, 0x00125848) */
.org 0x0
EL1_VBAR:
__loader_el1_begin:
_loader_el1_vector:
    .inst 0xd4000003 /* 0x00125000: smc #0 */
    .inst 0xd51bd052 /* 0x00125004: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125008: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012500c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125010: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125014: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125018: mov x0, x18 */
    .inst 0xd2800001 /* 0x0012501c: mov x1, #0 */
    .inst 0xd53bd052 /* 0x00125020: mrs x18, tpidr_el0 */
    .inst 0x140001f7 /* 0x00125024: b #0x125800 */
    .inst 0x00000000 /* 0x00125028: udf #0 */
    .inst 0x00000000 /* 0x0012502c: udf #0 */
    .inst 0x00000000 /* 0x00125030: udf #0 */
    .inst 0x00000000 /* 0x00125034: udf #0 */
    .inst 0x00000000 /* 0x00125038: udf #0 */
    .inst 0x00000000 /* 0x0012503c: udf #0 */
    .inst 0x00000000 /* 0x00125040: udf #0 */
    .inst 0x00000000 /* 0x00125044: udf #0 */
    .inst 0x00000000 /* 0x00125048: udf #0 */
    .inst 0x00000000 /* 0x0012504c: udf #0 */
    .inst 0x00000000 /* 0x00125050: udf #0 */
    .inst 0x00000000 /* 0x00125054: udf #0 */
    .inst 0x00000000 /* 0x00125058: udf #0 */
    .inst 0x00000000 /* 0x0012505c: udf #0 */
    .inst 0x00000000 /* 0x00125060: udf #0 */
    .inst 0x00000000 /* 0x00125064: udf #0 */
    .inst 0x00000000 /* 0x00125068: udf #0 */
    .inst 0x00000000 /* 0x0012506c: udf #0 */
    .inst 0x00000000 /* 0x00125070: udf #0 */
    .inst 0x00000000 /* 0x00125074: udf #0 */
    .inst 0x00000000 /* 0x00125078: udf #0 */
    .inst 0x00000000 /* 0x0012507c: udf #0 */
    .inst 0xd4001003 /* 0x00125080: smc #0x80 */
    .inst 0xd51bd052 /* 0x00125084: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125088: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012508c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125090: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125094: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125098: mov x0, x18 */
    .inst 0xd2801001 /* 0x0012509c: mov x1, #0x80 */
    .inst 0xd53bd052 /* 0x001250a0: mrs x18, tpidr_el0 */
    .inst 0x140001d7 /* 0x001250a4: b #0x125800 */
    .inst 0x00000000 /* 0x001250a8: udf #0 */
    .inst 0x00000000 /* 0x001250ac: udf #0 */
    .inst 0x00000000 /* 0x001250b0: udf #0 */
    .inst 0x00000000 /* 0x001250b4: udf #0 */
    .inst 0x00000000 /* 0x001250b8: udf #0 */
    .inst 0x00000000 /* 0x001250bc: udf #0 */
    .inst 0x00000000 /* 0x001250c0: udf #0 */
    .inst 0x00000000 /* 0x001250c4: udf #0 */
    .inst 0x00000000 /* 0x001250c8: udf #0 */
    .inst 0x00000000 /* 0x001250cc: udf #0 */
    .inst 0x00000000 /* 0x001250d0: udf #0 */
    .inst 0x00000000 /* 0x001250d4: udf #0 */
    .inst 0x00000000 /* 0x001250d8: udf #0 */
    .inst 0x00000000 /* 0x001250dc: udf #0 */
    .inst 0x00000000 /* 0x001250e0: udf #0 */
    .inst 0x00000000 /* 0x001250e4: udf #0 */
    .inst 0x00000000 /* 0x001250e8: udf #0 */
    .inst 0x00000000 /* 0x001250ec: udf #0 */
    .inst 0x00000000 /* 0x001250f0: udf #0 */
    .inst 0x00000000 /* 0x001250f4: udf #0 */
    .inst 0x00000000 /* 0x001250f8: udf #0 */
    .inst 0x00000000 /* 0x001250fc: udf #0 */
    .inst 0xd4002003 /* 0x00125100: smc #0x100 */
    .inst 0xd51bd052 /* 0x00125104: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125108: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012510c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125110: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125114: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125118: mov x0, x18 */
    .inst 0xd2802001 /* 0x0012511c: mov x1, #0x100 */
    .inst 0xd53bd052 /* 0x00125120: mrs x18, tpidr_el0 */
    .inst 0x140001b7 /* 0x00125124: b #0x125800 */
    .inst 0x00000000 /* 0x00125128: udf #0 */
    .inst 0x00000000 /* 0x0012512c: udf #0 */
    .inst 0x00000000 /* 0x00125130: udf #0 */
    .inst 0x00000000 /* 0x00125134: udf #0 */
    .inst 0x00000000 /* 0x00125138: udf #0 */
    .inst 0x00000000 /* 0x0012513c: udf #0 */
    .inst 0x00000000 /* 0x00125140: udf #0 */
    .inst 0x00000000 /* 0x00125144: udf #0 */
    .inst 0x00000000 /* 0x00125148: udf #0 */
    .inst 0x00000000 /* 0x0012514c: udf #0 */
    .inst 0x00000000 /* 0x00125150: udf #0 */
    .inst 0x00000000 /* 0x00125154: udf #0 */
    .inst 0x00000000 /* 0x00125158: udf #0 */
    .inst 0x00000000 /* 0x0012515c: udf #0 */
    .inst 0x00000000 /* 0x00125160: udf #0 */
    .inst 0x00000000 /* 0x00125164: udf #0 */
    .inst 0x00000000 /* 0x00125168: udf #0 */
    .inst 0x00000000 /* 0x0012516c: udf #0 */
    .inst 0x00000000 /* 0x00125170: udf #0 */
    .inst 0x00000000 /* 0x00125174: udf #0 */
    .inst 0x00000000 /* 0x00125178: udf #0 */
    .inst 0x00000000 /* 0x0012517c: udf #0 */
    .inst 0xd4003003 /* 0x00125180: smc #0x180 */
    .inst 0xd51bd052 /* 0x00125184: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125188: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012518c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125190: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125194: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125198: mov x0, x18 */
    .inst 0xd2803001 /* 0x0012519c: mov x1, #0x180 */
    .inst 0xd53bd052 /* 0x001251a0: mrs x18, tpidr_el0 */
    .inst 0x14000197 /* 0x001251a4: b #0x125800 */
    .inst 0x00000000 /* 0x001251a8: udf #0 */
    .inst 0x00000000 /* 0x001251ac: udf #0 */
    .inst 0x00000000 /* 0x001251b0: udf #0 */
    .inst 0x00000000 /* 0x001251b4: udf #0 */
    .inst 0x00000000 /* 0x001251b8: udf #0 */
    .inst 0x00000000 /* 0x001251bc: udf #0 */
    .inst 0x00000000 /* 0x001251c0: udf #0 */
    .inst 0x00000000 /* 0x001251c4: udf #0 */
    .inst 0x00000000 /* 0x001251c8: udf #0 */
    .inst 0x00000000 /* 0x001251cc: udf #0 */
    .inst 0x00000000 /* 0x001251d0: udf #0 */
    .inst 0x00000000 /* 0x001251d4: udf #0 */
    .inst 0x00000000 /* 0x001251d8: udf #0 */
    .inst 0x00000000 /* 0x001251dc: udf #0 */
    .inst 0x00000000 /* 0x001251e0: udf #0 */
    .inst 0x00000000 /* 0x001251e4: udf #0 */
    .inst 0x00000000 /* 0x001251e8: udf #0 */
    .inst 0x00000000 /* 0x001251ec: udf #0 */
    .inst 0x00000000 /* 0x001251f0: udf #0 */
    .inst 0x00000000 /* 0x001251f4: udf #0 */
    .inst 0x00000000 /* 0x001251f8: udf #0 */
    .inst 0x00000000 /* 0x001251fc: udf #0 */
    .inst 0xd4004003 /* 0x00125200: smc #0x200 */
    .inst 0xd51bd052 /* 0x00125204: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125208: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012520c: ldr x18, [x18] */
    .inst 0xd4000023 /* 0x00125210: smc #1 */
    .inst 0xf9000a40 /* 0x00125214: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125218: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x0012521c: mov x0, x18 */
    .inst 0xd2804001 /* 0x00125220: mov x1, #0x200 */
    .inst 0xd53bd052 /* 0x00125224: mrs x18, tpidr_el0 */
    .inst 0x14000176 /* 0x00125228: b #0x125800 */
    .inst 0x00000000 /* 0x0012522c: udf #0 */
    .inst 0x00000000 /* 0x00125230: udf #0 */
    .inst 0x00000000 /* 0x00125234: udf #0 */
    .inst 0x00000000 /* 0x00125238: udf #0 */
    .inst 0x00000000 /* 0x0012523c: udf #0 */
    .inst 0x00000000 /* 0x00125240: udf #0 */
    .inst 0x00000000 /* 0x00125244: udf #0 */
    .inst 0x00000000 /* 0x00125248: udf #0 */
    .inst 0x00000000 /* 0x0012524c: udf #0 */
    .inst 0x00000000 /* 0x00125250: udf #0 */
    .inst 0x00000000 /* 0x00125254: udf #0 */
    .inst 0x00000000 /* 0x00125258: udf #0 */
    .inst 0x00000000 /* 0x0012525c: udf #0 */
    .inst 0x00000000 /* 0x00125260: udf #0 */
    .inst 0x00000000 /* 0x00125264: udf #0 */
    .inst 0x00000000 /* 0x00125268: udf #0 */
    .inst 0x00000000 /* 0x0012526c: udf #0 */
    .inst 0x00000000 /* 0x00125270: udf #0 */
    .inst 0x00000000 /* 0x00125274: udf #0 */
    .inst 0x00000000 /* 0x00125278: udf #0 */
    .inst 0x00000000 /* 0x0012527c: udf #0 */
    .inst 0xd4005003 /* 0x00125280: smc #0x280 */
    .inst 0xd51bd052 /* 0x00125284: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125288: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012528c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125290: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125294: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125298: mov x0, x18 */
    .inst 0xd2805001 /* 0x0012529c: mov x1, #0x280 */
    .inst 0xd53bd052 /* 0x001252a0: mrs x18, tpidr_el0 */
    .inst 0x14000157 /* 0x001252a4: b #0x125800 */
    .inst 0x00000000 /* 0x001252a8: udf #0 */
    .inst 0x00000000 /* 0x001252ac: udf #0 */
    .inst 0x00000000 /* 0x001252b0: udf #0 */
    .inst 0x00000000 /* 0x001252b4: udf #0 */
    .inst 0x00000000 /* 0x001252b8: udf #0 */
    .inst 0x00000000 /* 0x001252bc: udf #0 */
    .inst 0x00000000 /* 0x001252c0: udf #0 */
    .inst 0x00000000 /* 0x001252c4: udf #0 */
    .inst 0x00000000 /* 0x001252c8: udf #0 */
    .inst 0x00000000 /* 0x001252cc: udf #0 */
    .inst 0x00000000 /* 0x001252d0: udf #0 */
    .inst 0x00000000 /* 0x001252d4: udf #0 */
    .inst 0x00000000 /* 0x001252d8: udf #0 */
    .inst 0x00000000 /* 0x001252dc: udf #0 */
    .inst 0x00000000 /* 0x001252e0: udf #0 */
    .inst 0x00000000 /* 0x001252e4: udf #0 */
    .inst 0x00000000 /* 0x001252e8: udf #0 */
    .inst 0x00000000 /* 0x001252ec: udf #0 */
    .inst 0x00000000 /* 0x001252f0: udf #0 */
    .inst 0x00000000 /* 0x001252f4: udf #0 */
    .inst 0x00000000 /* 0x001252f8: udf #0 */
    .inst 0x00000000 /* 0x001252fc: udf #0 */
    .inst 0xd4006003 /* 0x00125300: smc #0x300 */
    .inst 0xd51bd052 /* 0x00125304: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125308: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012530c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125310: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125314: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125318: mov x0, x18 */
    .inst 0xd2806001 /* 0x0012531c: mov x1, #0x300 */
    .inst 0xd53bd052 /* 0x00125320: mrs x18, tpidr_el0 */
    .inst 0x14000137 /* 0x00125324: b #0x125800 */
    .inst 0x00000000 /* 0x00125328: udf #0 */
    .inst 0x00000000 /* 0x0012532c: udf #0 */
    .inst 0x00000000 /* 0x00125330: udf #0 */
    .inst 0x00000000 /* 0x00125334: udf #0 */
    .inst 0x00000000 /* 0x00125338: udf #0 */
    .inst 0x00000000 /* 0x0012533c: udf #0 */
    .inst 0x00000000 /* 0x00125340: udf #0 */
    .inst 0x00000000 /* 0x00125344: udf #0 */
    .inst 0x00000000 /* 0x00125348: udf #0 */
    .inst 0x00000000 /* 0x0012534c: udf #0 */
    .inst 0x00000000 /* 0x00125350: udf #0 */
    .inst 0x00000000 /* 0x00125354: udf #0 */
    .inst 0x00000000 /* 0x00125358: udf #0 */
    .inst 0x00000000 /* 0x0012535c: udf #0 */
    .inst 0x00000000 /* 0x00125360: udf #0 */
    .inst 0x00000000 /* 0x00125364: udf #0 */
    .inst 0x00000000 /* 0x00125368: udf #0 */
    .inst 0x00000000 /* 0x0012536c: udf #0 */
    .inst 0x00000000 /* 0x00125370: udf #0 */
    .inst 0x00000000 /* 0x00125374: udf #0 */
    .inst 0x00000000 /* 0x00125378: udf #0 */
    .inst 0x00000000 /* 0x0012537c: udf #0 */
    .inst 0xd4007003 /* 0x00125380: smc #0x380 */
    .inst 0xd51bd052 /* 0x00125384: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125388: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012538c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125390: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125394: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125398: mov x0, x18 */
    .inst 0xd2807001 /* 0x0012539c: mov x1, #0x380 */
    .inst 0xd53bd052 /* 0x001253a0: mrs x18, tpidr_el0 */
    .inst 0x14000117 /* 0x001253a4: b #0x125800 */
    .inst 0x00000000 /* 0x001253a8: udf #0 */
    .inst 0x00000000 /* 0x001253ac: udf #0 */
    .inst 0x00000000 /* 0x001253b0: udf #0 */
    .inst 0x00000000 /* 0x001253b4: udf #0 */
    .inst 0x00000000 /* 0x001253b8: udf #0 */
    .inst 0x00000000 /* 0x001253bc: udf #0 */
    .inst 0x00000000 /* 0x001253c0: udf #0 */
    .inst 0x00000000 /* 0x001253c4: udf #0 */
    .inst 0x00000000 /* 0x001253c8: udf #0 */
    .inst 0x00000000 /* 0x001253cc: udf #0 */
    .inst 0x00000000 /* 0x001253d0: udf #0 */
    .inst 0x00000000 /* 0x001253d4: udf #0 */
    .inst 0x00000000 /* 0x001253d8: udf #0 */
    .inst 0x00000000 /* 0x001253dc: udf #0 */
    .inst 0x00000000 /* 0x001253e0: udf #0 */
    .inst 0x00000000 /* 0x001253e4: udf #0 */
    .inst 0x00000000 /* 0x001253e8: udf #0 */
    .inst 0x00000000 /* 0x001253ec: udf #0 */
    .inst 0x00000000 /* 0x001253f0: udf #0 */
    .inst 0x00000000 /* 0x001253f4: udf #0 */
    .inst 0x00000000 /* 0x001253f8: udf #0 */
    .inst 0x00000000 /* 0x001253fc: udf #0 */
    .inst 0xd4008003 /* 0x00125400: smc #0x400 */
    .inst 0xd51bd052 /* 0x00125404: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125408: mrs x18, tpidr_el1 */
    .inst 0xaa1203f4 /* 0x0012540c: mov x20, x18 */
    .inst 0xf9400252 /* 0x00125410: ldr x18, [x18] */
    .inst 0xaa1203f5 /* 0x00125414: mov x21, x18 */
    .inst 0xaa1203f6 /* 0x00125418: mov x22, x18 */
    .inst 0xf9000a40 /* 0x0012541c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125420: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125424: mov x0, x18 */
    .inst 0xd2808001 /* 0x00125428: mov x1, #0x400 */
    .inst 0xd53bd052 /* 0x0012542c: mrs x18, tpidr_el0 */
    .inst 0x140000f4 /* 0x00125430: b #0x125800 */
    .inst 0x00000000 /* 0x00125434: udf #0 */
    .inst 0x00000000 /* 0x00125438: udf #0 */
    .inst 0x00000000 /* 0x0012543c: udf #0 */
    .inst 0x00000000 /* 0x00125440: udf #0 */
    .inst 0x00000000 /* 0x00125444: udf #0 */
    .inst 0x00000000 /* 0x00125448: udf #0 */
    .inst 0x00000000 /* 0x0012544c: udf #0 */
    .inst 0x00000000 /* 0x00125450: udf #0 */
    .inst 0x00000000 /* 0x00125454: udf #0 */
    .inst 0x00000000 /* 0x00125458: udf #0 */
    .inst 0x00000000 /* 0x0012545c: udf #0 */
    .inst 0x00000000 /* 0x00125460: udf #0 */
    .inst 0x00000000 /* 0x00125464: udf #0 */
    .inst 0x00000000 /* 0x00125468: udf #0 */
    .inst 0x00000000 /* 0x0012546c: udf #0 */
    .inst 0x00000000 /* 0x00125470: udf #0 */
    .inst 0x00000000 /* 0x00125474: udf #0 */
    .inst 0x00000000 /* 0x00125478: udf #0 */
    .inst 0x00000000 /* 0x0012547c: udf #0 */
    .inst 0xd4009003 /* 0x00125480: smc #0x480 */
    .inst 0xd51bd052 /* 0x00125484: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125488: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012548c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125490: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125494: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125498: mov x0, x18 */
    .inst 0xd2809001 /* 0x0012549c: mov x1, #0x480 */
    .inst 0xd53bd052 /* 0x001254a0: mrs x18, tpidr_el0 */
    .inst 0x140000d7 /* 0x001254a4: b #0x125800 */
    .inst 0x00000000 /* 0x001254a8: udf #0 */
    .inst 0x00000000 /* 0x001254ac: udf #0 */
    .inst 0x00000000 /* 0x001254b0: udf #0 */
    .inst 0x00000000 /* 0x001254b4: udf #0 */
    .inst 0x00000000 /* 0x001254b8: udf #0 */
    .inst 0x00000000 /* 0x001254bc: udf #0 */
    .inst 0x00000000 /* 0x001254c0: udf #0 */
    .inst 0x00000000 /* 0x001254c4: udf #0 */
    .inst 0x00000000 /* 0x001254c8: udf #0 */
    .inst 0x00000000 /* 0x001254cc: udf #0 */
    .inst 0x00000000 /* 0x001254d0: udf #0 */
    .inst 0x00000000 /* 0x001254d4: udf #0 */
    .inst 0x00000000 /* 0x001254d8: udf #0 */
    .inst 0x00000000 /* 0x001254dc: udf #0 */
    .inst 0x00000000 /* 0x001254e0: udf #0 */
    .inst 0x00000000 /* 0x001254e4: udf #0 */
    .inst 0x00000000 /* 0x001254e8: udf #0 */
    .inst 0x00000000 /* 0x001254ec: udf #0 */
    .inst 0x00000000 /* 0x001254f0: udf #0 */
    .inst 0x00000000 /* 0x001254f4: udf #0 */
    .inst 0x00000000 /* 0x001254f8: udf #0 */
    .inst 0x00000000 /* 0x001254fc: udf #0 */
    .inst 0xd400a003 /* 0x00125500: smc #0x500 */
    .inst 0xd51bd052 /* 0x00125504: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125508: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012550c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125510: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125514: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125518: mov x0, x18 */
    .inst 0xd280a001 /* 0x0012551c: mov x1, #0x500 */
    .inst 0xd53bd052 /* 0x00125520: mrs x18, tpidr_el0 */
    .inst 0x140000b7 /* 0x00125524: b #0x125800 */
    .inst 0x00000000 /* 0x00125528: udf #0 */
    .inst 0x00000000 /* 0x0012552c: udf #0 */
    .inst 0x00000000 /* 0x00125530: udf #0 */
    .inst 0x00000000 /* 0x00125534: udf #0 */
    .inst 0x00000000 /* 0x00125538: udf #0 */
    .inst 0x00000000 /* 0x0012553c: udf #0 */
    .inst 0x00000000 /* 0x00125540: udf #0 */
    .inst 0x00000000 /* 0x00125544: udf #0 */
    .inst 0x00000000 /* 0x00125548: udf #0 */
    .inst 0x00000000 /* 0x0012554c: udf #0 */
    .inst 0x00000000 /* 0x00125550: udf #0 */
    .inst 0x00000000 /* 0x00125554: udf #0 */
    .inst 0x00000000 /* 0x00125558: udf #0 */
    .inst 0x00000000 /* 0x0012555c: udf #0 */
    .inst 0x00000000 /* 0x00125560: udf #0 */
    .inst 0x00000000 /* 0x00125564: udf #0 */
    .inst 0x00000000 /* 0x00125568: udf #0 */
    .inst 0x00000000 /* 0x0012556c: udf #0 */
    .inst 0x00000000 /* 0x00125570: udf #0 */
    .inst 0x00000000 /* 0x00125574: udf #0 */
    .inst 0x00000000 /* 0x00125578: udf #0 */
    .inst 0x00000000 /* 0x0012557c: udf #0 */
    .inst 0xd400b003 /* 0x00125580: smc #0x580 */
    .inst 0xd51bd052 /* 0x00125584: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125588: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012558c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125590: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125594: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125598: mov x0, x18 */
    .inst 0xd280b001 /* 0x0012559c: mov x1, #0x580 */
    .inst 0xd53bd052 /* 0x001255a0: mrs x18, tpidr_el0 */
    .inst 0x14000097 /* 0x001255a4: b #0x125800 */
    .inst 0x00000000 /* 0x001255a8: udf #0 */
    .inst 0x00000000 /* 0x001255ac: udf #0 */
    .inst 0x00000000 /* 0x001255b0: udf #0 */
    .inst 0x00000000 /* 0x001255b4: udf #0 */
    .inst 0x00000000 /* 0x001255b8: udf #0 */
    .inst 0x00000000 /* 0x001255bc: udf #0 */
    .inst 0x00000000 /* 0x001255c0: udf #0 */
    .inst 0x00000000 /* 0x001255c4: udf #0 */
    .inst 0x00000000 /* 0x001255c8: udf #0 */
    .inst 0x00000000 /* 0x001255cc: udf #0 */
    .inst 0x00000000 /* 0x001255d0: udf #0 */
    .inst 0x00000000 /* 0x001255d4: udf #0 */
    .inst 0x00000000 /* 0x001255d8: udf #0 */
    .inst 0x00000000 /* 0x001255dc: udf #0 */
    .inst 0x00000000 /* 0x001255e0: udf #0 */
    .inst 0x00000000 /* 0x001255e4: udf #0 */
    .inst 0x00000000 /* 0x001255e8: udf #0 */
    .inst 0x00000000 /* 0x001255ec: udf #0 */
    .inst 0x00000000 /* 0x001255f0: udf #0 */
    .inst 0x00000000 /* 0x001255f4: udf #0 */
    .inst 0x00000000 /* 0x001255f8: udf #0 */
    .inst 0x00000000 /* 0x001255fc: udf #0 */
    .inst 0xd400c003 /* 0x00125600: smc #0x600 */
    .inst 0xd51bd052 /* 0x00125604: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125608: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012560c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125610: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125614: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125618: mov x0, x18 */
    .inst 0xd280c001 /* 0x0012561c: mov x1, #0x600 */
    .inst 0xd53bd052 /* 0x00125620: mrs x18, tpidr_el0 */
    .inst 0x14000077 /* 0x00125624: b #0x125800 */
    .inst 0x00000000 /* 0x00125628: udf #0 */
    .inst 0x00000000 /* 0x0012562c: udf #0 */
    .inst 0x00000000 /* 0x00125630: udf #0 */
    .inst 0x00000000 /* 0x00125634: udf #0 */
    .inst 0x00000000 /* 0x00125638: udf #0 */
    .inst 0x00000000 /* 0x0012563c: udf #0 */
    .inst 0x00000000 /* 0x00125640: udf #0 */
    .inst 0x00000000 /* 0x00125644: udf #0 */
    .inst 0x00000000 /* 0x00125648: udf #0 */
    .inst 0x00000000 /* 0x0012564c: udf #0 */
    .inst 0x00000000 /* 0x00125650: udf #0 */
    .inst 0x00000000 /* 0x00125654: udf #0 */
    .inst 0x00000000 /* 0x00125658: udf #0 */
    .inst 0x00000000 /* 0x0012565c: udf #0 */
    .inst 0x00000000 /* 0x00125660: udf #0 */
    .inst 0x00000000 /* 0x00125664: udf #0 */
    .inst 0x00000000 /* 0x00125668: udf #0 */
    .inst 0x00000000 /* 0x0012566c: udf #0 */
    .inst 0x00000000 /* 0x00125670: udf #0 */
    .inst 0x00000000 /* 0x00125674: udf #0 */
    .inst 0x00000000 /* 0x00125678: udf #0 */
    .inst 0x00000000 /* 0x0012567c: udf #0 */
    .inst 0xd400d003 /* 0x00125680: smc #0x680 */
    .inst 0xd51bd052 /* 0x00125684: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125688: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012568c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125690: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125694: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125698: mov x0, x18 */
    .inst 0xd280d001 /* 0x0012569c: mov x1, #0x680 */
    .inst 0xd53bd052 /* 0x001256a0: mrs x18, tpidr_el0 */
    .inst 0x14000057 /* 0x001256a4: b #0x125800 */
    .inst 0x00000000 /* 0x001256a8: udf #0 */
    .inst 0x00000000 /* 0x001256ac: udf #0 */
    .inst 0x00000000 /* 0x001256b0: udf #0 */
    .inst 0x00000000 /* 0x001256b4: udf #0 */
    .inst 0x00000000 /* 0x001256b8: udf #0 */
    .inst 0x00000000 /* 0x001256bc: udf #0 */
    .inst 0x00000000 /* 0x001256c0: udf #0 */
    .inst 0x00000000 /* 0x001256c4: udf #0 */
    .inst 0x00000000 /* 0x001256c8: udf #0 */
    .inst 0x00000000 /* 0x001256cc: udf #0 */
    .inst 0x00000000 /* 0x001256d0: udf #0 */
    .inst 0x00000000 /* 0x001256d4: udf #0 */
    .inst 0x00000000 /* 0x001256d8: udf #0 */
    .inst 0x00000000 /* 0x001256dc: udf #0 */
    .inst 0x00000000 /* 0x001256e0: udf #0 */
    .inst 0x00000000 /* 0x001256e4: udf #0 */
    .inst 0x00000000 /* 0x001256e8: udf #0 */
    .inst 0x00000000 /* 0x001256ec: udf #0 */
    .inst 0x00000000 /* 0x001256f0: udf #0 */
    .inst 0x00000000 /* 0x001256f4: udf #0 */
    .inst 0x00000000 /* 0x001256f8: udf #0 */
    .inst 0x00000000 /* 0x001256fc: udf #0 */
    .inst 0xd400e003 /* 0x00125700: smc #0x700 */
    .inst 0xd51bd052 /* 0x00125704: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125708: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012570c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125710: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125714: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125718: mov x0, x18 */
    .inst 0xd280e001 /* 0x0012571c: mov x1, #0x700 */
    .inst 0xd53bd052 /* 0x00125720: mrs x18, tpidr_el0 */
    .inst 0x14000037 /* 0x00125724: b #0x125800 */
    .inst 0x00000000 /* 0x00125728: udf #0 */
    .inst 0x00000000 /* 0x0012572c: udf #0 */
    .inst 0x00000000 /* 0x00125730: udf #0 */
    .inst 0x00000000 /* 0x00125734: udf #0 */
    .inst 0x00000000 /* 0x00125738: udf #0 */
    .inst 0x00000000 /* 0x0012573c: udf #0 */
    .inst 0x00000000 /* 0x00125740: udf #0 */
    .inst 0x00000000 /* 0x00125744: udf #0 */
    .inst 0x00000000 /* 0x00125748: udf #0 */
    .inst 0x00000000 /* 0x0012574c: udf #0 */
    .inst 0x00000000 /* 0x00125750: udf #0 */
    .inst 0x00000000 /* 0x00125754: udf #0 */
    .inst 0x00000000 /* 0x00125758: udf #0 */
    .inst 0x00000000 /* 0x0012575c: udf #0 */
    .inst 0x00000000 /* 0x00125760: udf #0 */
    .inst 0x00000000 /* 0x00125764: udf #0 */
    .inst 0x00000000 /* 0x00125768: udf #0 */
    .inst 0x00000000 /* 0x0012576c: udf #0 */
    .inst 0x00000000 /* 0x00125770: udf #0 */
    .inst 0x00000000 /* 0x00125774: udf #0 */
    .inst 0x00000000 /* 0x00125778: udf #0 */
    .inst 0x00000000 /* 0x0012577c: udf #0 */
    .inst 0xd400f003 /* 0x00125780: smc #0x780 */
    .inst 0xd51bd052 /* 0x00125784: msr tpidr_el0, x18 */
    .inst 0xd538d092 /* 0x00125788: mrs x18, tpidr_el1 */
    .inst 0xf9400252 /* 0x0012578c: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x00125790: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00125794: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00125798: mov x0, x18 */
    .inst 0xd280f001 /* 0x0012579c: mov x1, #0x780 */
    .inst 0xd53bd052 /* 0x001257a0: mrs x18, tpidr_el0 */
    .inst 0x14000017 /* 0x001257a4: b #0x125800 */
    .inst 0x00000000 /* 0x001257a8: udf #0 */
    .inst 0x00000000 /* 0x001257ac: udf #0 */
    .inst 0x00000000 /* 0x001257b0: udf #0 */
    .inst 0x00000000 /* 0x001257b4: udf #0 */
    .inst 0x00000000 /* 0x001257b8: udf #0 */
    .inst 0x00000000 /* 0x001257bc: udf #0 */
    .inst 0x00000000 /* 0x001257c0: udf #0 */
    .inst 0x00000000 /* 0x001257c4: udf #0 */
    .inst 0x00000000 /* 0x001257c8: udf #0 */
    .inst 0x00000000 /* 0x001257cc: udf #0 */
    .inst 0x00000000 /* 0x001257d0: udf #0 */
    .inst 0x00000000 /* 0x001257d4: udf #0 */
    .inst 0x00000000 /* 0x001257d8: udf #0 */
    .inst 0x00000000 /* 0x001257dc: udf #0 */
    .inst 0x00000000 /* 0x001257e0: udf #0 */
    .inst 0x00000000 /* 0x001257e4: udf #0 */
    .inst 0x00000000 /* 0x001257e8: udf #0 */
    .inst 0x00000000 /* 0x001257ec: udf #0 */
    .inst 0x00000000 /* 0x001257f0: udf #0 */
    .inst 0x00000000 /* 0x001257f4: udf #0 */
    .inst 0x00000000 /* 0x001257f8: udf #0 */
    .inst 0x00000000 /* 0x001257fc: udf #0 */
_el1_serror:
    .inst 0xd4000023 /* 0x00125800: smc #1 */
    .inst 0xf9000401 /* 0x00125804: str x1, [x0, #8] */
    .inst 0xf900c001 /* 0x00125808: str x1, [x0, #0x180] */
    .inst 0xd5384241 /* 0x0012580c: mrs x1, currentel */
    .inst 0xf900c401 /* 0x00125810: str x1, [x0, #0x188] */
    .inst 0xf9400801 /* 0x00125814: ldr x1, [x0, #0x10] */
    .inst 0xf9010401 /* 0x00125818: str x1, [x0, #0x208] */
    .inst 0xf9400c01 /* 0x0012581c: ldr x1, [x0, #0x18] */
    .inst 0xf9010801 /* 0x00125820: str x1, [x0, #0x210] */
    .inst 0xd2804301 /* 0x00125824: mov x1, #0x218 */
    .inst 0xf9010c01 /* 0x00125828: str x1, [x0, #0x218] */
    .inst 0xd5384001 /* 0x0012582c: mrs x1, spsr_el1 */
    .inst 0xf900c801 /* 0x00125830: str x1, [x0, #0x190] */
    .inst 0xd5385201 /* 0x00125834: mrs x1, esr_el1 */
    .inst 0xf900cc01 /* 0x00125838: str x1, [x0, #0x198] */
    .inst 0xd5384021 /* 0x0012583c: mrs x1, elr_el1 */
    .inst 0xf9010001 /* 0x00125840: str x1, [x0, #0x200] */
    .inst 0xd4000023 /* 0x00125844: smc #1 */
