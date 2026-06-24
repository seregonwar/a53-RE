/* A53-RE lossless reference section; bytes are authoritative. */
/* original section .el1.loader, VMA 0x00125000, 3368 bytes */
.arch armv8-a
.section .el1.loader, "ax", %progbits
.p2align 3
.globl __loader_el1_begin
.globl _loader_el1_vector
.globl g_debug_status_el1_core0
.globl g_debug_status_el1_core1
.globl g_dev_context_el1_core0
.globl g_dev_context_el1_core1
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
g_debug_status_el1_core0:
    .inst 0x1aa5daef /* 0x00125848: <undecoded> */
    .inst 0xcbb3d18a /* 0x0012584c: <undecoded> */
    .inst 0x00000000 /* 0x00125850: udf #0 */
    .inst 0x00000000 /* 0x00125854: udf #0 */
    .inst 0x00000000 /* 0x00125858: udf #0 */
    .inst 0x00000000 /* 0x0012585c: udf #0 */
    .inst 0x00000000 /* 0x00125860: udf #0 */
    .inst 0x00000000 /* 0x00125864: udf #0 */
    .inst 0x00000000 /* 0x00125868: udf #0 */
    .inst 0x00000000 /* 0x0012586c: udf #0 */
    .inst 0x00000000 /* 0x00125870: udf #0 */
    .inst 0x00000000 /* 0x00125874: udf #0 */
    .inst 0x00000000 /* 0x00125878: udf #0 */
    .inst 0x00000000 /* 0x0012587c: udf #0 */
    .inst 0x00000000 /* 0x00125880: udf #0 */
    .inst 0x00000000 /* 0x00125884: udf #0 */
    .inst 0x00000000 /* 0x00125888: udf #0 */
    .inst 0x00000000 /* 0x0012588c: udf #0 */
    .inst 0x00000000 /* 0x00125890: udf #0 */
    .inst 0x00000000 /* 0x00125894: udf #0 */
    .inst 0x00000000 /* 0x00125898: udf #0 */
    .inst 0x00000000 /* 0x0012589c: udf #0 */
    .inst 0x00000000 /* 0x001258a0: udf #0 */
    .inst 0x00000000 /* 0x001258a4: udf #0 */
    .inst 0x00000000 /* 0x001258a8: udf #0 */
    .inst 0x00000000 /* 0x001258ac: udf #0 */
    .inst 0x00000000 /* 0x001258b0: udf #0 */
    .inst 0x00000000 /* 0x001258b4: udf #0 */
    .inst 0x00000000 /* 0x001258b8: udf #0 */
    .inst 0x00000000 /* 0x001258bc: udf #0 */
    .inst 0x00000000 /* 0x001258c0: udf #0 */
    .inst 0x00000000 /* 0x001258c4: udf #0 */
    .inst 0x00000000 /* 0x001258c8: udf #0 */
    .inst 0x00000000 /* 0x001258cc: udf #0 */
    .inst 0x00000000 /* 0x001258d0: udf #0 */
    .inst 0x00000000 /* 0x001258d4: udf #0 */
    .inst 0x00000000 /* 0x001258d8: udf #0 */
    .inst 0x00000000 /* 0x001258dc: udf #0 */
    .inst 0x00000000 /* 0x001258e0: udf #0 */
    .inst 0x00000000 /* 0x001258e4: udf #0 */
    .inst 0x00000000 /* 0x001258e8: udf #0 */
    .inst 0x00000000 /* 0x001258ec: udf #0 */
    .inst 0x00000000 /* 0x001258f0: udf #0 */
    .inst 0x00000000 /* 0x001258f4: udf #0 */
    .inst 0x00000000 /* 0x001258f8: udf #0 */
    .inst 0x00000000 /* 0x001258fc: udf #0 */
    .inst 0x00000000 /* 0x00125900: udf #0 */
    .inst 0x00000000 /* 0x00125904: udf #0 */
    .inst 0x00000000 /* 0x00125908: udf #0 */
    .inst 0x00000000 /* 0x0012590c: udf #0 */
    .inst 0x00000000 /* 0x00125910: udf #0 */
    .inst 0x00000000 /* 0x00125914: udf #0 */
    .inst 0x00000000 /* 0x00125918: udf #0 */
    .inst 0x00000000 /* 0x0012591c: udf #0 */
    .inst 0x00000000 /* 0x00125920: udf #0 */
    .inst 0x00000000 /* 0x00125924: udf #0 */
    .inst 0x00000000 /* 0x00125928: udf #0 */
    .inst 0x00000000 /* 0x0012592c: udf #0 */
    .inst 0x00000000 /* 0x00125930: udf #0 */
    .inst 0x00000000 /* 0x00125934: udf #0 */
    .inst 0x00000000 /* 0x00125938: udf #0 */
    .inst 0x00000000 /* 0x0012593c: udf #0 */
    .inst 0x00000000 /* 0x00125940: udf #0 */
    .inst 0x00000000 /* 0x00125944: udf #0 */
    .inst 0x00000000 /* 0x00125948: udf #0 */
    .inst 0x00000000 /* 0x0012594c: udf #0 */
    .inst 0x00000000 /* 0x00125950: udf #0 */
    .inst 0x00000000 /* 0x00125954: udf #0 */
    .inst 0x00000000 /* 0x00125958: udf #0 */
    .inst 0x00000000 /* 0x0012595c: udf #0 */
    .inst 0x00000000 /* 0x00125960: udf #0 */
    .inst 0x00000000 /* 0x00125964: udf #0 */
    .inst 0x00000000 /* 0x00125968: udf #0 */
    .inst 0x00000000 /* 0x0012596c: udf #0 */
    .inst 0x00000000 /* 0x00125970: udf #0 */
    .inst 0x00000000 /* 0x00125974: udf #0 */
    .inst 0x00000000 /* 0x00125978: udf #0 */
    .inst 0x00000000 /* 0x0012597c: udf #0 */
    .inst 0x00000000 /* 0x00125980: udf #0 */
    .inst 0x00000000 /* 0x00125984: udf #0 */
    .inst 0x00000000 /* 0x00125988: udf #0 */
    .inst 0x00000000 /* 0x0012598c: udf #0 */
    .inst 0x00000000 /* 0x00125990: udf #0 */
    .inst 0x00000000 /* 0x00125994: udf #0 */
    .inst 0x00000000 /* 0x00125998: udf #0 */
    .inst 0x00000000 /* 0x0012599c: udf #0 */
    .inst 0x00000000 /* 0x001259a0: udf #0 */
    .inst 0x00000000 /* 0x001259a4: udf #0 */
    .inst 0x00000000 /* 0x001259a8: udf #0 */
    .inst 0x00000000 /* 0x001259ac: udf #0 */
    .inst 0x00000000 /* 0x001259b0: udf #0 */
    .inst 0x00000000 /* 0x001259b4: udf #0 */
    .inst 0x00000000 /* 0x001259b8: udf #0 */
    .inst 0x00000000 /* 0x001259bc: udf #0 */
    .inst 0x00000000 /* 0x001259c0: udf #0 */
    .inst 0x00000000 /* 0x001259c4: udf #0 */
    .inst 0x00000000 /* 0x001259c8: udf #0 */
    .inst 0x00000000 /* 0x001259cc: udf #0 */
    .inst 0x00000000 /* 0x001259d0: udf #0 */
    .inst 0x00000000 /* 0x001259d4: udf #0 */
    .inst 0x00000000 /* 0x001259d8: udf #0 */
    .inst 0x00000000 /* 0x001259dc: udf #0 */
    .inst 0x00000000 /* 0x001259e0: udf #0 */
    .inst 0x00000000 /* 0x001259e4: udf #0 */
    .inst 0x00000000 /* 0x001259e8: udf #0 */
    .inst 0x00000000 /* 0x001259ec: udf #0 */
    .inst 0x00000000 /* 0x001259f0: udf #0 */
    .inst 0x00000000 /* 0x001259f4: udf #0 */
    .inst 0x00000000 /* 0x001259f8: udf #0 */
    .inst 0x00000000 /* 0x001259fc: udf #0 */
    .inst 0x00000000 /* 0x00125a00: udf #0 */
    .inst 0x00000000 /* 0x00125a04: udf #0 */
    .inst 0x00000000 /* 0x00125a08: udf #0 */
    .inst 0x00000000 /* 0x00125a0c: udf #0 */
    .inst 0x00000000 /* 0x00125a10: udf #0 */
    .inst 0x00000000 /* 0x00125a14: udf #0 */
    .inst 0x00000000 /* 0x00125a18: udf #0 */
    .inst 0x00000000 /* 0x00125a1c: udf #0 */
    .inst 0x00000000 /* 0x00125a20: udf #0 */
    .inst 0x00000000 /* 0x00125a24: udf #0 */
    .inst 0x00000000 /* 0x00125a28: udf #0 */
    .inst 0x00000000 /* 0x00125a2c: udf #0 */
    .inst 0x00000000 /* 0x00125a30: udf #0 */
    .inst 0x00000000 /* 0x00125a34: udf #0 */
    .inst 0x00000000 /* 0x00125a38: udf #0 */
    .inst 0x00000000 /* 0x00125a3c: udf #0 */
    .inst 0x00000000 /* 0x00125a40: udf #0 */
    .inst 0x00000000 /* 0x00125a44: udf #0 */
    .inst 0x00000000 /* 0x00125a48: udf #0 */
    .inst 0x00000000 /* 0x00125a4c: udf #0 */
    .inst 0x00000000 /* 0x00125a50: udf #0 */
    .inst 0x00000000 /* 0x00125a54: udf #0 */
    .inst 0x00000000 /* 0x00125a58: udf #0 */
    .inst 0x00000000 /* 0x00125a5c: udf #0 */
    .inst 0x00000000 /* 0x00125a60: udf #0 */
    .inst 0x00000000 /* 0x00125a64: udf #0 */
    .inst 0x00000000 /* 0x00125a68: udf #0 */
    .inst 0x00000000 /* 0x00125a6c: udf #0 */
    .inst 0x00000000 /* 0x00125a70: udf #0 */
    .inst 0x00000000 /* 0x00125a74: udf #0 */
    .inst 0x00000000 /* 0x00125a78: udf #0 */
    .inst 0x00000000 /* 0x00125a7c: udf #0 */
g_debug_status_el1_core1:
    .inst 0x1aa5daef /* 0x00125a80: <undecoded> */
    .inst 0xcbb3d18a /* 0x00125a84: <undecoded> */
    .inst 0x00000000 /* 0x00125a88: udf #0 */
    .inst 0x00000000 /* 0x00125a8c: udf #0 */
    .inst 0x00000000 /* 0x00125a90: udf #0 */
    .inst 0x00000000 /* 0x00125a94: udf #0 */
    .inst 0x00000000 /* 0x00125a98: udf #0 */
    .inst 0x00000000 /* 0x00125a9c: udf #0 */
    .inst 0x00000000 /* 0x00125aa0: udf #0 */
    .inst 0x00000000 /* 0x00125aa4: udf #0 */
    .inst 0x00000000 /* 0x00125aa8: udf #0 */
    .inst 0x00000000 /* 0x00125aac: udf #0 */
    .inst 0x00000000 /* 0x00125ab0: udf #0 */
    .inst 0x00000000 /* 0x00125ab4: udf #0 */
    .inst 0x00000000 /* 0x00125ab8: udf #0 */
    .inst 0x00000000 /* 0x00125abc: udf #0 */
    .inst 0x00000000 /* 0x00125ac0: udf #0 */
    .inst 0x00000000 /* 0x00125ac4: udf #0 */
    .inst 0x00000000 /* 0x00125ac8: udf #0 */
    .inst 0x00000000 /* 0x00125acc: udf #0 */
    .inst 0x00000000 /* 0x00125ad0: udf #0 */
    .inst 0x00000000 /* 0x00125ad4: udf #0 */
    .inst 0x00000000 /* 0x00125ad8: udf #0 */
    .inst 0x00000000 /* 0x00125adc: udf #0 */
    .inst 0x00000000 /* 0x00125ae0: udf #0 */
    .inst 0x00000000 /* 0x00125ae4: udf #0 */
    .inst 0x00000000 /* 0x00125ae8: udf #0 */
    .inst 0x00000000 /* 0x00125aec: udf #0 */
    .inst 0x00000000 /* 0x00125af0: udf #0 */
    .inst 0x00000000 /* 0x00125af4: udf #0 */
    .inst 0x00000000 /* 0x00125af8: udf #0 */
    .inst 0x00000000 /* 0x00125afc: udf #0 */
    .inst 0x00000000 /* 0x00125b00: udf #0 */
    .inst 0x00000000 /* 0x00125b04: udf #0 */
    .inst 0x00000000 /* 0x00125b08: udf #0 */
    .inst 0x00000000 /* 0x00125b0c: udf #0 */
    .inst 0x00000000 /* 0x00125b10: udf #0 */
    .inst 0x00000000 /* 0x00125b14: udf #0 */
    .inst 0x00000000 /* 0x00125b18: udf #0 */
    .inst 0x00000000 /* 0x00125b1c: udf #0 */
    .inst 0x00000000 /* 0x00125b20: udf #0 */
    .inst 0x00000000 /* 0x00125b24: udf #0 */
    .inst 0x00000000 /* 0x00125b28: udf #0 */
    .inst 0x00000000 /* 0x00125b2c: udf #0 */
    .inst 0x00000000 /* 0x00125b30: udf #0 */
    .inst 0x00000000 /* 0x00125b34: udf #0 */
    .inst 0x00000000 /* 0x00125b38: udf #0 */
    .inst 0x00000000 /* 0x00125b3c: udf #0 */
    .inst 0x00000000 /* 0x00125b40: udf #0 */
    .inst 0x00000000 /* 0x00125b44: udf #0 */
    .inst 0x00000000 /* 0x00125b48: udf #0 */
    .inst 0x00000000 /* 0x00125b4c: udf #0 */
    .inst 0x00000000 /* 0x00125b50: udf #0 */
    .inst 0x00000000 /* 0x00125b54: udf #0 */
    .inst 0x00000000 /* 0x00125b58: udf #0 */
    .inst 0x00000000 /* 0x00125b5c: udf #0 */
    .inst 0x00000000 /* 0x00125b60: udf #0 */
    .inst 0x00000000 /* 0x00125b64: udf #0 */
    .inst 0x00000000 /* 0x00125b68: udf #0 */
    .inst 0x00000000 /* 0x00125b6c: udf #0 */
    .inst 0x00000000 /* 0x00125b70: udf #0 */
    .inst 0x00000000 /* 0x00125b74: udf #0 */
    .inst 0x00000000 /* 0x00125b78: udf #0 */
    .inst 0x00000000 /* 0x00125b7c: udf #0 */
    .inst 0x00000000 /* 0x00125b80: udf #0 */
    .inst 0x00000000 /* 0x00125b84: udf #0 */
    .inst 0x00000000 /* 0x00125b88: udf #0 */
    .inst 0x00000000 /* 0x00125b8c: udf #0 */
    .inst 0x00000000 /* 0x00125b90: udf #0 */
    .inst 0x00000000 /* 0x00125b94: udf #0 */
    .inst 0x00000000 /* 0x00125b98: udf #0 */
    .inst 0x00000000 /* 0x00125b9c: udf #0 */
    .inst 0x00000000 /* 0x00125ba0: udf #0 */
    .inst 0x00000000 /* 0x00125ba4: udf #0 */
    .inst 0x00000000 /* 0x00125ba8: udf #0 */
    .inst 0x00000000 /* 0x00125bac: udf #0 */
    .inst 0x00000000 /* 0x00125bb0: udf #0 */
    .inst 0x00000000 /* 0x00125bb4: udf #0 */
    .inst 0x00000000 /* 0x00125bb8: udf #0 */
    .inst 0x00000000 /* 0x00125bbc: udf #0 */
    .inst 0x00000000 /* 0x00125bc0: udf #0 */
    .inst 0x00000000 /* 0x00125bc4: udf #0 */
    .inst 0x00000000 /* 0x00125bc8: udf #0 */
    .inst 0x00000000 /* 0x00125bcc: udf #0 */
    .inst 0x00000000 /* 0x00125bd0: udf #0 */
    .inst 0x00000000 /* 0x00125bd4: udf #0 */
    .inst 0x00000000 /* 0x00125bd8: udf #0 */
    .inst 0x00000000 /* 0x00125bdc: udf #0 */
    .inst 0x00000000 /* 0x00125be0: udf #0 */
    .inst 0x00000000 /* 0x00125be4: udf #0 */
    .inst 0x00000000 /* 0x00125be8: udf #0 */
    .inst 0x00000000 /* 0x00125bec: udf #0 */
    .inst 0x00000000 /* 0x00125bf0: udf #0 */
    .inst 0x00000000 /* 0x00125bf4: udf #0 */
    .inst 0x00000000 /* 0x00125bf8: udf #0 */
    .inst 0x00000000 /* 0x00125bfc: udf #0 */
    .inst 0x00000000 /* 0x00125c00: udf #0 */
    .inst 0x00000000 /* 0x00125c04: udf #0 */
    .inst 0x00000000 /* 0x00125c08: udf #0 */
    .inst 0x00000000 /* 0x00125c0c: udf #0 */
    .inst 0x00000000 /* 0x00125c10: udf #0 */
    .inst 0x00000000 /* 0x00125c14: udf #0 */
    .inst 0x00000000 /* 0x00125c18: udf #0 */
    .inst 0x00000000 /* 0x00125c1c: udf #0 */
    .inst 0x00000000 /* 0x00125c20: udf #0 */
    .inst 0x00000000 /* 0x00125c24: udf #0 */
    .inst 0x00000000 /* 0x00125c28: udf #0 */
    .inst 0x00000000 /* 0x00125c2c: udf #0 */
    .inst 0x00000000 /* 0x00125c30: udf #0 */
    .inst 0x00000000 /* 0x00125c34: udf #0 */
    .inst 0x00000000 /* 0x00125c38: udf #0 */
    .inst 0x00000000 /* 0x00125c3c: udf #0 */
    .inst 0x00000000 /* 0x00125c40: udf #0 */
    .inst 0x00000000 /* 0x00125c44: udf #0 */
    .inst 0x00000000 /* 0x00125c48: udf #0 */
    .inst 0x00000000 /* 0x00125c4c: udf #0 */
    .inst 0x00000000 /* 0x00125c50: udf #0 */
    .inst 0x00000000 /* 0x00125c54: udf #0 */
    .inst 0x00000000 /* 0x00125c58: udf #0 */
    .inst 0x00000000 /* 0x00125c5c: udf #0 */
    .inst 0x00000000 /* 0x00125c60: udf #0 */
    .inst 0x00000000 /* 0x00125c64: udf #0 */
    .inst 0x00000000 /* 0x00125c68: udf #0 */
    .inst 0x00000000 /* 0x00125c6c: udf #0 */
    .inst 0x00000000 /* 0x00125c70: udf #0 */
    .inst 0x00000000 /* 0x00125c74: udf #0 */
    .inst 0x00000000 /* 0x00125c78: udf #0 */
    .inst 0x00000000 /* 0x00125c7c: udf #0 */
    .inst 0x00000000 /* 0x00125c80: udf #0 */
    .inst 0x00000000 /* 0x00125c84: udf #0 */
    .inst 0x00000000 /* 0x00125c88: udf #0 */
    .inst 0x00000000 /* 0x00125c8c: udf #0 */
    .inst 0x00000000 /* 0x00125c90: udf #0 */
    .inst 0x00000000 /* 0x00125c94: udf #0 */
    .inst 0x00000000 /* 0x00125c98: udf #0 */
    .inst 0x00000000 /* 0x00125c9c: udf #0 */
    .inst 0x00000000 /* 0x00125ca0: udf #0 */
    .inst 0x00000000 /* 0x00125ca4: udf #0 */
    .inst 0x00000000 /* 0x00125ca8: udf #0 */
    .inst 0x00000000 /* 0x00125cac: udf #0 */
    .inst 0x00000000 /* 0x00125cb0: udf #0 */
    .inst 0x00000000 /* 0x00125cb4: udf #0 */
g_dev_context_el1_core0:
    .inst 0x00125848 /* 0x00125cb8: <undecoded> */
    .inst 0x00000000 /* 0x00125cbc: udf #0 */
    .inst 0x00000000 /* 0x00125cc0: udf #0 */
    .inst 0x00000000 /* 0x00125cc4: udf #0 */
    .inst 0x00000000 /* 0x00125cc8: udf #0 */
    .inst 0x00000000 /* 0x00125ccc: udf #0 */
    .inst 0x00000000 /* 0x00125cd0: udf #0 */
    .inst 0x00000000 /* 0x00125cd4: udf #0 */
    .inst 0x00000000 /* 0x00125cd8: udf #0 */
    .inst 0x00000000 /* 0x00125cdc: udf #0 */
    .inst 0x00000000 /* 0x00125ce0: udf #0 */
    .inst 0x00000000 /* 0x00125ce4: udf #0 */
    .inst 0x00000000 /* 0x00125ce8: udf #0 */
    .inst 0x00000000 /* 0x00125cec: udf #0 */
g_dev_context_el1_core1:
    .inst 0x00125a80 /* 0x00125cf0: <undecoded> */
    .inst 0x00000000 /* 0x00125cf4: udf #0 */
    .inst 0x00000000 /* 0x00125cf8: udf #0 */
    .inst 0x00000000 /* 0x00125cfc: udf #0 */
    .inst 0x00000000 /* 0x00125d00: udf #0 */
    .inst 0x00000000 /* 0x00125d04: udf #0 */
    .inst 0x00000000 /* 0x00125d08: udf #0 */
    .inst 0x00000000 /* 0x00125d0c: udf #0 */
    .inst 0x00000000 /* 0x00125d10: udf #0 */
    .inst 0x00000000 /* 0x00125d14: udf #0 */
    .inst 0x00000000 /* 0x00125d18: udf #0 */
    .inst 0x00000000 /* 0x00125d1c: udf #0 */
    .inst 0x00000000 /* 0x00125d20: udf #0 */
    .inst 0x00000000 /* 0x00125d24: udf #0 */
