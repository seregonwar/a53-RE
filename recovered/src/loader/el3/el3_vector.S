/*
 * A53-RE instruction-exact assembly recovery artifact.
 * Original compilation unit: src/loader/el3/el3_vector.S
 * Section virtual-address base: 0x00107000
 * `.inst` is authoritative; comments are Capstone review disassembly.
 */
.arch armv8-a
.section .text.a53.loader_el3_el3_vector.S, "ax", %progbits
.p2align 2
.globl __loader_el3_text_begin
.globl __loader_el3_vector
.globl __page_table_el3_level3_end
.globl _loader_el3_vector

/* original range [0x00107000, 0x00107948) */
.org 0x0
EL3_VBAR:
__loader_el3_text_begin:
__loader_el3_vector:
__page_table_el3_level3_end:
_loader_el3_vector:
    .inst 0xd51bd052 /* 0x00107000: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107004: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107008: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010700c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107010: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107014: mov x0, x18 */
    .inst 0xd2800001 /* 0x00107018: mov x1, #0 */
    .inst 0xd53bd052 /* 0x0010701c: mrs x18, tpidr_el0 */
    .inst 0x14000203 /* 0x00107020: b #0x10782c */
    .inst 0x00000000 /* 0x00107024: udf #0 */
    .inst 0x00000000 /* 0x00107028: udf #0 */
    .inst 0x00000000 /* 0x0010702c: udf #0 */
    .inst 0x00000000 /* 0x00107030: udf #0 */
    .inst 0x00000000 /* 0x00107034: udf #0 */
    .inst 0x00000000 /* 0x00107038: udf #0 */
    .inst 0x00000000 /* 0x0010703c: udf #0 */
    .inst 0x00000000 /* 0x00107040: udf #0 */
    .inst 0x00000000 /* 0x00107044: udf #0 */
    .inst 0x00000000 /* 0x00107048: udf #0 */
    .inst 0x00000000 /* 0x0010704c: udf #0 */
    .inst 0x00000000 /* 0x00107050: udf #0 */
    .inst 0x00000000 /* 0x00107054: udf #0 */
    .inst 0x00000000 /* 0x00107058: udf #0 */
    .inst 0x00000000 /* 0x0010705c: udf #0 */
    .inst 0x00000000 /* 0x00107060: udf #0 */
    .inst 0x00000000 /* 0x00107064: udf #0 */
    .inst 0x00000000 /* 0x00107068: udf #0 */
    .inst 0x00000000 /* 0x0010706c: udf #0 */
    .inst 0x00000000 /* 0x00107070: udf #0 */
    .inst 0x00000000 /* 0x00107074: udf #0 */
    .inst 0x00000000 /* 0x00107078: udf #0 */
    .inst 0x00000000 /* 0x0010707c: udf #0 */
    .inst 0xd51bd052 /* 0x00107080: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107084: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107088: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010708c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107090: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107094: mov x0, x18 */
    .inst 0xd2801001 /* 0x00107098: mov x1, #0x80 */
    .inst 0xd53bd052 /* 0x0010709c: mrs x18, tpidr_el0 */
    .inst 0x140001e3 /* 0x001070a0: b #0x10782c */
    .inst 0x00000000 /* 0x001070a4: udf #0 */
    .inst 0x00000000 /* 0x001070a8: udf #0 */
    .inst 0x00000000 /* 0x001070ac: udf #0 */
    .inst 0x00000000 /* 0x001070b0: udf #0 */
    .inst 0x00000000 /* 0x001070b4: udf #0 */
    .inst 0x00000000 /* 0x001070b8: udf #0 */
    .inst 0x00000000 /* 0x001070bc: udf #0 */
    .inst 0x00000000 /* 0x001070c0: udf #0 */
    .inst 0x00000000 /* 0x001070c4: udf #0 */
    .inst 0x00000000 /* 0x001070c8: udf #0 */
    .inst 0x00000000 /* 0x001070cc: udf #0 */
    .inst 0x00000000 /* 0x001070d0: udf #0 */
    .inst 0x00000000 /* 0x001070d4: udf #0 */
    .inst 0x00000000 /* 0x001070d8: udf #0 */
    .inst 0x00000000 /* 0x001070dc: udf #0 */
    .inst 0x00000000 /* 0x001070e0: udf #0 */
    .inst 0x00000000 /* 0x001070e4: udf #0 */
    .inst 0x00000000 /* 0x001070e8: udf #0 */
    .inst 0x00000000 /* 0x001070ec: udf #0 */
    .inst 0x00000000 /* 0x001070f0: udf #0 */
    .inst 0x00000000 /* 0x001070f4: udf #0 */
    .inst 0x00000000 /* 0x001070f8: udf #0 */
    .inst 0x00000000 /* 0x001070fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107100: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107104: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107108: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010710c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107110: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107114: mov x0, x18 */
    .inst 0xd2802001 /* 0x00107118: mov x1, #0x100 */
    .inst 0xd53bd052 /* 0x0010711c: mrs x18, tpidr_el0 */
    .inst 0x140001c3 /* 0x00107120: b #0x10782c */
    .inst 0x00000000 /* 0x00107124: udf #0 */
    .inst 0x00000000 /* 0x00107128: udf #0 */
    .inst 0x00000000 /* 0x0010712c: udf #0 */
    .inst 0x00000000 /* 0x00107130: udf #0 */
    .inst 0x00000000 /* 0x00107134: udf #0 */
    .inst 0x00000000 /* 0x00107138: udf #0 */
    .inst 0x00000000 /* 0x0010713c: udf #0 */
    .inst 0x00000000 /* 0x00107140: udf #0 */
    .inst 0x00000000 /* 0x00107144: udf #0 */
    .inst 0x00000000 /* 0x00107148: udf #0 */
    .inst 0x00000000 /* 0x0010714c: udf #0 */
    .inst 0x00000000 /* 0x00107150: udf #0 */
    .inst 0x00000000 /* 0x00107154: udf #0 */
    .inst 0x00000000 /* 0x00107158: udf #0 */
    .inst 0x00000000 /* 0x0010715c: udf #0 */
    .inst 0x00000000 /* 0x00107160: udf #0 */
    .inst 0x00000000 /* 0x00107164: udf #0 */
    .inst 0x00000000 /* 0x00107168: udf #0 */
    .inst 0x00000000 /* 0x0010716c: udf #0 */
    .inst 0x00000000 /* 0x00107170: udf #0 */
    .inst 0x00000000 /* 0x00107174: udf #0 */
    .inst 0x00000000 /* 0x00107178: udf #0 */
    .inst 0x00000000 /* 0x0010717c: udf #0 */
    .inst 0xd51bd052 /* 0x00107180: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107184: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107188: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010718c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107190: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107194: mov x0, x18 */
    .inst 0xd2803001 /* 0x00107198: mov x1, #0x180 */
    .inst 0xd53bd052 /* 0x0010719c: mrs x18, tpidr_el0 */
    .inst 0x140001a3 /* 0x001071a0: b #0x10782c */
    .inst 0x00000000 /* 0x001071a4: udf #0 */
    .inst 0x00000000 /* 0x001071a8: udf #0 */
    .inst 0x00000000 /* 0x001071ac: udf #0 */
    .inst 0x00000000 /* 0x001071b0: udf #0 */
    .inst 0x00000000 /* 0x001071b4: udf #0 */
    .inst 0x00000000 /* 0x001071b8: udf #0 */
    .inst 0x00000000 /* 0x001071bc: udf #0 */
    .inst 0x00000000 /* 0x001071c0: udf #0 */
    .inst 0x00000000 /* 0x001071c4: udf #0 */
    .inst 0x00000000 /* 0x001071c8: udf #0 */
    .inst 0x00000000 /* 0x001071cc: udf #0 */
    .inst 0x00000000 /* 0x001071d0: udf #0 */
    .inst 0x00000000 /* 0x001071d4: udf #0 */
    .inst 0x00000000 /* 0x001071d8: udf #0 */
    .inst 0x00000000 /* 0x001071dc: udf #0 */
    .inst 0x00000000 /* 0x001071e0: udf #0 */
    .inst 0x00000000 /* 0x001071e4: udf #0 */
    .inst 0x00000000 /* 0x001071e8: udf #0 */
    .inst 0x00000000 /* 0x001071ec: udf #0 */
    .inst 0x00000000 /* 0x001071f0: udf #0 */
    .inst 0x00000000 /* 0x001071f4: udf #0 */
    .inst 0x00000000 /* 0x001071f8: udf #0 */
    .inst 0x00000000 /* 0x001071fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107200: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107204: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107208: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010720c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107210: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107214: mov x0, x18 */
    .inst 0xd2804001 /* 0x00107218: mov x1, #0x200 */
    .inst 0xd53bd052 /* 0x0010721c: mrs x18, tpidr_el0 */
    .inst 0x14000183 /* 0x00107220: b #0x10782c */
    .inst 0x00000000 /* 0x00107224: udf #0 */
    .inst 0x00000000 /* 0x00107228: udf #0 */
    .inst 0x00000000 /* 0x0010722c: udf #0 */
    .inst 0x00000000 /* 0x00107230: udf #0 */
    .inst 0x00000000 /* 0x00107234: udf #0 */
    .inst 0x00000000 /* 0x00107238: udf #0 */
    .inst 0x00000000 /* 0x0010723c: udf #0 */
    .inst 0x00000000 /* 0x00107240: udf #0 */
    .inst 0x00000000 /* 0x00107244: udf #0 */
    .inst 0x00000000 /* 0x00107248: udf #0 */
    .inst 0x00000000 /* 0x0010724c: udf #0 */
    .inst 0x00000000 /* 0x00107250: udf #0 */
    .inst 0x00000000 /* 0x00107254: udf #0 */
    .inst 0x00000000 /* 0x00107258: udf #0 */
    .inst 0x00000000 /* 0x0010725c: udf #0 */
    .inst 0x00000000 /* 0x00107260: udf #0 */
    .inst 0x00000000 /* 0x00107264: udf #0 */
    .inst 0x00000000 /* 0x00107268: udf #0 */
    .inst 0x00000000 /* 0x0010726c: udf #0 */
    .inst 0x00000000 /* 0x00107270: udf #0 */
    .inst 0x00000000 /* 0x00107274: udf #0 */
    .inst 0x00000000 /* 0x00107278: udf #0 */
    .inst 0x00000000 /* 0x0010727c: udf #0 */
    .inst 0xd51bd052 /* 0x00107280: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107284: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107288: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010728c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107290: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107294: mov x0, x18 */
    .inst 0xd2805001 /* 0x00107298: mov x1, #0x280 */
    .inst 0xd53bd052 /* 0x0010729c: mrs x18, tpidr_el0 */
    .inst 0x14000163 /* 0x001072a0: b #0x10782c */
    .inst 0x00000000 /* 0x001072a4: udf #0 */
    .inst 0x00000000 /* 0x001072a8: udf #0 */
    .inst 0x00000000 /* 0x001072ac: udf #0 */
    .inst 0x00000000 /* 0x001072b0: udf #0 */
    .inst 0x00000000 /* 0x001072b4: udf #0 */
    .inst 0x00000000 /* 0x001072b8: udf #0 */
    .inst 0x00000000 /* 0x001072bc: udf #0 */
    .inst 0x00000000 /* 0x001072c0: udf #0 */
    .inst 0x00000000 /* 0x001072c4: udf #0 */
    .inst 0x00000000 /* 0x001072c8: udf #0 */
    .inst 0x00000000 /* 0x001072cc: udf #0 */
    .inst 0x00000000 /* 0x001072d0: udf #0 */
    .inst 0x00000000 /* 0x001072d4: udf #0 */
    .inst 0x00000000 /* 0x001072d8: udf #0 */
    .inst 0x00000000 /* 0x001072dc: udf #0 */
    .inst 0x00000000 /* 0x001072e0: udf #0 */
    .inst 0x00000000 /* 0x001072e4: udf #0 */
    .inst 0x00000000 /* 0x001072e8: udf #0 */
    .inst 0x00000000 /* 0x001072ec: udf #0 */
    .inst 0x00000000 /* 0x001072f0: udf #0 */
    .inst 0x00000000 /* 0x001072f4: udf #0 */
    .inst 0x00000000 /* 0x001072f8: udf #0 */
    .inst 0x00000000 /* 0x001072fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107300: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107304: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107308: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010730c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107310: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107314: mov x0, x18 */
    .inst 0xd2806001 /* 0x00107318: mov x1, #0x300 */
    .inst 0xd53bd052 /* 0x0010731c: mrs x18, tpidr_el0 */
    .inst 0x14000143 /* 0x00107320: b #0x10782c */
    .inst 0x00000000 /* 0x00107324: udf #0 */
    .inst 0x00000000 /* 0x00107328: udf #0 */
    .inst 0x00000000 /* 0x0010732c: udf #0 */
    .inst 0x00000000 /* 0x00107330: udf #0 */
    .inst 0x00000000 /* 0x00107334: udf #0 */
    .inst 0x00000000 /* 0x00107338: udf #0 */
    .inst 0x00000000 /* 0x0010733c: udf #0 */
    .inst 0x00000000 /* 0x00107340: udf #0 */
    .inst 0x00000000 /* 0x00107344: udf #0 */
    .inst 0x00000000 /* 0x00107348: udf #0 */
    .inst 0x00000000 /* 0x0010734c: udf #0 */
    .inst 0x00000000 /* 0x00107350: udf #0 */
    .inst 0x00000000 /* 0x00107354: udf #0 */
    .inst 0x00000000 /* 0x00107358: udf #0 */
    .inst 0x00000000 /* 0x0010735c: udf #0 */
    .inst 0x00000000 /* 0x00107360: udf #0 */
    .inst 0x00000000 /* 0x00107364: udf #0 */
    .inst 0x00000000 /* 0x00107368: udf #0 */
    .inst 0x00000000 /* 0x0010736c: udf #0 */
    .inst 0x00000000 /* 0x00107370: udf #0 */
    .inst 0x00000000 /* 0x00107374: udf #0 */
    .inst 0x00000000 /* 0x00107378: udf #0 */
    .inst 0x00000000 /* 0x0010737c: udf #0 */
    .inst 0xd51bd052 /* 0x00107380: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107384: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107388: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010738c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107390: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107394: mov x0, x18 */
    .inst 0xd2807001 /* 0x00107398: mov x1, #0x380 */
    .inst 0xd53bd052 /* 0x0010739c: mrs x18, tpidr_el0 */
    .inst 0x14000123 /* 0x001073a0: b #0x10782c */
    .inst 0x00000000 /* 0x001073a4: udf #0 */
    .inst 0x00000000 /* 0x001073a8: udf #0 */
    .inst 0x00000000 /* 0x001073ac: udf #0 */
    .inst 0x00000000 /* 0x001073b0: udf #0 */
    .inst 0x00000000 /* 0x001073b4: udf #0 */
    .inst 0x00000000 /* 0x001073b8: udf #0 */
    .inst 0x00000000 /* 0x001073bc: udf #0 */
    .inst 0x00000000 /* 0x001073c0: udf #0 */
    .inst 0x00000000 /* 0x001073c4: udf #0 */
    .inst 0x00000000 /* 0x001073c8: udf #0 */
    .inst 0x00000000 /* 0x001073cc: udf #0 */
    .inst 0x00000000 /* 0x001073d0: udf #0 */
    .inst 0x00000000 /* 0x001073d4: udf #0 */
    .inst 0x00000000 /* 0x001073d8: udf #0 */
    .inst 0x00000000 /* 0x001073dc: udf #0 */
    .inst 0x00000000 /* 0x001073e0: udf #0 */
    .inst 0x00000000 /* 0x001073e4: udf #0 */
    .inst 0x00000000 /* 0x001073e8: udf #0 */
    .inst 0x00000000 /* 0x001073ec: udf #0 */
    .inst 0x00000000 /* 0x001073f0: udf #0 */
    .inst 0x00000000 /* 0x001073f4: udf #0 */
    .inst 0x00000000 /* 0x001073f8: udf #0 */
    .inst 0x00000000 /* 0x001073fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107400: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107404: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107408: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010740c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107410: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107414: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107418: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010741c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x00107420: str x1, [x18, #8] */
    .inst 0xd2808001 /* 0x00107424: mov x1, #0x400 */
    .inst 0xd53bd052 /* 0x00107428: mrs x18, tpidr_el0 */
    .inst 0x14000100 /* 0x0010742c: b #0x10782c */
    .inst 0x00000000 /* 0x00107430: udf #0 */
    .inst 0x00000000 /* 0x00107434: udf #0 */
    .inst 0x00000000 /* 0x00107438: udf #0 */
    .inst 0x00000000 /* 0x0010743c: udf #0 */
    .inst 0x00000000 /* 0x00107440: udf #0 */
    .inst 0x00000000 /* 0x00107444: udf #0 */
    .inst 0x00000000 /* 0x00107448: udf #0 */
    .inst 0x00000000 /* 0x0010744c: udf #0 */
    .inst 0x00000000 /* 0x00107450: udf #0 */
    .inst 0x00000000 /* 0x00107454: udf #0 */
    .inst 0x00000000 /* 0x00107458: udf #0 */
    .inst 0x00000000 /* 0x0010745c: udf #0 */
    .inst 0x00000000 /* 0x00107460: udf #0 */
    .inst 0x00000000 /* 0x00107464: udf #0 */
    .inst 0x00000000 /* 0x00107468: udf #0 */
    .inst 0x00000000 /* 0x0010746c: udf #0 */
    .inst 0x00000000 /* 0x00107470: udf #0 */
    .inst 0x00000000 /* 0x00107474: udf #0 */
    .inst 0x00000000 /* 0x00107478: udf #0 */
    .inst 0x00000000 /* 0x0010747c: udf #0 */
    .inst 0xd51bd052 /* 0x00107480: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107484: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107488: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010748c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107490: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107494: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107498: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010749c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x001074a0: str x1, [x18, #8] */
    .inst 0xd2809001 /* 0x001074a4: mov x1, #0x480 */
    .inst 0xd53bd052 /* 0x001074a8: mrs x18, tpidr_el0 */
    .inst 0x140000e0 /* 0x001074ac: b #0x10782c */
    .inst 0x00000000 /* 0x001074b0: udf #0 */
    .inst 0x00000000 /* 0x001074b4: udf #0 */
    .inst 0x00000000 /* 0x001074b8: udf #0 */
    .inst 0x00000000 /* 0x001074bc: udf #0 */
    .inst 0x00000000 /* 0x001074c0: udf #0 */
    .inst 0x00000000 /* 0x001074c4: udf #0 */
    .inst 0x00000000 /* 0x001074c8: udf #0 */
    .inst 0x00000000 /* 0x001074cc: udf #0 */
    .inst 0x00000000 /* 0x001074d0: udf #0 */
    .inst 0x00000000 /* 0x001074d4: udf #0 */
    .inst 0x00000000 /* 0x001074d8: udf #0 */
    .inst 0x00000000 /* 0x001074dc: udf #0 */
    .inst 0x00000000 /* 0x001074e0: udf #0 */
    .inst 0x00000000 /* 0x001074e4: udf #0 */
    .inst 0x00000000 /* 0x001074e8: udf #0 */
    .inst 0x00000000 /* 0x001074ec: udf #0 */
    .inst 0x00000000 /* 0x001074f0: udf #0 */
    .inst 0x00000000 /* 0x001074f4: udf #0 */
    .inst 0x00000000 /* 0x001074f8: udf #0 */
    .inst 0x00000000 /* 0x001074fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107500: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107504: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107508: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010750c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107510: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107514: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107518: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010751c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x00107520: str x1, [x18, #8] */
    .inst 0xd280a001 /* 0x00107524: mov x1, #0x500 */
    .inst 0xd53bd052 /* 0x00107528: mrs x18, tpidr_el0 */
    .inst 0x140000c0 /* 0x0010752c: b #0x10782c */
    .inst 0x00000000 /* 0x00107530: udf #0 */
    .inst 0x00000000 /* 0x00107534: udf #0 */
    .inst 0x00000000 /* 0x00107538: udf #0 */
    .inst 0x00000000 /* 0x0010753c: udf #0 */
    .inst 0x00000000 /* 0x00107540: udf #0 */
    .inst 0x00000000 /* 0x00107544: udf #0 */
    .inst 0x00000000 /* 0x00107548: udf #0 */
    .inst 0x00000000 /* 0x0010754c: udf #0 */
    .inst 0x00000000 /* 0x00107550: udf #0 */
    .inst 0x00000000 /* 0x00107554: udf #0 */
    .inst 0x00000000 /* 0x00107558: udf #0 */
    .inst 0x00000000 /* 0x0010755c: udf #0 */
    .inst 0x00000000 /* 0x00107560: udf #0 */
    .inst 0x00000000 /* 0x00107564: udf #0 */
    .inst 0x00000000 /* 0x00107568: udf #0 */
    .inst 0x00000000 /* 0x0010756c: udf #0 */
    .inst 0x00000000 /* 0x00107570: udf #0 */
    .inst 0x00000000 /* 0x00107574: udf #0 */
    .inst 0x00000000 /* 0x00107578: udf #0 */
    .inst 0x00000000 /* 0x0010757c: udf #0 */
    .inst 0xd51bd052 /* 0x00107580: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107584: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107588: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010758c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107590: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107594: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107598: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010759c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x001075a0: str x1, [x18, #8] */
    .inst 0xd280b001 /* 0x001075a4: mov x1, #0x580 */
    .inst 0xd53bd052 /* 0x001075a8: mrs x18, tpidr_el0 */
    .inst 0x140000a0 /* 0x001075ac: b #0x10782c */
    .inst 0x00000000 /* 0x001075b0: udf #0 */
    .inst 0x00000000 /* 0x001075b4: udf #0 */
    .inst 0x00000000 /* 0x001075b8: udf #0 */
    .inst 0x00000000 /* 0x001075bc: udf #0 */
    .inst 0x00000000 /* 0x001075c0: udf #0 */
    .inst 0x00000000 /* 0x001075c4: udf #0 */
    .inst 0x00000000 /* 0x001075c8: udf #0 */
    .inst 0x00000000 /* 0x001075cc: udf #0 */
    .inst 0x00000000 /* 0x001075d0: udf #0 */
    .inst 0x00000000 /* 0x001075d4: udf #0 */
    .inst 0x00000000 /* 0x001075d8: udf #0 */
    .inst 0x00000000 /* 0x001075dc: udf #0 */
    .inst 0x00000000 /* 0x001075e0: udf #0 */
    .inst 0x00000000 /* 0x001075e4: udf #0 */
    .inst 0x00000000 /* 0x001075e8: udf #0 */
    .inst 0x00000000 /* 0x001075ec: udf #0 */
    .inst 0x00000000 /* 0x001075f0: udf #0 */
    .inst 0x00000000 /* 0x001075f4: udf #0 */
    .inst 0x00000000 /* 0x001075f8: udf #0 */
    .inst 0x00000000 /* 0x001075fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107600: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107604: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107608: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010760c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107610: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107614: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107618: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010761c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x00107620: str x1, [x18, #8] */
    .inst 0xd280c001 /* 0x00107624: mov x1, #0x600 */
    .inst 0xd53bd052 /* 0x00107628: mrs x18, tpidr_el0 */
    .inst 0x14000080 /* 0x0010762c: b #0x10782c */
    .inst 0x00000000 /* 0x00107630: udf #0 */
    .inst 0x00000000 /* 0x00107634: udf #0 */
    .inst 0x00000000 /* 0x00107638: udf #0 */
    .inst 0x00000000 /* 0x0010763c: udf #0 */
    .inst 0x00000000 /* 0x00107640: udf #0 */
    .inst 0x00000000 /* 0x00107644: udf #0 */
    .inst 0x00000000 /* 0x00107648: udf #0 */
    .inst 0x00000000 /* 0x0010764c: udf #0 */
    .inst 0x00000000 /* 0x00107650: udf #0 */
    .inst 0x00000000 /* 0x00107654: udf #0 */
    .inst 0x00000000 /* 0x00107658: udf #0 */
    .inst 0x00000000 /* 0x0010765c: udf #0 */
    .inst 0x00000000 /* 0x00107660: udf #0 */
    .inst 0x00000000 /* 0x00107664: udf #0 */
    .inst 0x00000000 /* 0x00107668: udf #0 */
    .inst 0x00000000 /* 0x0010766c: udf #0 */
    .inst 0x00000000 /* 0x00107670: udf #0 */
    .inst 0x00000000 /* 0x00107674: udf #0 */
    .inst 0x00000000 /* 0x00107678: udf #0 */
    .inst 0x00000000 /* 0x0010767c: udf #0 */
    .inst 0xd51bd052 /* 0x00107680: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107684: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107688: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010768c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107690: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107694: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107698: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010769c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x001076a0: str x1, [x18, #8] */
    .inst 0xd280d001 /* 0x001076a4: mov x1, #0x680 */
    .inst 0xd53bd052 /* 0x001076a8: mrs x18, tpidr_el0 */
    .inst 0x14000060 /* 0x001076ac: b #0x10782c */
    .inst 0x00000000 /* 0x001076b0: udf #0 */
    .inst 0x00000000 /* 0x001076b4: udf #0 */
    .inst 0x00000000 /* 0x001076b8: udf #0 */
    .inst 0x00000000 /* 0x001076bc: udf #0 */
    .inst 0x00000000 /* 0x001076c0: udf #0 */
    .inst 0x00000000 /* 0x001076c4: udf #0 */
    .inst 0x00000000 /* 0x001076c8: udf #0 */
    .inst 0x00000000 /* 0x001076cc: udf #0 */
    .inst 0x00000000 /* 0x001076d0: udf #0 */
    .inst 0x00000000 /* 0x001076d4: udf #0 */
    .inst 0x00000000 /* 0x001076d8: udf #0 */
    .inst 0x00000000 /* 0x001076dc: udf #0 */
    .inst 0x00000000 /* 0x001076e0: udf #0 */
    .inst 0x00000000 /* 0x001076e4: udf #0 */
    .inst 0x00000000 /* 0x001076e8: udf #0 */
    .inst 0x00000000 /* 0x001076ec: udf #0 */
    .inst 0x00000000 /* 0x001076f0: udf #0 */
    .inst 0x00000000 /* 0x001076f4: udf #0 */
    .inst 0x00000000 /* 0x001076f8: udf #0 */
    .inst 0x00000000 /* 0x001076fc: udf #0 */
    .inst 0xd51bd052 /* 0x00107700: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107704: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107708: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010770c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107710: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107714: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107718: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010771c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x00107720: str x1, [x18, #8] */
    .inst 0xd280e001 /* 0x00107724: mov x1, #0x700 */
    .inst 0xd53bd052 /* 0x00107728: mrs x18, tpidr_el0 */
    .inst 0x14000040 /* 0x0010772c: b #0x10782c */
    .inst 0x00000000 /* 0x00107730: udf #0 */
    .inst 0x00000000 /* 0x00107734: udf #0 */
    .inst 0x00000000 /* 0x00107738: udf #0 */
    .inst 0x00000000 /* 0x0010773c: udf #0 */
    .inst 0x00000000 /* 0x00107740: udf #0 */
    .inst 0x00000000 /* 0x00107744: udf #0 */
    .inst 0x00000000 /* 0x00107748: udf #0 */
    .inst 0x00000000 /* 0x0010774c: udf #0 */
    .inst 0x00000000 /* 0x00107750: udf #0 */
    .inst 0x00000000 /* 0x00107754: udf #0 */
    .inst 0x00000000 /* 0x00107758: udf #0 */
    .inst 0x00000000 /* 0x0010775c: udf #0 */
    .inst 0x00000000 /* 0x00107760: udf #0 */
    .inst 0x00000000 /* 0x00107764: udf #0 */
    .inst 0x00000000 /* 0x00107768: udf #0 */
    .inst 0x00000000 /* 0x0010776c: udf #0 */
    .inst 0x00000000 /* 0x00107770: udf #0 */
    .inst 0x00000000 /* 0x00107774: udf #0 */
    .inst 0x00000000 /* 0x00107778: udf #0 */
    .inst 0x00000000 /* 0x0010777c: udf #0 */
    .inst 0xd51bd052 /* 0x00107780: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x00107784: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107788: ldr x18, [x18] */
    .inst 0xf9000a40 /* 0x0010778c: str x0, [x18, #0x10] */
    .inst 0xf9000e41 /* 0x00107790: str x1, [x18, #0x18] */
    .inst 0xaa1203e0 /* 0x00107794: mov x0, x18 */
    .inst 0xd53ed052 /* 0x00107798: mrs x18, tpidr_el3 */
    .inst 0xd53bd061 /* 0x0010779c: mrs x1, tpidrro_el0 */
    .inst 0xf9000641 /* 0x001077a0: str x1, [x18, #8] */
    .inst 0xd280f001 /* 0x001077a4: mov x1, #0x780 */
    .inst 0xd53bd052 /* 0x001077a8: mrs x18, tpidr_el0 */
    .inst 0x14000020 /* 0x001077ac: b #0x10782c */
    .inst 0x00000000 /* 0x001077b0: udf #0 */
    .inst 0x00000000 /* 0x001077b4: udf #0 */
    .inst 0x00000000 /* 0x001077b8: udf #0 */
    .inst 0x00000000 /* 0x001077bc: udf #0 */
    .inst 0x00000000 /* 0x001077c0: udf #0 */
    .inst 0x00000000 /* 0x001077c4: udf #0 */
    .inst 0x00000000 /* 0x001077c8: udf #0 */
    .inst 0x00000000 /* 0x001077cc: udf #0 */
    .inst 0x00000000 /* 0x001077d0: udf #0 */
    .inst 0x00000000 /* 0x001077d4: udf #0 */
    .inst 0x00000000 /* 0x001077d8: udf #0 */
    .inst 0x00000000 /* 0x001077dc: udf #0 */
    .inst 0x00000000 /* 0x001077e0: udf #0 */
    .inst 0x00000000 /* 0x001077e4: udf #0 */
    .inst 0x00000000 /* 0x001077e8: udf #0 */
    .inst 0x00000000 /* 0x001077ec: udf #0 */
    .inst 0x00000000 /* 0x001077f0: udf #0 */
    .inst 0x00000000 /* 0x001077f4: udf #0 */
    .inst 0x00000000 /* 0x001077f8: udf #0 */
    .inst 0x00000000 /* 0x001077fc: udf #0 */
    .inst 0xd51bd043 /* 0x00107800: msr tpidr_el0, x3 */
    .inst 0xd53800a3 /* 0x00107804: mrs x3, mpidr_el1 */
    .inst 0x92401c63 /* 0x00107808: and x3, x3, #0xff */
    .inst 0xb5000083 /* 0x0010780c: cbnz x3, #0x10781c */
    .inst 0x58000943 /* 0x00107810: ldr x3, #0x107938 */
    .inst 0x9100007f /* 0x00107814: mov sp, x3 */
    .inst 0x14000003 /* 0x00107818: b #0x107824 */
core1:
    .inst 0x58000923 /* 0x0010781c: ldr x3, #0x107940 */
    .inst 0x9100007f /* 0x00107820: mov sp, x3 */
join:
    .inst 0xd53bd043 /* 0x00107824: mrs x3, tpidr_el0 */
    .inst 0x14000203 /* 0x00107828: b #0x108034 */
_el3_serror:
    .inst 0xf9000401 /* 0x0010782c: str x1, [x0, #8] */
    .inst 0xd53ed041 /* 0x00107830: mrs x1, tpidr_el3 */
    .inst 0xd51bd061 /* 0x00107834: msr tpidrro_el0, x1 */
    .inst 0xd5384241 /* 0x00107838: mrs x1, currentel */
    .inst 0xf9008c01 /* 0x0010783c: str x1, [x0, #0x118] */
    .inst 0x91004000 /* 0x00107840: add x0, x0, #0x10 */
    .inst 0xa9810c02 /* 0x00107844: stp x2, x3, [x0, #0x10]! */
    .inst 0xa9811404 /* 0x00107848: stp x4, x5, [x0, #0x10]! */
    .inst 0xa9811c06 /* 0x0010784c: stp x6, x7, [x0, #0x10]! */
    .inst 0xa9812408 /* 0x00107850: stp x8, x9, [x0, #0x10]! */
    .inst 0xa9812c0a /* 0x00107854: stp x10, x11, [x0, #0x10]! */
    .inst 0xa981340c /* 0x00107858: stp x12, x13, [x0, #0x10]! */
    .inst 0xa9813c0e /* 0x0010785c: stp x14, x15, [x0, #0x10]! */
    .inst 0xa9814410 /* 0x00107860: stp x16, x17, [x0, #0x10]! */
    .inst 0xa9814c12 /* 0x00107864: stp x18, x19, [x0, #0x10]! */
    .inst 0xa9815414 /* 0x00107868: stp x20, x21, [x0, #0x10]! */
    .inst 0xa9815c16 /* 0x0010786c: stp x22, x23, [x0, #0x10]! */
    .inst 0xa9816418 /* 0x00107870: stp x24, x25, [x0, #0x10]! */
    .inst 0xa9816c1a /* 0x00107874: stp x26, x27, [x0, #0x10]! */
    .inst 0xa981741c /* 0x00107878: stp x28, x29, [x0, #0x10]! */
    .inst 0x910003e2 /* 0x0010787c: mov x2, sp */
    .inst 0xa981081e /* 0x00107880: stp x30, x2, [x0, #0x10]! */
    .inst 0xd53e4022 /* 0x00107884: mrs x2, elr_el3 */
    .inst 0xf9000802 /* 0x00107888: str x2, [x0, #0x10] */
    .inst 0xd5384242 /* 0x0010788c: mrs x2, currentel */
    .inst 0xf9000c02 /* 0x00107890: str x2, [x0, #0x18] */
    .inst 0xd53b4222 /* 0x00107894: mrs x2, daif */
    .inst 0xf9001002 /* 0x00107898: str x2, [x0, #0x20] */
    .inst 0xd53b4202 /* 0x0010789c: mrs x2, nzcv */
    .inst 0xf9001402 /* 0x001078a0: str x2, [x0, #0x28] */
    .inst 0xd53e4002 /* 0x001078a4: mrs x2, spsr_el3 */
    .inst 0xf9001802 /* 0x001078a8: str x2, [x0, #0x30] */
    .inst 0xd53e5202 /* 0x001078ac: mrs x2, esr_el3 */
    .inst 0xf9001c02 /* 0x001078b0: str x2, [x0, #0x38] */
    .inst 0xd53e6002 /* 0x001078b4: mrs x2, far_el3 */
    .inst 0xf9002002 /* 0x001078b8: str x2, [x0, #0x40] */
    .inst 0xd53ed040 /* 0x001078bc: mrs x0, tpidr_el3 */
    .inst 0xf9400000 /* 0x001078c0: ldr x0, [x0] */
    .inst 0xf9400401 /* 0x001078c4: ldr x1, [x0, #8] */
    .inst 0x94000445 /* 0x001078c8: bl #0x1089dc */
    .inst 0xd53ed040 /* 0x001078cc: mrs x0, tpidr_el3 */
    .inst 0xf9400000 /* 0x001078d0: ldr x0, [x0] */
    .inst 0x91008000 /* 0x001078d4: add x0, x0, #0x20 */
    .inst 0xa8c10c02 /* 0x001078d8: ldp x2, x3, [x0], #0x10 */
    .inst 0xa8c11404 /* 0x001078dc: ldp x4, x5, [x0], #0x10 */
    .inst 0xa8c11c06 /* 0x001078e0: ldp x6, x7, [x0], #0x10 */
    .inst 0xa8c12408 /* 0x001078e4: ldp x8, x9, [x0], #0x10 */
    .inst 0xa8c12c0a /* 0x001078e8: ldp x10, x11, [x0], #0x10 */
    .inst 0xa8c1340c /* 0x001078ec: ldp x12, x13, [x0], #0x10 */
    .inst 0xa8c13c0e /* 0x001078f0: ldp x14, x15, [x0], #0x10 */
    .inst 0xa8c14410 /* 0x001078f4: ldp x16, x17, [x0], #0x10 */
    .inst 0xa8c14c12 /* 0x001078f8: ldp x18, x19, [x0], #0x10 */
    .inst 0xa8c15414 /* 0x001078fc: ldp x20, x21, [x0], #0x10 */
    .inst 0xa8c15c16 /* 0x00107900: ldp x22, x23, [x0], #0x10 */
    .inst 0xa8c16418 /* 0x00107904: ldp x24, x25, [x0], #0x10 */
    .inst 0xa8c16c1a /* 0x00107908: ldp x26, x27, [x0], #0x10 */
    .inst 0xa8c1741c /* 0x0010790c: ldp x28, x29, [x0], #0x10 */
    .inst 0xa8c1041e /* 0x00107910: ldp x30, x1, [x0], #0x10 */
    .inst 0x9100003f /* 0x00107914: mov sp, x1 */
    .inst 0xd51bd052 /* 0x00107918: msr tpidr_el0, x18 */
    .inst 0xd53ed052 /* 0x0010791c: mrs x18, tpidr_el3 */
    .inst 0xf9400252 /* 0x00107920: ldr x18, [x18] */
    .inst 0xf9400a40 /* 0x00107924: ldr x0, [x18, #0x10] */
    .inst 0xf9400e41 /* 0x00107928: ldr x1, [x18, #0x18] */
    .inst 0xd53bd052 /* 0x0010792c: mrs x18, tpidr_el0 */
    .inst 0xd69f03e0 /* 0x00107930: eret */
    .inst 0xd503201f /* 0x00107934: nop */
    .inst 0x00128000 /* 0x00107938: <undecoded> */
    .inst 0x00000000 /* 0x0010793c: udf #0 */
    .inst 0x0012a000 /* 0x00107940: <undecoded> */
    .inst 0x00000000 /* 0x00107944: udf #0 */
