/*
   hc12mem - HC12 memory reader & writer
   io_mc9s12a64.h: MC9S12A64 I/O registers
   $Id$

   Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. Neither the name of the project nor the names of its contributors
      may be used to endorse or promote products derived from this software
      without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
   FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
   DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
   OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
   LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
   OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
   SUCH DAMAGE.
*/

#ifndef __IO_MC9S12A64_H
#define __IO_MC9S12A64_H

/* MEBI map 1 of 3 */

#define PORTA 0x0000
#  define PORTA_BIT7 0x80
#  define PORTA_BIT6 0x40
#  define PORTA_BIT5 0x20
#  define PORTA_BIT4 0x10
#  define PORTA_BIT3 0x08
#  define PORTA_BIT2 0x04
#  define PORTA_BIT1 0x02
#  define PORTA_BIT0 0x01
#define PORTB 0x0001
#  define PORTB_BIT7 0x80
#  define PORTB_BIT6 0x40
#  define PORTB_BIT5 0x20
#  define PORTB_BIT4 0x10
#  define PORTB_BIT3 0x08
#  define PORTB_BIT2 0x04
#  define PORTB_BIT1 0x02
#  define PORTB_BIT0 0x01
#define DDRA  0x0002
#  define DDRA_BIT7 0x80
#  define DDRA_BIT6 0x40
#  define DDRA_BIT5 0x20
#  define DDRA_BIT4 0x10
#  define DDRA_BIT3 0x08
#  define DDRA_BIT2 0x04
#  define DDRA_BIT1 0x02
#  define DDRA_BIT0 0x01
#define DDRB  0x0003
#  define DDRB_BIT7 0x80
#  define DDRB_BIT6 0x40
#  define DDRB_BIT5 0x20
#  define DDRB_BIT4 0x10
#  define DDRB_BIT3 0x08
#  define DDRB_BIT2 0x04
#  define DDRB_BIT1 0x02
#  define DDRB_BIT0 0x01
/* #define RESERVED_0x0004 0x0004 */
/* #define RESERVED_0x0005 0x0005 */
/* #define RESERVED_0x0006 0x0006 */
/* #define RESERVED_0x0007 0x0007 */
#define PORTE 0x0008
#  define PORTE_BIT7 0x80
#  define PORTE_BIT6 0x40
#  define PORTE_BIT5 0x20
#  define PORTE_BIT4 0x10
#  define PORTE_BIT3 0x08
#  define PORTE_BIT2 0x04
#  define PORTE_BIT1 0x02
#  define PORTE_BIT0 0x01
#define DDRE  0x0009
#  define DDRE_BIT7 0x80
#  define DDRE_BIT6 0x40
#  define DDRE_BIT5 0x20
#  define DDRE_BIT4 0x10
#  define DDRE_BIT3 0x08
#  define DDRE_BIT2 0x04
#  define DDRE_BIT1 0x02
#  define DDRE_BIT0 0x01
#define PEAR  0x000a
#  define PEAR_NOACCE 0x80
   /* #  define PEAR_ 0x40 */
#  define PEAR_PIP0E 0x20
#  define PEAR_NECLK 0x10
#  define PEAR_LSTRE 0x08
#  define PEAR_RDWE 0x04
   /* #  define PEAR_ 0x02 */
   /* #  define PEAR_ 0x01 */
#define MODE  0x000b
#  define MODE_MODC 0x80
#  define MODE_MODB 0x40
#  define MODE_MODA 0x20
   /* #  define MODE_ 0x10 */
#  define MODE_IVIS 0x08
   /* #  define MODE_ 0x04 */
#  define MODE_EMK 0x02
#  define MODE_EME 0x01
#define PUCR  0x000c
#  define PUCR_PUPKE 0x80
   /* #  define PUCR_ 0x40 */
   /* #  define PUCR_ 0x20 */
#  define PUCR_PUPEE 0x10
   /* #  define PUCR_ 0x08 */
   /* #  define PUCR_ 0x04 */
#  define PUCR_PUPBE 0x02
#  define PUCR_PUPAE 0x01
#define RDRIV 0x000d
#  define RDRIV_RDPK 0x80
   /* #  define RDRIV_ 0x40 */
   /* #  define RDRIV_ 0x20 */
#  define RDRIV_RDPE 0x10
   /* #  define RDRIV_ 0x08 */
   /* #  define RDRIV_ 0x04 */
#  define RDRIV_RDPB 0x02
#  define RDRIV_DRPA 0x01
#define EBICTL 0x00e
   /* #  define EBICTL_ 0x80 */
   /* #  define EBICTL_ 0x40 */
   /* #  define EBICTL_ 0x20 */
   /* #  define EBICTL_ 0x10 */
   /* #  define EBICTL_ 0x08 */
   /* #  define EBICTL_ 0x04 */
   /* #  define EBICTL_ 0x02 */
#  define EBICTL_ESTR 0x01
/* #define RESERVED_0x000f 0x000f */

/* MMC map 1 of 4 */

#define INITRM 0x0010
#  define INITRM_RAM15 0x80
#  define INITRM_RAM14 0x40
#  define INITRM_RAM13 0x20
#  define INITRM_RAM12 0x10
#  define INITRM_RAM11 0x08
   /* #  define INITRM_ 0x04 */
   /* #  define INITRM_ 0x02 */
#  define INITRM_RAMHAL 0x01
#define INITRG 0x0011
   /* #  define INITRG_ 0x80 */
#  define INITRG_REG14 0x40
#  define INITRG_REG13 0x20
#  define INITRG_REG12 0x10
#  define INITRG_REG11 0x08
   /* #  define INITRG_ 0x04 */
   /* #  define INITRG_ 0x02 */
   /* #  define INITRG_ 0x01 */
#define INITEE 0x0012
#  define INITEE_EE15 0x80
#  define INITEE_EE14 0x40
#  define INITEE_EE13 0x20
#  define INITEE_EE12 0x10
#  define INITEE_EE11 0x08
   /* #  define INITEE_ 0x04 */
   /* #  define INITEE_ 0x02 */
#  define INITEE_EEON 0x01
#define MISC 0x0013
   /* #  define MISC_ 0x80 */
   /* #  define MISC_ 0x40 */
   /* #  define MISC_ 0x20 */
   /* #  define MISC_ 0x10 */
#  define MISC_EXSTR1 0x08
#  define MISC_EXSTR0 0x04
#  define MISC_ROMHM 0x02
#  define MISC_ROMON 0x01
/* #define RESERVED_0x0014 0x0014 */

/* INT map 1 of 2 */

#define ITCR 0x0015
   /* #  define ITCR_ 0x80 */
   /* #  define ITCR_ 0x40 */
   /* #  define ITCR_ 0x20 */
#  define ITCR_WRINT 0x10
#  define ITCR_ADR3 0x08
#  define ITCR_ADR2 0x04
#  define ITCR_ADR1 0x02
#  define ITCR_ADR0 0x01
#define ITEST 0x0016
#  define ITEST_INTE 0x80
#  define ITEST_INTC 0x40
#  define ITEST_INTA 0x20
#  define ITEST_INT8 0x10
#  define ITEST_INT6 0x08
#  define ITEST_INT4 0x04
#  define ITEST_INT2 0x02
#  define ITEST_INT0 0x01

/* reserved 0x0017..0x0019 */

/* device id register */

#define PARTIDH 0x001a
#  define PARTIDH_ID15 0x80
#  define PARTIDH_ID14 0x40
#  define PARTIDH_ID13 0x20
#  define PARTIDH_ID12 0x10
#  define PARTIDH_ID11 0x08
#  define PARTIDH_ID10 0x04
#  define PARTIDH_ID9 0x02
#  define PARTIDH_ID8 0x01
#define PARTIDL 0x001b
#  define PARTIDL_ID7 0x80
#  define PARTIDL_ID6 0x40
#  define PARTIDL_ID5 0x20
#  define PARTIDL_ID4 0x10
#  define PARTIDL_ID3 0x08
#  define PARTIDL_ID2 0x04
#  define PARTIDL_ID1 0x02
#  define PARTIDL_ID0 0x01

/* MMC map 3 of 4 */

#define MEMSIZ0 0x001c
#  define MEMSIZ0_REG_SW0 0x80
   /* #  define MEMSIZ0_ 0x40 */
#  define MEMSIZ0_EEP_SW1 0x20
#  define MEMSIZ0_EEP_SW0 0x10
   /* #  define MEMSIZ0_ 0x08 */
#  define MEMSIZ0_RAM_SW2 0x04
#  define MEMSIZ0_RAM_SW1 0x02
#  define MEMSIZ0_RAM_SW0 0x01
#define MEMSIZ1 0x001d
#  define MEMSIZ1_ROM_SW1 0x80
#  define MEMSIZ1_ROM_SW0 0x40
   /* #  define MEMSIZ1_ 0x20 */
   /* #  define MEMSIZ1_ 0x10 */
   /* #  define MEMSIZ1_ 0x08 */
   /* #  define MEMSIZ1_ 0x04 */
#  define MEMSIZ1_PAG_SW1 0x02
#  define MEMSIZ1_PAG_SW0 0x01

/* MEBI map 2 of 3 */

#define INTCR 0x001e
#  define INTCR_IRQE 0x80
#  define INTCR_IRQEN 0x40
   /* #  define INTCR_ 0x20 */
   /* #  define INTCR_ 0x10 */
   /* #  define INTCR_ 0x08 */
   /* #  define INTCR_ 0x04 */
   /* #  define INTCR_ 0x02 */
   /* #  define INTCR_ 0x01 */

/* INT map 2 of 2 */

#define HPRIO  0x001f
#  define HPRIO_PSEL7 0x80
#  define HPRIO_PSEL6 0x40
#  define HPRIO_PSEL5 0x20
#  define HPRIO_PSEL4 0x10
#  define HPRIO_PSEL3 0x08
#  define HPRIO_PSEL2 0x04
#  define HPRIO_PSEL1 0x02
   /* #  define HPRIO_ 0x01 */

/* reserved 0x0020..0x0027 */

/* BKP */

#define BKPCT0 0x0028
#  define BKPCT0_BKEN 0x80
#  define BKPCT0_BKFULL 0x40
#  define BKPCT0_BKBDM 0x20
#  define BKPCT0_BKTAG 0x10
   /* #  define BKPCT0_ 0x08 */
   /* #  define BKPCT0_ 0x04 */
   /* #  define BKPCT0_ 0x02 */
   /* #  define BKPCT0_ 0x01 */
#define BKPCT1 0x0029
#  define BKPCT1_BK0MBH 0x80
#  define BKPCT1_BK0MBL 0x40
#  define BKPCT1_BK1MBH 0x20
#  define BKPCT1_BK1MBL 0x10
#  define BKPCT1_BK0RWE 0x08
#  define BKPCT1_BK0RW 0x04
#  define BKPCT1_BK1RWE 0x02
#  define BKPCT1_BK1RW 0x01
#define BKP0X  0x002a
   /* #  define BKP0X_ 0x80 */
   /* #  define BKP0X_ 0x40 */
#  define BKP0X_BK0V5 0x20
#  define BKP0X_BK0V4 0x10
#  define BKP0X_BK0V3 0x08
#  define BKP0X_BK0V2 0x04
#  define BKP0X_BK0V1 0x02
#  define BKP0X_BK0V0 0x01
#define BKP0H  0x002b
#define BKP0L  0x002c
#define BKP1X  0x002d
   /* #  define BKP1X_ 0x80 */
   /* #  define BKP1X_ 0x40 */
#  define BKP1X_BK1V5 0x20
#  define BKP1X_BK1V4 0x10
#  define BKP1X_BK1V3 0x08
#  define BKP1X_BK1V2 0x04
#  define BKP1X_BK1V1 0x02
#  define BKP1X_BK1V0 0x01
#define BKP1H  0x002e
#define BKP1L  0x002f

/* MMC map 4 of 4 */

#define PPAGE 0x0030
   /* #  define PPAGE_ 0x80 */
   /* #  define PPAGE_ 0x40 */
#  define PPAGE_PIX5 0x20
#  define PPAGE_PIX4 0x10
#  define PPAGE_PIX3 0x08
#  define PPAGE_PIX2 0x04
#  define PPAGE_PIX1 0x02
#  define PPAGE_PIX0 0x01
/* #define RESERVED_0x0031 0x0031 */

/* MEBI map 3 of 3 */

#define PORTK 0x0032
#  define PORTK_BIT7 0x80
#  define PORTK_BIT6 0x40
#  define PORTK_BIT5 0x20
#  define PORTK_BIT4 0x10
#  define PORTK_BIT3 0x08
#  define PORTK_BIT2 0x04
#  define PORTK_BIT1 0x02
#  define PORTK_BIT0 0x01
#define DDRK  0x0033
#  define DDRK_BIT7 0x80
#  define DDRK_BIT6 0x40
#  define DDRK_BIT5 0x20
#  define DDRK_BIT4 0x10
#  define DDRK_BIT3 0x08
#  define DDRK_BIT2 0x04
#  define DDRK_BIT1 0x02
#  define DDRK_BIT0 0x01
#define SYNR  0x0034
   /* #  define SYNR_ 0x80 */
   /* #  define SYNR_ 0x40 */
#  define SYNR_SYN5 0x20
#  define SYNR_SYN4 0x10
#  define SYNR_SYN3 0x08
#  define SYNR_SYN2 0x04
#  define SYNR_SYN1 0x02
#  define SYNR_SYN0 0x01
#define REFDV 0x0035
   /* #  define REFDV_ 0x80 */
   /* #  define REFDV_ 0x40 */
   /* #  define REFDV_ 0x20 */
   /* #  define REFDV_ 0x10 */
#  define REFDV_REFDV3 0x08
#  define REFDV_REFDV2 0x04
#  define REFDV_REFDV1 0x02
#  define REFDV_REFDV0 0x01
#define CTFLG 0x0036 /* test only */
#define CRGFLG 0x0037
#  define CRGFLG_RTIF 0x80
#  define CRGFLG_PORF 0x40
   /* #  define CRGFLG_ 0x20 */
#  define CRGFLG_LOCKIF 0x10
#  define CRGFLG_LOCK 0x08
#  define CRGFLG_TRACK 0x04
#  define CRGFLG_SCMIF 0x02
#  define CRGFLG_SCM 0x01
#define CRGINT 0x0038
#  define CRGINT_RTIE 0x80
   /* #  define CRGINT_ 0x40 */
   /* #  define CRGINT_ 0x20 */
#  define CRGINT_LOCKIE 0x10
   /* #  define CRGINT_ 0x08 */
   /* #  define CRGINT_ 0x04 */
#  define CRGINT_SCMIE 0x02
   /* #  define CRGINT_ 0x01 */
#define CLKSEL 0x0039
#  define CLKSEL_PLLSEL 0x80
#  define CLKSEL_PSTP 0x40
#  define CLKSEL_SYSWAI 0x20
#  define CLKSEL_ROAWAI 0x10
#  define CLKSEL_PLLWAI 0x08
#  define CLKSEL_CWAI 0x04
#  define CLKSEL_RTIWAI 0x02
#  define CLKSEL_COPWAI 0x01
#define PLLCTL 0x003a
#  define PLLCTL_CME 0x80
#  define PLLCTL_PLLON 0x40
#  define PLLCTL_AUTO 0x20
#  define PLLCTL_ACQ 0x10
   /* #  define PLLCTL_ 0x08 */
#  define PLLCTL_PRE 0x04
#  define PLLCTL_PCE 0x02
#  define PLLCTL_SCME 0x01
#define RTICTL 0x003b
   /* #  define RTICTL_ 0x80 */
#  define RTICTL_RTR6 0x40
#  define RTICTL_RTR5 0x20
#  define RTICTL_RTR4 0x10
#  define RTICTL_RTR3 0x08
#  define RTICTL_RTR2 0x04
#  define RTICTL_RTR1 0x02
#  define RTICTL_RTR0 0x01
#define COPCTL 0x003c
#  define COPCTL_WCOP 0x80
#  define COPCTL_RSBCK 0x40
   /* #  define COPCTL_ 0x20 */
   /* #  define COPCTL_ 0x10 */
   /* #  define COPCTL_ 0x08 */
#  define COPCTL_CR2 0x04
#  define COPCTL_CR1 0x02
#  define COPCTL_CR0 0x01
#define FORBYP 0x003d /* test only */
#define CTCTL  0x003e /* test only */
#define ARMCOP 0x003f

/* ECT */

#define TIOS 0x0040
#  define TIOS_IOS7 0x80
#  define TIOS_IOS6 0x40
#  define TIOS_IOS5 0x20
#  define TIOS_IOS4 0x10
#  define TIOS_IOS3 0x08
#  define TIOS_IOS2 0x04
#  define TIOS_IOS1 0x02
#  define TIOS_IOS0 0x01
#define CFORC 0x0041
#  define CFORC_FOC7 0x80
#  define CFORC_FOC6 0x40
#  define CFORC_FOC5 0x20
#  define CFORC_FOC4 0x10
#  define CFORC_FOC3 0x08
#  define CFORC_FOC2 0x04
#  define CFORC_FOC1 0x02
#  define CFORC_FOC0 0x01
#define OC7M 0x0042
#  define OC7M_OC7M7 0x80
#  define OC7M_OC7M6 0x40
#  define OC7M_OC7M5 0x20
#  define OC7M_OC7M4 0x10
#  define OC7M_OC7M3 0x08
#  define OC7M_OC7M2 0x04
#  define OC7M_OC7M1 0x02
#  define OC7M_OC7M0 0x01
#define OC7D 0x0043
#  define OC7D_OC7D7 0x80
#  define OC7D_OC7D6 0x40
#  define OC7D_OC7D5 0x20
#  define OC7D_OC7D4 0x10
#  define OC7D_OC7D3 0x08
#  define OC7D_OC7D2 0x04
#  define OC7D_OC7D1 0x02
#  define OC7D_OC7D0 0x01
#define TCNT 0x0044
#define TCNTHI 0x0044
#define TCNTLO 0x0045
#define TSCR1 0x0046
#  define TSCR1_TEN 0x80
#  define TSCR1_TSWAI 0x40
#  define TSCR1_TSFRZ 0x20
#  define TSCR1_TFFCA 0x10
   /* #  define TSCR1_ 0x08 */
   /* #  define TSCR1_ 0x04 */
   /* #  define TSCR1_ 0x02 */
   /* #  define TSCR1_ 0x01 */
#define TTOV 0x0047
#  define TTOV_TOV7 0x80
#  define TTOV_TOV6 0x40
#  define TTOV_TOV5 0x20
#  define TTOV_TOV4 0x10
#  define TTOV_TOV3 0x08
#  define TTOV_TOV2 0x04
#  define TTOV_TOV1 0x02
#  define TTOV_TOV0 0x01
#define TCTL1 0x0048
#  define TCTL1_OM7 0x80
#  define TCTL1_OL7 0x40
#  define TCTL1_OM6 0x20
#  define TCTL1_OL6 0x10
#  define TCTL1_OM5 0x08
#  define TCTL1_OL5 0x04
#  define TCTL1_OM4 0x02
#  define TCTL1_OL4 0x01
#define TCTL2 0x0049
#  define TCTL2_OM3 0x80
#  define TCTL2_OL3 0x40
#  define TCTL2_OM2 0x20
#  define TCTL2_OL2 0x10
#  define TCTL2_OM1 0x08
#  define TCTL2_OL1 0x04
#  define TCTL2_OM0 0x02
#  define TCTL2_OL0 0x01
#define TCTL3 0x004a
#  define TCTL3_EDG7B 0x80
#  define TCTL3_EDG7A 0x40
#  define TCTL3_EDG6B 0x20
#  define TCTL3_EDG6A 0x10
#  define TCTL3_EDG5B 0x08
#  define TCTL3_EDG5A 0x04
#  define TCTL3_EDG4B 0x02
#  define TCTL3_EDG4A 0x01
#define TCTL4 0x004b
#  define TCTL4_EDG3B 0x80
#  define TCTL4_EDG3A 0x40
#  define TCTL4_EDG2B 0x20
#  define TCTL4_EDG2A 0x10
#  define TCTL4_EDG1B 0x08
#  define TCTL4_EDG1A 0x04
#  define TCTL4_EDG0B 0x02
#  define TCTL4_EDG0A 0x01
#define TIE 0x004c
#  define TIE_C7I 0x80
#  define TIE_C6I 0x40
#  define TIE_C5I 0x20
#  define TIE_C4I 0x10
#  define TIE_C3I 0x08
#  define TIE_C2I 0x04
#  define TIE_C1I 0x02
#  define TIE_C0I 0x01
#define TCSR2 0x004d
#  define TCSR2_TOI 0x80
   /* #  define TCSR2_ 0x40 */
   /* #  define TCSR2_ 0x20 */
   /* #  define TCSR2_ 0x10 */
#  define TCSR2_TCRE 0x08
#  define TCSR2_PR2 0x04
#  define TCSR2_PR1 0x02
#  define TCSR2_PR0 0x01
#define TFLG1 0x004e
#  define TFLG1_C7F 0x80
#  define TFLG1_C6F 0x40
#  define TFLG1_C5F 0x20
#  define TFLG1_C4F 0x10
#  define TFLG1_C3F 0x08
#  define TFLG1_C2F 0x04
#  define TFLG1_C1F 0x02
#  define TFLG1_C0F 0x01
#define TFLG2 0x004f
#  define TFLG2_TOF 0x80
   /* #  define TFLG2_ 0x40 */
   /* #  define TFLG2_ 0x20 */
   /* #  define TFLG2_ 0x10 */
   /* #  define TFLG2_ 0x08 */
   /* #  define TFLG2_ 0x04 */
   /* #  define TFLG2_ 0x02 */
   /* #  define TFLG2_ 0x01 */
#define TC0 0x0050
#define TC0HI 0x0050
#define TC0LO 0x005
#define TC1 0x0052
#define TC1HI 0x0052
#define TC1LO 0x005
#define TC2 0x0054
#define TC2HI 0x0054
#define TC2LO 0x005
#define TC3 0x0056
#define TC3HI 0x0056
#define TC3LO 0x005
#define TC4 0x0058
#define TC4HI 0x0058
#define TC4LO 0x005
#define TC5 0x005a
#define TC5HI 0x005a
#define TC5LO 0x005
#define TC6 0x005c
#define TC6HI 0x005c
#define TC6LO 0x005
#define TC7 0x005e
#define TC7HI 0x005e
#define TC7LO 0x005
#define PACTL 0x0060
   /* #  define PACTL_ 0x80 */
#  define PACTL_PAEN 0x40
#  define PACTL_PAMOD 0x20
#  define PACTL_PEDGE 0x10
#  define PACTL_CLK1 0x08
#  define PACTL_CLK0 0x04
#  define PACTL_PAOVI 0x02
#  define PACTL_PAIF 0x01
#define PAFLG 0x0061
   /* #  define PAFLG_ 0x80 */
   /* #  define PAFLG_ 0x40 */
   /* #  define PAFLG_ 0x20 */
   /* #  define PAFLG_ 0x10 */
   /* #  define PAFLG_ 0x08 */
   /* #  define PAFLG_ 0x04 */
#  define PAFLG_PAOVF 0x02
#  define PAFLG_PAIF 0x01
#define PACN3 0x0062
#define PACN2 0x0063
#define PACN1 0x0064
#define PACN0 0x0065
#define MCCTL 0x0066
#  define MCCTL_MCZI 0x80
#  define MCCTL_MODMC 0x40
#  define MCCTL_RDMCL 0x20
#  define MCCTL_ICLAT 0x10
#  define MCCTL_FLMC 0x08
#  define MCCTL_MCEN 0x04
#  define MCCTL_MCPR1 0x02
#  define MCCTL_MCPR0 0x01
#define MCFLG 0x0067
#  define MCFLG_MCZF 0x80
   /* #  define MCFLG_ 0x40 */
   /* #  define MCFLG_ 0x20 */
   /* #  define MCFLG_ 0x10 */
#  define MCFLG_POLF3 0x08
#  define MCFLG_POLF2 0x04
#  define MCFLG_POLF1 0x02
#  define MCFLG_POLF0 0x01
#define ICPAR 0x0068
   /* #  define ICPAR_ 0x80 */
   /* #  define ICPAR_ 0x40 */
   /* #  define ICPAR_ 0x20 */
   /* #  define ICPAR_ 0x10 */
#  define ICPAR_PA3EN 0x08
#  define ICPAR_PA2EN 0x04
#  define ICPAR_PA1EN 0x02
#  define ICPAR_PA0EN 0x01
#define DLYCT 0x0069
   /* #  define DLYCT_ 0x80 */
   /* #  define DLYCT_ 0x40 */
   /* #  define DLYCT_ 0x20 */
   /* #  define DLYCT_ 0x10 */
   /* #  define DLYCT_ 0x08 */
   /* #  define DLYCT_ 0x04 */
#  define DLYCT_DLY1 0x02
#  define DLYCT_DLY0 0x01
#define ICOVW 0x006a
#  define ICOVW_NOVW7 0x80
#  define ICOVW_NOVW6 0x40
#  define ICOVW_NOVW5 0x20
#  define ICOVW_NOVW4 0x10
#  define ICOVW_NOVW3 0x08
#  define ICOVW_NOVW2 0x04
#  define ICOVW_NOVW1 0x02
#  define ICOVW_NOVW0 0x01
#define ICSYS 0x006b
#  define ICSYS_SH37 0x80
#  define ICSYS_SH26 0x40
#  define ICSYS_SH15 0x20
#  define ICSYS_SH04 0x10
#  define ICSYS_TFMOD 0x08
#  define ICSYS_PACMX 0x04
#  define ICSYS_BUFEN 0x02
#  define ICSYS_LATQ 0x01
/* #define RESERVED_0x006c 0x006c */
#define TIMTST 0x006d /* test only */
   /* #  define TIMTST_ 0x80 */
   /* #  define TIMTST_ 0x40 */
   /* #  define TIMTST_ 0x20 */
   /* #  define TIMTST_ 0x10 */
   /* #  define TIMTST_ 0x08 */
   /* #  define TIMTST_ 0x04 */
#  define TIMTST_TCBYP 0x02
   /* #  define TIMTST_ 0x01 */
/* #define RESERVED_0x006e 0x006e */
/* #define RESERVED_0x006f 0x006f */
#define PBCTL 0x0070
   /* #  define PBCTL_ 0x80 */
#  define PBCTL_PBEN 0x40
   /* #  define PBCTL_ 0x20 */
   /* #  define PBCTL_ 0x10 */
   /* #  define PBCTL_ 0x08 */
   /* #  define PBCTL_ 0x04 */
#  define PBCTL_PBOVI 0x02
   /* #  define PBCTL_ 0x01 */
#define PBFLG 0x0071
   /* #  define PBFLG_ 0x80 */
   /* #  define PBFLG_ 0x40 */
   /* #  define PBFLG_ 0x20 */
   /* #  define PBFLG_ 0x10 */
   /* #  define PBFLG_ 0x08 */
   /* #  define PBFLG_ 0x04 */
#  define PBFLG_PBOVF 0x02
   /* #  define PBFLG_ 0x01 */
#define PA3H 0x0072
#define PA2H 0x0073
#define PA1H 0x0074
#define PA0H 0x0075
#define MCCNT 0x0076
#define MCCNTHI 0x0076
#define MCCNTLO 0x007
#define TC0H 0x0078
#define TC0HHI 0x0078
#define TC0HLO 0x007
#define TC1H 0x007a
#define TC1HHI 0x007a
#define TC1HLO 0x007
#define TC2H 0x007c
#define TC2HHI 0x007c
#define TC2HLO 0x007
#define TC3H 0x007e
#define TC3HHI 0x007e
#define TC3HLO 0x007

/* ATD0 */

#define ATD0CTL0 0x0080
#define ATD0CTL1 0x0081
#define ATD0CTL2 0x0082
#  define ATD0CTL2_ADPU 0x80
#  define ATD0CTL2_AFFC 0x40
#  define ATD0CTL2_AWAI 0x20
#  define ATD0CTL2_ETRIGLE 0x10
#  define ATD0CTL2_ETRIGP 0x08
#  define ATD0CTL2_ETRIG 0x04
#  define ATD0CTL2_ASCIE 0x02
#  define ATD0CTL2_ASCIF 0x01
#define ATD0CTL3 0x0083
   /* #  define ATD0CTL3_ 0x80 */
#  define ATD0CTL3_S8C 0x40
#  define ATD0CTL3_S4C 0x20
#  define ATD0CTL3_S2C 0x10
#  define ATD0CTL3_S1C 0x08
#  define ATD0CTL3_FIFO 0x04
#  define ATD0CTL3_FRZ1 0x02
#  define ATD0CTL3_FRZ0 0x01
#define ATD0CTL4 0x0084
#  define ATD0CTL4_SRES8 0x80
#  define ATD0CTL4_SMP1 0x40
#  define ATD0CTL4_SMP0 0x20
#  define ATD0CTL4_PRS4 0x10
#  define ATD0CTL4_PRS3 0x08
#  define ATD0CTL4_PRS2 0x04
#  define ATD0CTL4_PRS1 0x02
#  define ATD0CTL4_PRS0 0x01
#define ATD0CTL5 0x0085
#  define ATD0CTL5_DJM 0x80
#  define ATD0CTL5_DSGN 0x40
#  define ATD0CTL5_SCAN 0x20
#  define ATD0CTL5_MULT 0x10
   /* #  define ATD0CTL5_ 0x08 */
#  define ATD0CTL5_CC 0x04
#  define ATD0CTL5_CB 0x02
#  define ATD0CTL5_CA 0x01
#define ATD0STAT0 0x0086
#  define ATD0STAT0_SCF 0x80
   /* #  define ATD0STAT0_ 0x40 */
#  define ATD0STAT0_ETORF 0x20
#  define ATD0STAT0_FIFOR 0x10
   /* #  define ATD0STAT0_ 0x08 */
#  define ATD0STAT0_CC2 0x04
#  define ATD0STAT0_CC1 0x02
#  define ATD0STAT0_CC0 0x01
/* #define RESERVED_0x0087 0x0087 */
#define ATD0TEST0 0x0088
#define ATD0TEST1 0x0089
#  define ATD0TEST1_SC 0x01
/* #define RESERVED_0x008a 0x008a */
#define ATD0STAT1 0x008b
#  define ATD0STAT1_CCF7 0x80
#  define ATD0STAT1_CCF6 0x40
#  define ATD0STAT1_CCF5 0x20
#  define ATD0STAT1_CCF4 0x10
#  define ATD0STAT1_CCF3 0x08
#  define ATD0STAT1_CCF2 0x04
#  define ATD0STAT1_CCF1 0x02
#  define ATD0STAT1_CCF0 0x01
/* #define RESERVED_0x008c 0x008c */
#define ATD0DIEN 0x008d
/* #define RESERVED_0x008e 0x008e */
#define PORTAD0 0x008f
#  define PORTAD0_BIT7 0x80
#  define PORTAD0_BIT6 0x40
#  define PORTAD0_BIT5 0x20
#  define PORTAD0_BIT4 0x10
#  define PORTAD0_BIT3 0x08
#  define PORTAD0_BIT2 0x04
#  define PORTAD0_BIT1 0x02
#  define PORTAD0_BIT0 0x01
#define ATD0DR0H 0x0090
#define ATD0DR0L 0x0091
#define ATD0DR1H 0x0092
#define ATD0DR1L 0x0093
#define ATD0DR2H 0x0094
#define ATD0DR2L 0x0095
#define ATD0DR3H 0x0096
#define ATD0DR3L 0x0097
#define ATD0DR4H 0x0098
#define ATD0DR4L 0x0099
#define ATD0DR5H 0x009a
#define ATD0DR5L 0x009b
#define ATD0DR6H 0x009c
#define ATD0DR6L 0x009d
#define ATD0DR7H 0x009e
#define ATD0DR7L 0x009f

/* PWM */

#define PWME 0x00a0
#  define PWME_PWME7 0x80
#  define PWME_PWME6 0x40
#  define PWME_PWME5 0x20
#  define PWME_PWME4 0x10
#  define PWME_PWME3 0x08
#  define PWME_PWME2 0x04
#  define PWME_PWME1 0x02
#  define PWME_PWME0 0x01
#define PWMPOL 0x00a1
#  define PWMPOL_PPOL7 0x80
#  define PWMPOL_PPOL6 0x40
#  define PWMPOL_PPOL5 0x20
#  define PWMPOL_PPOL4 0x10
#  define PWMPOL_PPOL3 0x08
#  define PWMPOL_PPOL2 0x04
#  define PWMPOL_PPOL1 0x02
#  define PWMPOL_PPOL0 0x01
#define PWMCLK 0x00a2
#  define PWMCLK_PCLK7 0x80
#  define PWMCLK_PCLK6 0x40
#  define PWMCLK_PCLK5 0x20
#  define PWMCLK_PCLK4 0x10
#  define PWMCLK_PCLK3 0x08
#  define PWMCLK_PCLK2 0x04
#  define PWMCLK_PCLK1 0x02
#  define PWMCLK_PCLK0 0x01
#define PWMPRCLK 0x00a3
   /* #  define PWMPRCLK_ 0x80 */
#  define PWMPRCLK_PCKB2 0x40
#  define PWMPRCLK_PCKB1 0x20
#  define PWMPRCLK_PCKB0 0x10
   /* #  define PWMPRCLK_ 0x08 */
#  define PWMPRCLK_PCKA2 0x04
#  define PWMPRCLK_PCKA1 0x02
#  define PWMPRCLK_PCKA0 0x01
#define PWMCAE 0x00a4
#  define PWMCAE_CAE7 0x80
#  define PWMCAE_CAE6 0x40
#  define PWMCAE_CAE5 0x20
#  define PWMCAE_CAE4 0x10
#  define PWMCAE_CAE3 0x08
#  define PWMCAE_CAE2 0x04
#  define PWMCAE_CAE1 0x02
#  define PWMCAE_CAE0 0x01
#define PWMCTL 0x00a5
#  define PWMCTL_CON67 0x80
#  define PWMCTL_CON45 0x40
#  define PWMCTL_CON23 0x20
#  define PWMCTL_CON01 0x10
#  define PWMCTL_PSWAI 0x08
#  define PWMCTL_PFRZ 0x04
   /* #  define PWMCTL_ 0x02 */
   /* #  define PWMCTL_ 0x01 */
#define PWMTST 0x00a6 /* test only */
#define PWMPRSC 0x00a7 /* test only */
#define PWMSCLA 0x00a8
#define PWMSCLB 0x00a9
#define PWMSCNTA 0x00aa
#define PWMSCNTB 0x00ab
#define PWMCNT0 0x00ac
#define PWMCNT1 0x00ad
#define PWMCNT2 0x00ae
#define PWMCNT3 0x00af
#define PWMCNT4 0x00b0
#define PWMCNT5 0x00b1
#define PWMCNT6 0x00b2
#define PWMCNT7 0x00b3
#define PWMPER0 0x00b4
#define PWMPER1 0x00b5
#define PWMPER2 0x00b6
#define PWMPER3 0x00b7
#define PWMPER4 0x00b8
#define PWMPER5 0x00b9
#define PWMPER6 0x00ba
#define PWMPER7 0x00bb
#define PWMDTY0 0x00bc
#define PWMDTY1 0x00bd
#define PWMDTY2 0x00be
#define PWMDTY3 0x00bf

#define PWMDTY4 0x00c0
#define PWMDTY5 0x00c1
#define PWMDTY6 0x00c2
#define PWMDTY7 0x00c3
#define PWMSDN 0x00c4
#  define PWMSDN_PWMIF 0x80
#  define PWMSDN_PWMIE 0x40
#  define PWMSDN_PWMRSTRT 0x20
#  define PWMSDN_PWMLVL 0x10
   /* #  define PWMSDN_ 0x08 */
#  define PWMSDN_PWM7IN 0x04
#  define PWMSDN_PWM7INL 0x02
#  define PWMSDN_PWM7ENA 0x01
/* #define RESERVED_0x00c5 0x00c5 */
/* #define RESERVED_0x00c6 0x00c6 */
/* #define RESERVED_0x00c7 0x00c7 */

/* SCI0 */

#define SCI0BD  0x00c8
#define SCI0BDH 0x00c8
   /* #  define SCI0BDH_ 0x80 */
   /* #  define SCI0BDH_ 0x40 */
   /* #  define SCI0BDH_ 0x20 */
#  define SCI0BDH_SBR12 0x10
#  define SCI0BDH_SBR11 0x08
#  define SCI0BDH_SBR10 0x04
#  define SCI0BDH_SBR9 0x02
#  define SCI0BDH_SBR8 0x01
#define SCI0BDL 0x00c9
#  define SCI0BDL_SBR7 0x80
#  define SCI0BDL_SBR6 0x40
#  define SCI0BDL_SBR5 0x20
#  define SCI0BDL_SBR4 0x10
#  define SCI0BDL_SBR3 0x08
#  define SCI0BDL_SBR2 0x04
#  define SCI0BDL_SBR1 0x02
#  define SCI0BDL_SBR0 0x01
#define SCI0CR1 0x00ca
#  define SCI0CR1_LOOPS 0x80
#  define SCI0CR1_SCISWAI 0x40
#  define SCI0CR1_RSRC 0x20
#  define SCI0CR1_M 0x10
#  define SCI0CR1_WAKE 0x08
#  define SCI0CR1_ILT 0x04
#  define SCI0CR1_PE 0x02
#  define SCI0CR1_PT 0x01
#define SCI0CR2 0x00cb
#  define SCI0CR2_TIE 0x80
#  define SCI0CR2_TCIE 0x40
#  define SCI0CR2_RIE 0x20
#  define SCI0CR2_ILIE 0x10
#  define SCI0CR2_TE 0x08
#  define SCI0CR2_RE 0x04
#  define SCI0CR2_RWU 0x02
#  define SCI0CR2_SBK 0x01
#define SCI0SR1 0x00cc
#  define SCI0SR1_TDRE 0x80
#  define SCI0SR1_TC 0x40
#  define SCI0SR1_RDRF 0x20
#  define SCI0SR1_IDLE 0x10
#  define SCI0SR1_OR 0x08
#  define SCI0SR1_NF 0x04
#  define SCI0SR1_FE 0x02
#  define SCI0SR1_PF 0x01
#define SCI0SR2 0x00cd
   /* #  define SCI0SR2_ 0x80 */
   /* #  define SCI0SR2_ 0x40 */
   /* #  define SCI0SR2_ 0x20 */
   /* #  define SCI0SR2_ 0x10 */
   /* #  define SCI0SR2_ 0x08 */
#  define SCI0SR2_BRK13 0x04
#  define SCI0SR2_TXDIR 0x02
#  define SCI0SR2_RAF 0x01
#define SCI0DRH 0x00ce
#  define SCI0DRH_R8 0x80
#  define SCI0DRH_T8 0x40
   /* #  define SCI0DRH_ 0x20 */
   /* #  define SCI0DRH_ 0x10 */
   /* #  define SCI0DRH_ 0x08 */
   /* #  define SCI0DRH_ 0x04 */
   /* #  define SCI0DRH_ 0x02 */
   /* #  define SCI0DRH_ 0x01 */
#define SCI0DRL 0x00cf
#  define SCI0DRL_R7_T7 0x80
#  define SCI0DRL_R6_T6 0x40
#  define SCI0DRL_R5_T5 0x20
#  define SCI0DRL_R4_T4 0x10
#  define SCI0DRL_R3_T3 0x08
#  define SCI0DRL_R2_T2 0x04
#  define SCI0DRL_R1_T1 0x02
#  define SCI0DRL_R0_T0 0x01

/* SCI1 */

#define SCI1BD  0x00d0
#define SCI1BDH 0x00d0
   /* #  define SCI1BDH_ 0x80 */
   /* #  define SCI1BDH_ 0x40 */
   /* #  define SCI1BDH_ 0x20 */
#  define SCI1BDH_SBR12 0x10
#  define SCI1BDH_SBR11 0x08
#  define SCI1BDH_SBR10 0x04
#  define SCI1BDH_SBR9 0x02
#  define SCI1BDH_SBR8 0x01
#define SCI1BDL 0x00d1
#  define SCI1BDL_SBR7 0x80
#  define SCI1BDL_SBR6 0x40
#  define SCI1BDL_SBR5 0x20
#  define SCI1BDL_SBR4 0x10
#  define SCI1BDL_SBR3 0x08
#  define SCI1BDL_SBR2 0x04
#  define SCI1BDL_SBR1 0x02
#  define SCI1BDL_SBR0 0x01
#define SCI1CR1 0x00d2
#  define SCI1CR1_LOOPS 0x80
#  define SCI1CR1_SCISWAI 0x40
#  define SCI1CR1_RSRC 0x20
#  define SCI1CR1_M 0x10
#  define SCI1CR1_WAKE 0x08
#  define SCI1CR1_ILT 0x04
#  define SCI1CR1_PE 0x02
#  define SCI1CR1_PT 0x01
#define SCI1CR2 0x00d3
#  define SCI1CR2_TIE 0x80
#  define SCI1CR2_TCIE 0x40
#  define SCI1CR2_RIE 0x20
#  define SCI1CR2_ILIE 0x10
#  define SCI1CR2_TE 0x08
#  define SCI1CR2_RE 0x04
#  define SCI1CR2_RWU 0x02
#  define SCI1CR2_SBK 0x01
#define SCI1SR1 0x00d4
#  define SCI1SR1_TDRE 0x80
#  define SCI1SR1_TC 0x40
#  define SCI1SR1_RDRF 0x20
#  define SCI1SR1_IDLE 0x10
#  define SCI1SR1_OR 0x08
#  define SCI1SR1_NF 0x04
#  define SCI1SR1_FE 0x02
#  define SCI1SR1_PF 0x01
#define SCI1SR2 0x00d5
   /* #  define SCI1SR2_ 0x80 */
   /* #  define SCI1SR2_ 0x40 */
   /* #  define SCI1SR2_ 0x20 */
   /* #  define SCI1SR2_ 0x10 */
   /* #  define SCI1SR2_ 0x08 */
#  define SCI1SR2_BRK13 0x04
#  define SCI1SR2_TXDIR 0x02
#  define SCI1SR2_RAF 0x01
#define SCI1DRH 0x00d6
#  define SCI1DRH_R8 0x80
#  define SCI1DRH_T8 0x40
   /* #  define SCI1DRH_ 0x20 */
   /* #  define SCI1DRH_ 0x10 */
   /* #  define SCI1DRH_ 0x08 */
   /* #  define SCI1DRH_ 0x04 */
   /* #  define SCI1DRH_ 0x02 */
   /* #  define SCI1DRH_ 0x01 */
#define SCI1DRL 0x00d7
#  define SCI1DRL_R7_T7 0x80
#  define SCI1DRL_R6_T6 0x40
#  define SCI1DRL_R5_T5 0x20
#  define SCI1DRL_R4_T4 0x10
#  define SCI1DRL_R3_T3 0x08
#  define SCI1DRL_R2_T2 0x04
#  define SCI1DRL_R1_T1 0x02
#  define SCI1DRL_R0_T0 0x01

/* SPI0 */

#define SPI0CR1 0x00d8
#  define SPI0CR1_SPIE 0x80
#  define SPI0CR1_SPE 0x40
#  define SPI0CR1_SPTIE 0x20
#  define SPI0CR1_MSTR 0x10
#  define SPI0CR1_CPOL 0x08
#  define SPI0CR1_CPHA 0x04
#  define SPI0CR1_SSOE 0x02
#  define SPI0CR1_LSBFE 0x01
#define SPI0CR2 0x00d9
   /* #  define SPI0CR2_ 0x80 */
   /* #  define SPI0CR2_ 0x40 */
   /* #  define SPI0CR2_ 0x20 */
#  define SPI0CR2_MODFEN 0x10
#  define SPI0CR2_BIDIROE 0x08
   /* #  define SPI0CR2_ 0x04 */
#  define SPI0CR2_SPISWAI 0x02
#  define SPI0CR2_SPC0 0x01
#define SPI0BR 0x00da
   /* #  define SPI0BR_ 0x80 */
#  define SPI0BR_SPPR2 0x40
#  define SPI0BR_SPPR1 0x20
#  define SPI0BR_SPPR0 0x10
   /* #  define SPI0BR_ 0x08 */
#  define SPI0BR_SPR2 0x04
#  define SPI0BR_SPR1 0x02
#  define SPI0BR_SPR0 0x01
#define SPI0SR 0x00db
#  define SPI0SR_SPIF 0x80
   /* #  define SPI0SR_ 0x40 */
#  define SPI0SR_SPTEF 0x20
#  define SPI0SR_MODF 0x10
   /* #  define SPI0SR_ 0x08 */
   /* #  define SPI0SR_ 0x04 */
   /* #  define SPI0SR_ 0x02 */
   /* #  define SPI0SR_ 0x01 */
/* #define RESERVED_0x00dc 0x00dc */
#define SPI0DR 0x00dd
/* #define RESERVED_0x00de 0x00de */
/* #define RESERVED_0x00df 0x00df */

/* IIC */

#define IBAD 0x00e0
#  define IBAD_ADR7 0x80
#  define IBAD_ADR6 0x40
#  define IBAD_ADR5 0x20
#  define IBAD_ADR4 0x10
#  define IBAD_ADR3 0x08
#  define IBAD_ADR2 0x04
#  define IBAD_ADR1 0x02
   /* #  define IBAD_ 0x01 */
#define IBFD 0x00e1
#  define IBFD_IBC7 0x80
#  define IBFD_IBC6 0x40
#  define IBFD_IBC5 0x20
#  define IBFD_IBC4 0x10
#  define IBFD_IBC3 0x08
#  define IBFD_IBC2 0x04
#  define IBFD_IBC1 0x02
#  define IBFD_IBC0 0x01
#define IBCR 0x00e2
#  define IBCR_IBEN 0x80
#  define IBCR_IBIE 0x40
#  define IBCR_MSSL 0x20
#  define IBCR_TXRX 0x10
#  define IBCR_TXAK 0x08
#  define IBCR_RSTA 0x04
   /* #  define IBCR_ 0x02 */
#  define IBCR_IBSWAI 0x01
#define IBSR 0x00e3
#  define IBSR_TCF 0x80
#  define IBSR_IAAS 0x40
#  define IBSR_IBB 0x20
#  define IBSR_IBAL 0x10
   /* #  define IBSR_ 0x08 */
#  define IBSR_SRW 0x04
#  define IBSR_IBIF 0x02
#  define IBSR_RXAK 0x01
#define IBDR 0x00e4
/* #define RESERVED_0x00e5 0x00e5 */
/* #define RESERVED_0x00e6 0x00e6 */
/* #define RESERVED_0x00e7 0x00e7 */

/* BDLC */

#define DLCBCR1 0x00e8
#  define DLCBCR1_IMSG 0x80
#  define DLCBCR1_CLKS 0x40
   /* #  define DLCBCR1_ 0x20 */
   /* #  define DLCBCR1_ 0x10 */
   /* #  define DLCBCR1_ 0x08 */
   /* #  define DLCBCR1_ 0x04 */
#  define DLCBCR1_IE 0x02
#  define DLCBCR1_WCM 0x01
#define DLCBSVR 0x00e9
   /* #  define DLCBSVR_ 0x80 */
   /* #  define DLCBSVR_ 0x40 */
#  define DLCBSVR_I3 0x20
#  define DLCBSVR_I2 0x10
#  define DLCBSVR_I1 0x08
#  define DLCBSVR_I0 0x04
   /* #  define DLCBSVR_ 0x02 */
   /* #  define DLCBSVR_ 0x01 */
#define DLCBCR2 0x00ea
#  define DLCBCR2_SMRST 0x80
#  define DLCBCR2_DLOOP 0x40
#  define DLCBCR2_RX4XE 0x20
#  define DLCBCR2_NBFS 0x10
#  define DLCBCR2_TEOD 0x08
#  define DLCBCR2_TSIFR 0x04
#  define DLCBCR2_TMIFR1 0x02
#  define DLCBCR2_TMIFR0 0x01
#define DLCBDR  0x00eb
#  define DLCBDR_D7 0x80
#  define DLCBDR_D6 0x40
#  define DLCBDR_D5 0x20
#  define DLCBDR_D4 0x10
#  define DLCBDR_D3 0x08
#  define DLCBDR_D2 0x04
#  define DLCBDR_D1 0x02
#  define DLCBDR_D0 0x01
#define DLCBARD 0x00ec
   /* #  define DLCBARD_ 0x80 */
#  define DLCBARD_RXPOL 0x40
   /* #  define DLCBARD_ 0x20 */
   /* #  define DLCBARD_ 0x10 */
#  define DLCBARD_BO3 0x08
#  define DLCBARD_BO2 0x04
#  define DLCBARD_BO1 0x02
#  define DLCBARD_BO0 0x01
#define DLCBRSR 0x00ed
   /* #  define DLCBRSR_ 0x80 */
   /* #  define DLCBRSR_ 0x40 */
#  define DLCBRSR_R5 0x20
#  define DLCBRSR_R4 0x10
#  define DLCBRSR_R3 0x08
#  define DLCBRSR_R2 0x04
#  define DLCBRSR_R1 0x02
#  define DLCBRSR_R0 0x01
#define DLCSCR  0x00ee
   /* #  define DLCSCR_ 0x80 */
   /* #  define DLCSCR_ 0x40 */
   /* #  define DLCSCR_ 0x20 */
#  define DLCSCR_BDLCE 0x10
   /* #  define DLCSCR_ 0x08 */
   /* #  define DLCSCR_ 0x04 */
   /* #  define DLCSCR_ 0x02 */
   /* #  define DLCSCR_ 0x01 */
#define DLCBSTAT 0x00ef
   /* #  define DLCBSTAT_ 0x80 */
   /* #  define DLCBSTAT_ 0x40 */
   /* #  define DLCBSTAT_ 0x20 */
   /* #  define DLCBSTAT_ 0x10 */
   /* #  define DLCBSTAT_ 0x08 */
   /* #  define DLCBSTAT_ 0x04 */
   /* #  define DLCBSTAT_ 0x02 */
#  define DLCBSTAT_IDLE 0x01

/* reserved 0x00f0..0x00ff */

/* FLASH */

#define FCLKDIV 0x0100
#  define FCLKDIV_FDIVLD 0x80
#  define FCLKDIV_PRDIV8 0x40
#  define FCLKDIV_FDIV5 0x20
#  define FCLKDIV_FDIV4 0x10
#  define FCLKDIV_FDIV3 0x08
#  define FCLKDIV_FDIV2 0x04
#  define FCLKDIV_FDIV1 0x02
#  define FCLKDIV_FDIV0 0x01
#define FSEC 0x0101
#  define FSEC_KEYEN 0x80
#  define FSEC_NV6 0x40
#  define FSEC_NV5 0x20
#  define FSEC_NV4 0x10
#  define FSEC_NV3 0x08
#  define FSEC_NV2 0x04
#  define FSEC_SEC1 0x02
#  define FSEC_SEC0 0x01
/* #define RESERVED_0x0102 0x0102 */
#define FCNFG 0x0103
#  define FCNFG_CBEIE 0x80
#  define FCNFG_CCIE 0x40
#  define FCNFG_KEYACC 0x20
   /* #  define FCNFG_ 0x10 */
   /* #  define FCNFG_ 0x08 */
   /* #  define FCNFG_ 0x04 */
   /* #  define FCNFG_ 0x02 */
   /* #  define FCNFG_ 0x01 */
#define FPROT 0x0104
#  define FPROT_FPOPEN 0x80
#  define FPROT_NV6 0x40
#  define FPROT_FPHDIS 0x20
#  define FPROT_FPHS1 0x10
#  define FPROT_FPHS0 0x08
#  define FPROT_FPLDIS 0x04
#  define FPROT_FPLS1 0x02
#  define FPROT_FPLS0 0x01
#define FSTAT 0x0105
#  define FSTAT_CBEIF 0x80
#  define FSTAT_CCIF 0x40
#  define FSTAT_PVIOL 0x20
#  define FSTAT_ACCERR 0x10
   /* #  define FSTAT_ 0x08 */
#  define FSTAT_BLANK 0x04
   /* #  define FSTAT_ 0x02 */
   /* #  define FSTAT_ 0x01 */
#define FCMD 0x0106
   /* #  define FCMD_ 0x80 */
#  define FCMD_CMDB6 0x40
#  define FCMD_CMDB5 0x20
   /* #  define FCMD_ 0x10 */
   /* #  define FCMD_ 0x08 */
#  define FCMD_CMDB2 0x04
   /* #  define FCMD_ 0x02 */
#  define FCMD_CMDB0 0x01
/* #define RESERVED_0x0107 0x0107 */
#define FADDR 0x0108
#define FADDRHI 0x0108
#define FADDRLO 0x0109
#define FDATA 0x010a
#define FDATAHI 0x010a
#define FDATALO 0x010b
/* #define RESERVED_0x010c 0x010c */
/* #define RESERVED_0x010d 0x010d */
/* #define RESERVED_0x010e 0x010e */
/* #define RESERVED_0x010f 0x010f */

/* EEPROM */

#define ECLKDIV 0x0110
#  define ECLKDIV_EDIVLD 0x80
#  define ECLKDIV_PRDIV8 0x40
#  define ECLKDIV_EDIV5 0x20
#  define ECLKDIV_EDIV4 0x10
#  define ECLKDIV_EDIV3 0x08
#  define ECLKDIV_EDIV2 0x04
#  define ECLKDIV_EDIV1 0x02
#  define ECLKDIV_EDIV0 0x01
/* #define RESERVED_0x0111 0x0111 */
/* #define RESERVED_0x0112 0x0112 */
#define ECNFG 0x0113
#  define ECNFG_CBEIE 0x80
#  define ECNFG_CCIE 0x40
   /* #  define ECNFG_ 0x20 */
   /* #  define ECNFG_ 0x10 */
   /* #  define ECNFG_ 0x08 */
   /* #  define ECNFG_ 0x04 */
   /* #  define ECNFG_ 0x02 */
   /* #  define ECNFG_ 0x01 */
#define EPROT 0x0114
#  define EPROT_EPOPEN 0x80
#  define EPROT_NV6 0x40
#  define EPROT_NV5 0x20
#  define EPROT_NV4 0x10
#  define EPROT_EPDIS 0x08
#  define EPROT_EP2 0x04
#  define EPROT_EP1 0x02
#  define EPROT_EP0 0x01
#define ESTAT 0x0115
#  define ESTAT_CBEIF 0x80
#  define ESTAT_CCIF 0x40
#  define ESTAT_PVIOL 0x20
#  define ESTAT_ACCERR 0x10
   /* #  define ESTAT_ 0x08 */
#  define ESTAT_BLANK 0x04
   /* #  define ESTAT_ 0x02 */
   /* #  define ESTAT_ 0x01 */
#define ECMD 0x0116
   /* #  define ECMD_ 0x80 */
#  define ECMD_CMDB6 0x40
#  define ECMD_CMDB5 0x20
   /* #  define ECMD_ 0x10 */
   /* #  define ECMD_ 0x08 */
#  define ECMD_CMDB2 0x04
   /* #  define ECMD_ 0x02 */
#  define ECMD_CMDB0 0x01
/* #define RESERVED_0x0117 0x0117 */
#define EADDR 0x0118
#define EADDRHI 0x0118
#define EADDRLO 0x0119
#define EDATA 0x011a
#define EDATAHI 0x011a
#define EDATALO 0x011b
/* #define RESERVED_0x011c 0x011c */
/* #define RESERVED_0x011d 0x011d */
/* #define RESERVED_0x011e 0x011e */
/* #define RESERVED_0x011f 0x011f */

/* ATD1 */

#define ATD1CTL0 0x0120
#define ATD1CTL1 0x0121
#define ATD1CTL2 0x0122
#  define ATD1CTL2_ADPU 0x80
#  define ATD1CTL2_AFFC 0x40
#  define ATD1CTL2_AWAI 0x20
#  define ATD1CTL2_ETRIGLE 0x10
#  define ATD1CTL2_ETRIGP 0x08
#  define ATD1CTL2_ETRIG 0x04
#  define ATD1CTL2_ASCIE 0x02
#  define ATD1CTL2_ASCIF 0x01
#define ATD1CTL3 0x0123
   /* #  define ATD1CTL3_ 0x80 */
#  define ATD1CTL3_S8C 0x40
#  define ATD1CTL3_S4C 0x20
#  define ATD1CTL3_S2C 0x10
#  define ATD1CTL3_S1C 0x08
#  define ATD1CTL3_FIFO 0x04
#  define ATD1CTL3_FRZ1 0x02
#  define ATD1CTL3_FRZ0 0x01
#define ATD1CTL4 0x0124
#  define ATD1CTL4_SRES8 0x80
#  define ATD1CTL4_SMP1 0x40
#  define ATD1CTL4_SMP0 0x20
#  define ATD1CTL4_PRS4 0x10
#  define ATD1CTL4_PRS3 0x08
#  define ATD1CTL4_PRS2 0x04
#  define ATD1CTL4_PRS1 0x02
#  define ATD1CTL4_PRS0 0x01
#define ATD1CTL5 0x0125
#  define ATD1CTL5_DJM 0x80
#  define ATD1CTL5_DSGN 0x40
#  define ATD1CTL5_SCAN 0x20
#  define ATD1CTL5_MULT 0x10
   /* #  define ATD1CTL5_ 0x08 */
#  define ATD1CTL5_CC 0x04
#  define ATD1CTL5_CB 0x02
#  define ATD1CTL5_CA 0x01
#define ATD1STAT0 0x0126
#  define ATD1STAT0_SCF 0x80
   /* #  define ATD1STAT0_ 0x40 */
#  define ATD1STAT0_ETORF 0x20
#  define ATD1STAT0_FIFOR 0x10
   /* #  define ATD1STAT0_ 0x08 */
#  define ATD1STAT0_CC2 0x04
#  define ATD1STAT0_CC1 0x02
#  define ATD1STAT0_CC0 0x01
/* #define RESERVED_0x0127 0x0127 */
#define ATD1TEST0 0x0128
#define ATD1TEST1 0x0129
#  define ATD1TEST1_SC 0x01
/* #define RESERVED_0x012a 0x012a */
#define ATD1STAT1 0x012b
#  define ATD1STAT1_CCF7 0x80
#  define ATD1STAT1_CCF6 0x40
#  define ATD1STAT1_CCF5 0x20
#  define ATD1STAT1_CCF4 0x10
#  define ATD1STAT1_CCF3 0x08
#  define ATD1STAT1_CCF2 0x04
#  define ATD1STAT1_CCF1 0x02
#  define ATD1STAT1_CCF0 0x01
/* #define RESERVED_0x012c 0x012c */
#define ATD1DIEN 0x012d
/* #define RESERVED_0x012e 0x012e */
#define PORTAD1 0x012f
#  define PORTAD1_BIT7 0x80
#  define PORTAD1_BIT6 0x40
#  define PORTAD1_BIT5 0x20
#  define PORTAD1_BIT4 0x10
#  define PORTAD1_BIT3 0x08
#  define PORTAD1_BIT2 0x04
#  define PORTAD1_BIT1 0x02
#  define PORTAD1_BIT0 0x01
#define ATD1DR0H 0x0130
#define ATD1DR0L 0x0131
#define ATD1DR1H 0x0132
#define ATD1DR1L 0x0133
#define ATD1DR2H 0x0134
#define ATD1DR2L 0x0135
#define ATD1DR3H 0x0136
#define ATD1DR3L 0x0137
#define ATD1DR4H 0x0138
#define ATD1DR4L 0x0139
#define ATD1DR5H 0x013a
#define ATD1DR5L 0x013b
#define ATD1DR6H 0x013c
#define ATD1DR6L 0x013d
#define ATD1DR7H 0x013e
#define ATD1DR7L 0x013f

/* CAN0 */

#define CAN0CTL0 0x0140
#define CAN0CTL1 0x0141
#define CAN0BTR0 0x0142
#define CAN0BTR1 0x0143
#define CAN0RFLG 0x0144
#define CAN0RIER 0x0145
#define CAN0TFLG 0x0146
#define CAN0TIER 0x0147
#define CAN0TARQ 0x0148
#define CAN0TAAK 0x0149
#define CAN0TBSEL 0x014a
#define CAN0IDAC 0x014b
/* #define  0x014c */
/* #define  0x014d */
#define CAN0RXERR 0x014e
#define CAN0TXERR 0x014f
#define CAN0IDAR0 0x0150
#define CAN0IDAR1 0x0151
#define CAN0IDAR2 0x0152
#define CAN0IDAR3 0x0153
#define CAN0IDMR0 0x0154
#define CAN0IDMR1 0x0155
#define CAN0IDMR2 0x0156
#define CAN0IDMR3 0x0157
#define CAN0IDAR4 0x0158
#define CAN0IDAR5 0x0159
#define CAN0IDAR6 0x015a
#define CAN0IDAR7 0x015b
#define CAN0IDMR4 0x015c
#define CAN0IDMR5 0x015d
#define CAN0IDMR6 0x015e
#define CAN0IDMR7 0x015f
#define CAN0RXFG 0x0160 /* 0x0160..0x016f */
#define CAN0TXFG 0x0170 /* 0x0170..0x017f */

/* reserved: 0x0180..0x23f */

/* PIM */

#define PTT 0x0240
#  define PTT_PTT7 0x80
#  define PTT_PTT6 0x40
#  define PTT_PTT5 0x20
#  define PTT_PTT4 0x10
#  define PTT_PTT3 0x08
#  define PTT_PTT2 0x04
#  define PTT_PTT1 0x02
#  define PTT_PTT0 0x01
#define PTIT 0x0241
#  define PTIT_PTIT7 0x80
#  define PTIT_PTIT6 0x40
#  define PTIT_PTIT5 0x20
#  define PTIT_PTIT4 0x10
#  define PTIT_PTIT3 0x08
#  define PTIT_PTIT2 0x04
#  define PTIT_PTIT1 0x02
#  define PTIT_PTIT0 0x01
#define DDRT 0x0242
#  define DDRT_DDRT7 0x80
#  define DDRT_DDRT6 0x40
#  define DDRT_DDRT5 0x20
#  define DDRT_DDRT4 0x10
#  define DDRT_DDRT3 0x08
#  define DDRT_DDRT2 0x04
#  define DDRT_DDRT1 0x02
#  define DDRT_DDRT0 0x01
#define RDRT 0x0243
#  define RDRT_RDRT7 0x80
#  define RDRT_RDRT6 0x40
#  define RDRT_RDRT5 0x20
#  define RDRT_RDRT4 0x10
#  define RDRT_RDRT3 0x08
#  define RDRT_RDRT2 0x04
#  define RDRT_RDRT1 0x02
#  define RDRT_RDRT0 0x01
#define PERT 0x0244
#  define PERT_PERT7 0x80
#  define PERT_PERT6 0x40
#  define PERT_PERT5 0x20
#  define PERT_PERT4 0x10
#  define PERT_PERT3 0x08
#  define PERT_PERT2 0x04
#  define PERT_PERT1 0x02
#  define PERT_PERT0 0x01
#define PPST 0x0245
#  define PPST_PPST7 0x80
#  define PPST_PPST6 0x40
#  define PPST_PPST5 0x20
#  define PPST_PPST4 0x10
#  define PPST_PPST3 0x08
#  define PPST_PPST2 0x04
#  define PPST_PPST1 0x02
#  define PPST_PPST0 0x01
/* #define RESERVED_0x0246 0x0246 */
/* #define RESERVED_0x0247 0x0247 */
#define PTS 0x0248
#  define PTS_PTS7 0x80
#  define PTS_PTS6 0x40
#  define PTS_PTS5 0x20
#  define PTS_PTS4 0x10
#  define PTS_PTS3 0x08
#  define PTS_PTS2 0x04
#  define PTS_PTS1 0x02
#  define PTS_PTS0 0x01
#define PTIS 0x0249
#  define PTIS_PTIS7 0x80
#  define PTIS_PTIS6 0x40
#  define PTIS_PTIS5 0x20
#  define PTIS_PTIS4 0x10
#  define PTIS_PTIS3 0x08
#  define PTIS_PTIS2 0x04
#  define PTIS_PTIS1 0x02
#  define PTIS_PTIS0 0x01
#define DDRS 0x024a
#  define DDRS_DDRS7 0x80
#  define DDRS_DDRS6 0x40
#  define DDRS_DDRS5 0x20
#  define DDRS_DDRS4 0x10
#  define DDRS_DDRS3 0x08
#  define DDRS_DDRS2 0x04
#  define DDRS_DDRS1 0x02
#  define DDRS_DDRS0 0x01
#define RDRS 0x024b
#  define RDRS_RDRS7 0x80
#  define RDRS_RDRS6 0x40
#  define RDRS_RDRS5 0x20
#  define RDRS_RDRS4 0x10
#  define RDRS_RDRS3 0x08
#  define RDRS_RDRS2 0x04
#  define RDRS_RDRS1 0x02
#  define RDRS_RDRS0 0x01
#define PERS 0x024c
#  define PERS_PERS7 0x80
#  define PERS_PERS6 0x40
#  define PERS_PERS5 0x20
#  define PERS_PERS4 0x10
#  define PERS_PERS3 0x08
#  define PERS_PERS2 0x04
#  define PERS_PERS1 0x02
#  define PERS_PERS0 0x01
#define PPSS 0x024d
#  define PPSS_PPSS7 0x80
#  define PPSS_PPSS6 0x40
#  define PPSS_PPSS5 0x20
#  define PPSS_PPSS4 0x10
#  define PPSS_PPSS3 0x08
#  define PPSS_PPSS2 0x04
#  define PPSS_PPSS1 0x02
#  define PPSS_PPSS0 0x01
#define WOMS 0x024e
#  define WOMS_WOMS7 0x80
#  define WOMS_WOMS6 0x40
#  define WOMS_WOMS5 0x20
#  define WOMS_WOMS4 0x10
#  define WOMS_WOMS3 0x08
#  define WOMS_WOMS2 0x04
#  define WOMS_WOMS1 0x02
#  define WOMS_WOMS0 0x01
/* #define RESERVED_0x024f 0x024f */
#define PTM 0x0250
#  define PTM_PTM7 0x80
#  define PTM_PTM6 0x40
#  define PTM_PTM5 0x20
#  define PTM_PTM4 0x10
#  define PTM_PTM3 0x08
#  define PTM_PTM2 0x04
#  define PTM_PTM1 0x02
#  define PTM_PTM0 0x01
#define PTIM 0x0251
#  define PTIM_PTIM7 0x80
#  define PTIM_PTIM6 0x40
#  define PTIM_PTIM5 0x20
#  define PTIM_PTIM4 0x10
#  define PTIM_PTIM3 0x08
#  define PTIM_PTIM2 0x04
#  define PTIM_PTIM1 0x02
#  define PTIM_PTIM0 0x01
#define DDRM 0x0252
#  define DDRM_DDRM7 0x80
#  define DDRM_DDRM6 0x40
#  define DDRM_DDRM5 0x20
#  define DDRM_DDRM4 0x10
#  define DDRM_DDRM3 0x08
#  define DDRM_DDRM2 0x04
#  define DDRM_DDRM1 0x02
#  define DDRM_DDRM0 0x01
#define RDRM 0x0253
#  define RDRM_RDRM7 0x80
#  define RDRM_RDRM6 0x40
#  define RDRM_RDRM5 0x20
#  define RDRM_RDRM4 0x10
#  define RDRM_RDRM3 0x08
#  define RDRM_RDRM2 0x04
#  define RDRM_RDRM1 0x02
#  define RDRM_RDRM0 0x01
#define PERM 0x0254
#  define PERM_PERM7 0x80
#  define PERM_PERM6 0x40
#  define PERM_PERM5 0x20
#  define PERM_PERM4 0x10
#  define PERM_PERM3 0x08
#  define PERM_PERM2 0x04
#  define PERM_PERM1 0x02
#  define PERM_PERM0 0x01
#define PPSM 0x0255
#  define PPSM_PPSM7 0x80
#  define PPSM_PPSM6 0x40
#  define PPSM_PPSM5 0x20
#  define PPSM_PPSM4 0x10
#  define PPSM_PPSM3 0x08
#  define PPSM_PPSM2 0x04
#  define PPSM_PPSM1 0x02
#  define PPSM_PPSM0 0x01
#define WOMM 0x0256
#  define WOMM_WOMM7 0x80
#  define WOMM_WOMM6 0x40
#  define WOMM_WOMM5 0x20
#  define WOMM_WOMM4 0x10
#  define WOMM_WOMM3 0x08
#  define WOMM_WOMM2 0x04
#  define WOMM_WOMM1 0x02
#  define WOMM_WOMM0 0x01
#define MODRR 0x0257
#  define MODRR_MODRR7 0x80
#  define MODRR_MODRR6 0x40
#  define MODRR_MODRR5 0x20
#  define MODRR_MODRR4 0x10
#  define MODRR_MODRR3 0x08
#  define MODRR_MODRR2 0x04
#  define MODRR_MODRR1 0x02
#  define MODRR_MODRR0 0x01
#define PTP 0x0258
#  define PTP_PTP7 0x80
#  define PTP_PTP6 0x40
#  define PTP_PTP5 0x20
#  define PTP_PTP4 0x10
#  define PTP_PTP3 0x08
#  define PTP_PTP2 0x04
#  define PTP_PTP1 0x02
#  define PTP_PTP0 0x01
#define PTIP 0x0259
#  define PTIP_PTIP7 0x80
#  define PTIP_PTIP6 0x40
#  define PTIP_PTIP5 0x20
#  define PTIP_PTIP4 0x10
#  define PTIP_PTIP3 0x08
#  define PTIP_PTIP2 0x04
#  define PTIP_PTIP1 0x02
#  define PTIP_PTIP0 0x01
#define DDRP 0x025a
#  define DDRP_DDRP7 0x80
#  define DDRP_DDRP6 0x40
#  define DDRP_DDRP5 0x20
#  define DDRP_DDRP4 0x10
#  define DDRP_DDRP3 0x08
#  define DDRP_DDRP2 0x04
#  define DDRP_DDRP1 0x02
#  define DDRP_DDRP0 0x01
#define RDRP 0x025b
#  define RDRP_RDRP7 0x80
#  define RDRP_RDRP6 0x40
#  define RDRP_RDRP5 0x20
#  define RDRP_RDRP4 0x10
#  define RDRP_RDRP3 0x08
#  define RDRP_RDRP2 0x04
#  define RDRP_RDRP1 0x02
#  define RDRP_RDRP0 0x01
#define PERP 0x025c
#  define PERP_PERP7 0x80
#  define PERP_PERP6 0x40
#  define PERP_PERP5 0x20
#  define PERP_PERP4 0x10
#  define PERP_PERP3 0x08
#  define PERP_PERP2 0x04
#  define PERP_PERP1 0x02
#  define PERP_PERP0 0x01
#define PPSP 0x025d
#  define PPSP_PPSP7 0x80
#  define PPSP_PPSP6 0x40
#  define PPSP_PPSP5 0x20
#  define PPSP_PPSP4 0x10
#  define PPSP_PPSP3 0x08
#  define PPSP_PPSP2 0x04
#  define PPSP_PPSP1 0x02
#  define PPSP_PPSP0 0x01
#define PIEP 0x025e
#  define PIEP_PIEP7 0x80
#  define PIEP_PIEP6 0x40
#  define PIEP_PIEP5 0x20
#  define PIEP_PIEP4 0x10
#  define PIEP_PIEP3 0x08
#  define PIEP_PIEP2 0x04
#  define PIEP_PIEP1 0x02
#  define PIEP_PIEP0 0x01
#define PIFP 0x025f
#  define PIFP_PIFP7 0x80
#  define PIFP_PIFP6 0x40
#  define PIFP_PIFP5 0x20
#  define PIFP_PIFP4 0x10
#  define PIFP_PIFP3 0x08
#  define PIFP_PIFP2 0x04
#  define PIFP_PIFP1 0x02
#  define PIFP_PIFP0 0x01
#define PTH 0x0260
#  define PTH_PTH7 0x80
#  define PTH_PTH6 0x40
#  define PTH_PTH5 0x20
#  define PTH_PTH4 0x10
#  define PTH_PTH3 0x08
#  define PTH_PTH2 0x04
#  define PTH_PTH1 0x02
#  define PTH_PTH0 0x01
#define PTIH 0x0261
#  define PTIH_PTIH7 0x80
#  define PTIH_PTIH6 0x40
#  define PTIH_PTIH5 0x20
#  define PTIH_PTIH4 0x10
#  define PTIH_PTIH3 0x08
#  define PTIH_PTIH2 0x04
#  define PTIH_PTIH1 0x02
#  define PTIH_PTIH0 0x01
#define DDRH 0x0262
#  define DDRH_DDRH7 0x80
#  define DDRH_DDRH6 0x40
#  define DDRH_DDRH5 0x20
#  define DDRH_DDRH4 0x10
#  define DDRH_DDRH3 0x08
#  define DDRH_DDRH2 0x04
#  define DDRH_DDRH1 0x02
#  define DDRH_DDRH0 0x01
#define RDRH 0x0263
#  define RDRH_RDRH7 0x80
#  define RDRH_RDRH6 0x40
#  define RDRH_RDRH5 0x20
#  define RDRH_RDRH4 0x10
#  define RDRH_RDRH3 0x08
#  define RDRH_RDRH2 0x04
#  define RDRH_RDRH1 0x02
#  define RDRH_RDRH0 0x01
#define PERH 0x0264
#  define PERH_PERH7 0x80
#  define PERH_PERH6 0x40
#  define PERH_PERH5 0x20
#  define PERH_PERH4 0x10
#  define PERH_PERH3 0x08
#  define PERH_PERH2 0x04
#  define PERH_PERH1 0x02
#  define PERH_PERH0 0x01
#define PPSH 0x0265
#  define PPSH_PPSH7 0x80
#  define PPSH_PPSH6 0x40
#  define PPSH_PPSH5 0x20
#  define PPSH_PPSH4 0x10
#  define PPSH_PPSH3 0x08
#  define PPSH_PPSH2 0x04
#  define PPSH_PPSH1 0x02
#  define PPSH_PPSH0 0x01
#define PIEH 0x0266
#  define PIEH_PIEH7 0x80
#  define PIEH_PIEH6 0x40
#  define PIEH_PIEH5 0x20
#  define PIEH_PIEH4 0x10
#  define PIEH_PIEH3 0x08
#  define PIEH_PIEH2 0x04
#  define PIEH_PIEH1 0x02
#  define PIEH_PIEH0 0x01
#define PIFH 0x0267
#  define PIFH_PIFH7 0x80
#  define PIFH_PIFH6 0x40
#  define PIFH_PIFH5 0x20
#  define PIFH_PIFH4 0x10
#  define PIFH_PIFH3 0x08
#  define PIFH_PIFH2 0x04
#  define PIFH_PIFH1 0x02
#  define PIFH_PIFH0 0x01
#define PTJ 0x0268
#  define PTJ_PTJ7 0x80
#  define PTJ_PTJ6 0x40
#  define PTJ_PTJ5 0x20
#  define PTJ_PTJ4 0x10
#  define PTJ_PTJ3 0x08
#  define PTJ_PTJ2 0x04
#  define PTJ_PTJ1 0x02
#  define PTJ_PTJ0 0x01
#define PTIJ 0x0269
#  define PTIJ_PTIJ7 0x80
#  define PTIJ_PTIJ6 0x40
#  define PTIJ_PTIJ5 0x20
#  define PTIJ_PTIJ4 0x10
#  define PTIJ_PTIJ3 0x08
#  define PTIJ_PTIJ2 0x04
#  define PTIJ_PTIJ1 0x02
#  define PTIJ_PTIJ0 0x01
#define DDRJ 0x026a
#  define DDRJ_DDRJ7 0x80
#  define DDRJ_DDRJ6 0x40
#  define DDRJ_DDRJ5 0x20
#  define DDRJ_DDRJ4 0x10
#  define DDRJ_DDRJ3 0x08
#  define DDRJ_DDRJ2 0x04
#  define DDRJ_DDRJ1 0x02
#  define DDRJ_DDRJ0 0x01
#define RDRJ 0x026b
#  define RDRJ_RDRJ7 0x80
#  define RDRJ_RDRJ6 0x40
#  define RDRJ_RDRJ5 0x20
#  define RDRJ_RDRJ4 0x10
#  define RDRJ_RDRJ3 0x08
#  define RDRJ_RDRJ2 0x04
#  define RDRJ_RDRJ1 0x02
#  define RDRJ_RDRJ0 0x01
#define PERJ 0x026c
#  define PERJ_PERJ7 0x80
#  define PERJ_PERJ6 0x40
#  define PERJ_PERJ5 0x20
#  define PERJ_PERJ4 0x10
#  define PERJ_PERJ3 0x08
#  define PERJ_PERJ2 0x04
#  define PERJ_PERJ1 0x02
#  define PERJ_PERJ0 0x01
#define PPSJ 0x026d
#  define PPSJ_PPSJ7 0x80
#  define PPSJ_PPSJ6 0x40
#  define PPSJ_PPSJ5 0x20
#  define PPSJ_PPSJ4 0x10
#  define PPSJ_PPSJ3 0x08
#  define PPSJ_PPSJ2 0x04
#  define PPSJ_PPSJ1 0x02
#  define PPSJ_PPSJ0 0x01
#define PIEJ 0x026e
#  define PIEJ_PIEJ7 0x80
#  define PIEJ_PIEJ6 0x40
#  define PIEJ_PIEJ5 0x20
#  define PIEJ_PIEJ4 0x10
#  define PIEJ_PIEJ3 0x08
#  define PIEJ_PIEJ2 0x04
#  define PIEJ_PIEJ1 0x02
#  define PIEJ_PIEJ0 0x01
#define PIFJ 0x026f
#  define PIFJ_PIFJ7 0x80
#  define PIFJ_PIFJ6 0x40
#  define PIFJ_PIFJ5 0x20
#  define PIFJ_PIFJ4 0x10
#  define PIFJ_PIFJ3 0x08
#  define PIFJ_PIFJ2 0x04
#  define PIFJ_PIFJ1 0x02
#  define PIFJ_PIFJ0 0x01

/* reserved: 0x0270..0x027f */

/* reserved: 0x0280..0x03ff */

#endif /* __IO_MC9S12A64_H */
