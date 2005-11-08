/*
   hc12mem - HC12 memory reader & writer
   io_mc9s12ne64.h: MC9S12NE64 I/O registers
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

#ifndef __IO_MC9S12NE64_H
#define __IO_MC9S12NE64_H

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
   /* #  define PORTE_ 0x02 */
#  define PORTE_BIT0 0x01
#define DDRE  0x0009
#  define DDRE_BIT7 0x80
#  define DDRE_BIT6 0x40
#  define DDRE_BIT5 0x20
#  define DDRE_BIT4 0x10
#  define DDRE_BIT3 0x08
#  define DDRE_BIT2 0x04
   /* #  define DDRE_ 0x02 */
   /* #  define DDRE_ 0x01 */
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
#  define ITCR_WRTINT 0x10
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

/* MMC map 2 of 4 */

#define MTST1 0x0017

/* reserved 0x0018..0x0019 */

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

#define IRQCR 0x001e
#  define IRQCR_IRQE 0x80
#  define IRQCR_IRQEN 0x40
   /* #  define IRQCR_ 0x20 */
   /* #  define IRQCR_ 0x10 */
   /* #  define IRQCR_ 0x08 */
   /* #  define IRQCR_ 0x04 */
   /* #  define IRQCR_ 0x02 */
   /* #  define IRQCR_ 0x01 */

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

/* DBG map 1 of 1 */

#define DBGC1 0x0020
#  define DBGC1_DBGEN 0x80
#  define DBGC1_ARM 0x40
#  define DBGC1_TRGSEL 0x20
#  define DBGC1_BEGIN 0x10
#  define DBGC1_DBGBRK 0x08
   /* #  define DBGC1_ 0x04 */
#  define DBGC1_CAPMOD1 0x02 /* ??? CAPMOD */
#  define DBGC1_CAPMOD0 0x01 /* ??? CAPMOD */
#define DBGSC 0x0021
#  define DBGSC_AF 0x80
#  define DBGSC_BF 0x40
#  define DBGSC_CF 0x20
   /* #  define DBGSC_ 0x10 */
#  define DBGSC_TRG3 0x08 /* ??? TRG */
#  define DBGSC_TRG2 0x04 /* ??? TRG */
#  define DBGSC_TRG1 0x02 /* ??? TRG */
#  define DBGSC_TRG0 0x01 /* ??? TRG */
#define DBGTBH 0x0022
#define DBGTBL 0x0023
#define DBGCNT 0x0024
#  define DBGCNT_TBF 0x80
   /*#  define DBGCNT_ 0x40 */
#  define DBGCNT_CNT5 0x20 /* ??? CNT */
#  define DBGCNT_CNT4 0x10 /* ??? CNT */
#  define DBGCNT_CNT3 0x08 /* ??? CNT */
#  define DBGCNT_CNT2 0x04 /* ??? CNT */
#  define DBGCNT_CNT1 0x02 /* ??? CNT */
#  define DBGCNT_CNT0 0x01 /* ??? CNT */
#define DBGCCX 0x0025
#define DBGCCH 0x0026
#define DBGCCL 0x0027
#define DBGC2 0x0028
#  define DBGC2_BKABEN 0x80
#  define DBGC2_FULL 0x40
#  define DBGC2_BDM 0x20
#  define DBGC2_TAGAB 0x10
#  define DBGC2_BKCEN 0x08
#  define DBGC2_TAGC 0x04
#  define DBGC2_RWCEN 0x02
#  define DBGC2_RWC 0x01
#define DBGC3 0x0029
#  define DBGC3_BKAMBH 0x80
#  define DBGC3_BKAMBL 0x40
#  define DBGC3_BKBMBH 0x20
#  define DBGC3_BKBMBL 0x10
#  define DBGC3_RWAEN 0x08
#  define DBGC3_RWA 0x04
#  define DBGC3_RWBEN 0x02
#  define DBGC3_RWB 0x01
#define DBGCAX 0x002a
#define DBGCAH 0x002b
#define DBGCAL 0x002c
#define DBGCBX 0x002d
#define DBGCBH 0x002e
#define DBGCBL 0x002f

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
#  define CRGFLG_RTIF   0x80
#  define CRGFLG_PORF   0x40
#  define CRGFLG_LVRF   0x20
#  define CRGFLG_LOCKIF 0x10
#  define CRGFLG_LOCK   0x08
#  define CRGFLG_TRACK  0x04
#  define CRGFLG_SCMIF  0x02
#  define CRGFLG_SCM    0x01
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
#  define CLKSEL_PSTP   0x40
#  define CLKSEL_SYSWAI 0x20
#  define CLKSEL_ROAWAI 0x10
#  define CLKSEL_PLLWAI 0x08
#  define CLKSEL_CWAI   0x04
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

/* TIM */

#define TIOS 0x0040
#  define TIOS_IOS7 0x80
#  define TIOS_IOS6 0x40
#  define TIOS_IOS5 0x20
#  define TIOS_IOS4 0x10
   /* #  define TIOS_ 0x08 */
   /* #  define TIOS_ 0x04 */
   /* #  define TIOS_ 0x02 */
   /* #  define TIOS_ 0x01 */
#define CFORC 0x0041
#  define CFORC_FOC7 0x80
#  define CFORC_FOC6 0x40
#  define CFORC_FOC5 0x20
#  define CFORC_FOC4 0x10
   /* #  define CFORC_ 0x08 */
   /* #  define CFORC_ 0x04 */
   /* #  define CFORC_ 0x02 */
   /* #  define CFORC_ 0x01 */
#define OC7M 0x0042
#  define OC7M_OC7M7 0x80
#  define OC7M_OC7M6 0x40
#  define OC7M_OC7M5 0x20
#  define OC7M_OC7M4 0x10
   /* #  define OC7M_ 0x08 */
   /* #  define OC7M_ 0x04 */
   /* #  define OC7M_ 0x02 */
   /* #  define OC7M_ 0x01 */
#define OC7D 0x0043
#  define OC7D_OC7D7 0x80
#  define OC7D_OC7D6 0x40
#  define OC7D_OC7D5 0x20
#  define OC7D_OC7D4 0x10
   /* #  define OC7D_ 0x08 */
   /* #  define OC7D_ 0x04 */
   /* #  define OC7D_ 0x02 */
   /* #  define OC7D_ 0x01 */
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
   /* #  define TTOV_ 0x08 */
   /* #  define TTOV_ 0x04 */
   /* #  define TTOV_ 0x02 */
   /* #  define TTOV_ 0x01 */
#define TCTL1 0x0048
#  define TCTL1_OM7 0x80
#  define TCTL1_OL7 0x40
#  define TCTL1_OM6 0x20
#  define TCTL1_OL6 0x10
#  define TCTL1_OM5 0x08
#  define TCTL1_OL5 0x04
#  define TCTL1_OM4 0x02
#  define TCTL1_OL4 0x01
/* #define RESERVED_0x0049 0x0049 */
#define TCTL3 0x004a
#  define TCTL3_EDG7B 0x80
#  define TCTL3_EDG7A 0x40
#  define TCTL3_EDG6B 0x20
#  define TCTL3_EDG6A 0x10
#  define TCTL3_EDG5B 0x08
#  define TCTL3_EDG5A 0x04
#  define TCTL3_EDG4B 0x02
#  define TCTL3_EDG4A 0x01
/* #define RESERVED_0x004b 0x004b */
#define TIE 0x004c
#  define TIE_C7I 0x80
#  define TIE_C6I 0x40
#  define TIE_C5I 0x20
#  define TIE_C4I 0x10
   /* #  define TIE_ 0x08 */
   /* #  define TIE_ 0x04 */
   /* #  define TIE_ 0x02 */
   /* #  define TIE_ 0x01 */
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
   /* #  define TFLG1_ 0x08 */
   /* #  define TFLG1_ 0x04 */
   /* #  define TFLG1_ 0x02 */
   /* #  define TFLG1_ 0x01 */
#define TFLG2 0x004f
#  define TFLG2_TOF 0x80
   /* #  define TFLG2_ 0x40 */
   /* #  define TFLG2_ 0x20 */
   /* #  define TFLG2_ 0x10 */
   /* #  define TFLG2_ 0x08 */
   /* #  define TFLG2_ 0x04 */
   /* #  define TFLG2_ 0x02 */
   /* #  define TFLG2_ 0x01 */
/* #define RESERVED_0x0050 0x0050 */
/* #define RESERVED_0x0051 0x0051 */
/* #define RESERVED_0x0052 0x0052 */
/* #define RESERVED_0x0053 0x0053 */
/* #define RESERVED_0x0054 0x0054 */
/* #define RESERVED_0x0055 0x0055 */
/* #define RESERVED_0x0056 0x0056 */
/* #define RESERVED_0x0057 0x0057 */
#define TC4 0x0058
#define TC4HI 0x0058
#define TC4LO 0x0059
#define TC5 0x005a
#define TC5HI 0x005a
#define TC5LO 0x005b
#define TC6 0x005c
#define TC6HI 0x005c
#define TC6LO 0x005d
#define TC7 0x005e
#define TC7HI 0x005e
#define TC7LO 0x005f
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
#define PACNT 0x0062
#define PACNTHI 0x0062
#define PACNTLO 0x0063

/* reserved 0x0064..0x006f */
/* reserved 0x0070..0x007f */

/* ATD0 */

#define ATDCTL0 0x0080
   /* #  define ATDCTL0_ 0x80 */
   /* #  define ATDCTL0_ 0x40 */
   /* #  define ATDCTL0_ 0x20 */
   /* #  define ATDCTL0_ 0x10 */
   /* #  define ATDCTL0_ 0x08 */
#  define ATDCTL0_WRAP2 0x04
#  define ATDCTL0_WRAP1 0x02
#  define ATDCTL0_WRAP0 0x01
#define ATDCTL1 0x0081
#  define ATDCTL1_ETRIG_SEL 0x80
   /* #  define ATDCTL1_ 0x40 */
   /* #  define ATDCTL1_ 0x20 */
   /* #  define ATDCTL1_ 0x10 */
   /* #  define ATDCTL1_ 0x08 */
#  define ATDCTL1_ETRIG_CH2 0x04
#  define ATDCTL1_ETRIG_CH1 0x02
#  define ATDCTL1_ETRIG_CH0 0x01
#define ATDCTL2 0x0082
#  define ATDCTL2_ADPU 0x80
#  define ATDCTL2_AFFC 0x40
#  define ATDCTL2_AWAI 0x20
#  define ATDCTL2_ETRIGLE 0x10
#  define ATDCTL2_ETRIGP 0x08
#  define ATDCTL2_ETRIGE 0x04
#  define ATDCTL2_ASCIE 0x02
#  define ATDCTL2_ASCIF 0x01
#define ATDCTL3 0x0083
   /* #  define ATDCTL3_ 0x80 */
#  define ATDCTL3_S8C 0x40
#  define ATDCTL3_S4C 0x20
#  define ATDCTL3_S2C 0x10
#  define ATDCTL3_S1C 0x08
#  define ATDCTL3_FIFO 0x04
#  define ATDCTL3_FRZ1 0x02
#  define ATDCTL3_FRZ0 0x01
#define ATDCTL4 0x0084
#  define ATDCTL4_SRES8 0x80
#  define ATDCTL4_SMP1 0x40
#  define ATDCTL4_SMP0 0x20
#  define ATDCTL4_PRS4 0x10
#  define ATDCTL4_PRS3 0x08
#  define ATDCTL4_PRS2 0x04
#  define ATDCTL4_PRS1 0x02
#  define ATDCTL4_PRS0 0x01
#define ATDCTL5 0x0085
#  define ATDCTL5_DJM 0x80
#  define ATDCTL5_DSGN 0x40
#  define ATDCTL5_SCAN 0x20
#  define ATDCTL5_MULT 0x10
   /* #  define ATDCTL5_ 0x08 */
#  define ATDCTL5_CC 0x04
#  define ATDCTL5_CB 0x02
#  define ATDCTL5_CA 0x01
#define ATDSTAT0 0x0086
#  define ATDSTAT0_SCF 0x80
   /* #  define ATDSTAT0_ 0x40 */
#  define ATDSTAT0_ETORF 0x20
#  define ATDSTAT0_FIFOR 0x10
   /* #  define ATDSTAT0_ 0x08 */
#  define ATDSTAT0_CC2 0x04
#  define ATDSTAT0_CC1 0x02
#  define ATDSTAT0_CC0 0x01
/* #define RESERVED_0x0087 0x0087 */
#define ATDTEST0 0x0088
#define ATDTEST1 0x0089
#  define ATDTEST1_SC 0x01
/* #define RESERVED_0x008a 0x008a */
#define ATDSTAT1 0x008b
#  define ATDSTAT1_CCF7 0x80
#  define ATDSTAT1_CCF6 0x40
#  define ATDSTAT1_CCF5 0x20
#  define ATDSTAT1_CCF4 0x10
#  define ATDSTAT1_CCF3 0x08
#  define ATDSTAT1_CCF2 0x04
#  define ATDSTAT1_CCF1 0x02
#  define ATDSTAT1_CCF0 0x01
/* #define RESERVED_0x008c 0x008c */
#define ATDDIEN 0x008d
#  define ATDDIEN_IEN7 0x80
#  define ATDDIEN_IEN6 0x40
#  define ATDDIEN_IEN5 0x20
#  define ATDDIEN_IEN4 0x10
#  define ATDDIEN_IEN3 0x08
#  define ATDDIEN_IEN2 0x04
#  define ATDDIEN_IEN1 0x02
#  define ATDDIEN_IEN0 0x01
/* #define RESERVED_0x008e 0x008e */
#define PORTAD0 0x008f
#  define PORTAD0_PTAD7 0x80
#  define PORTAD0_PTAD6 0x40
#  define PORTAD0_PTAD5 0x20
#  define PORTAD0_PTAD4 0x10
#  define PORTAD0_PTAD3 0x08
#  define PORTAD0_PTAD2 0x04
#  define PORTAD0_PTAD1 0x02
#  define PORTAD0_PTAD0 0x01
#define ATDDR0H 0x0090
#define ATDDR0L 0x0091
#define ATDDR1H 0x0092
#define ATDDR1L 0x0093
#define ATDDR2H 0x0094
#define ATDDR2L 0x0095
#define ATDDR3H 0x0096
#define ATDDR3L 0x0097
#define ATDDR4H 0x0098
#define ATDDR4L 0x0099
#define ATDDR5H 0x009a
#define ATDDR5L 0x009b
#define ATDDR6H 0x009c
#define ATDDR6L 0x009d
#define ATDDR7H 0x009e
#define ATDDR7L 0x009f

/* reserved 0x00a0..0x00c7 */

/* SCI0 */

#define SCI0BD  0x00c8
#define SCI0BDH 0x00c8
#  define SCI0BDH_IREN 0x80
#  define SCI0BDH_TNP1 0x40
#  define SCI0BDH_TNP0 0x20
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
#  define SCI1BDH_IREN 0x80
#  define SCI1BDH_TNP1 0x40
#  define SCI1BDH_TNP0 0x20
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

/* SPI */

#define SPICR1 0x00d8
#  define SPICR1_SPIE 0x80
#  define SPICR1_SPE 0x40
#  define SPICR1_SPTIE 0x20
#  define SPICR1_MSTR 0x10
#  define SPICR1_CPOL 0x08
#  define SPICR1_CPHA 0x04
#  define SPICR1_SSOE 0x02
#  define SPICR1_LSBFE 0x01
#define SPICR2 0x00d9
   /* #  define SPICR2_ 0x80 */
   /* #  define SPICR2_ 0x40 */
   /* #  define SPICR2_ 0x20 */
#  define SPICR2_MODFEN 0x10
#  define SPICR2_BIDIROE 0x08
   /* #  define SPICR2_ 0x04 */
#  define SPICR2_SPISWAI 0x02
#  define SPICR2_SPC0 0x01
#define SPIBR 0x00da
   /* #  define SPIBR_ 0x80 */
#  define SPIBR_SPPR2 0x40
#  define SPIBR_SPPR1 0x20
#  define SPIBR_SPPR0 0x10
   /* #  define SPIBR_ 0x08 */
#  define SPIBR_SPR2 0x04
#  define SPIBR_SPR1 0x02
#  define SPIBR_SPR0 0x01
#define SPISR 0x00db
#  define SPISR_SPIF 0x80
   /* #  define SPISR_ 0x40 */
#  define SPISR_SPTEF 0x20
#  define SPISR_MODF 0x10
   /* #  define SPISR_ 0x08 */
   /* #  define SPISR_ 0x04 */
   /* #  define SPISR_ 0x02 */
   /* #  define SPISR_ 0x01 */
/* #define RESERVED_0x00dc 0x00dc */
#define SPIDR 0x00dd
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

/* reserved 0x00e8..0x00ff */

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

/* reserved 0x0110..0x011f */

/* EPHY */

#define EPHYCTL0 0x0120
#  define EPHYCTL0_EPHYEN 0x80
#  define EPHYCTL0_ANDIS 0x40
#  define EPHYCTL0_DIS100 0x20
#  define EPHYCTL0_DIS10 0x10
#  define EPHYCTL0_LEDEN 0x08
#  define EPHYCTL0_EPHYWAI 0x04
   /* #  define EPHYCTL0_ 0x02 */
#  define EPHYCTL0_EPHYIEN 0x01
#define EPHYCTL1 0x0121
   /* #  define EPHYCTL1_ 0x80 */
   /* #  define EPHYCTL1_ 0x40 */
   /* #  define EPHYCTL1_ 0x20 */
#  define EPHYCTL1_PHYADD4 0x10
#  define EPHYCTL1_PHYADD3 0x08
#  define EPHYCTL1_PHYADD2 0x04
#  define EPHYCTL1_PHYADD1 0x02
#  define EPHYCTL1_PHYADD0 0x01
#define EPHYSR   0x0122
   /* #  define EPHYSR_ 0x80 */
   /* #  define EPHYSR_ 0x40 */
#  define EPHYSR_100DIS 0x20
#  define EPHYSR_10DIS 0x10
   /* #  define EPHYSR_ 0x08 */
   /* #  define EPHYSR_ 0x04 */
   /* #  define EPHYSR_ 0x02 */
#  define EPHYSR_EPHYIF 0x01
#define EPHYTST  0x0123

/* reserved 0x0124..0x013f */

/* EMAC */

#define NETCTL 0x0140
#  define NETCTL_EMACE 0x80
   /* #  define NETCTL_ 0x40 */
   /* #  define NETCTL_ 0x20 */
#  define NETCTL_ESWAI 0x10
#  define NETCTL_EXTPHY 0x08
#  define NETCTL_MLB 0x04
#  define NETCTL_FDX 0x02
   /* #  define NETCTL_ 0x01 */
/* #define RESERVED_0x0141 0x0141 */
/* #define RESERVED_0x0142 0x0142 */
#define RXCTS 0x0143
#  define RXCTS_RXACT 0x80
   /* #  define RXCTS_ 0x40 */
   /* #  define RXCTS_ 0x20 */
#  define RXCTS_RFCE 0x10
   /* #  define RXCTS_ 0x08 */
#  define RXCTS_PROM 0x04
#  define RXCTS_CONMC 0x02
#  define RXCTS_BCREJ 0x01
#define TXCTS 0x0144
#  define TXCTS_TXACT 0x80
   /* #  define TXCTS_ 0x40 */
#  define TXCTS_CSLF 0x20
#  define TXCTS_PTRC 0x10
#  define TXCTS_SSB 0x08
   /* #  define TXCTS_ 0x04 */
#  define TXCTS_TCMD1 0x02
#  define TXCTS_TCMD0 0x01
#  define TXCTS_TCMD_MASK 0x03
#define ETCTL 0x0145
#  define ETCTL_FPET 0x80
   /* #  define ETCTL_ 0x40 */
   /* #  define ETCTL_ 0x20 */
#  define ETCTL_FEMW 0x10
#  define ETCTL_FIPV6 0x08
#  define ETCTL_FARP 0x04
#  define ETCTL_FIPV4 0x02
#  define ETCTL_FIEEE 0x01
#define ETYPE 0x0146
#define ETYPEHI 0x0146
#define ETYPELO 0x0147
#define PTIME 0x0148
#define PTIMEHI 0x0148
#define PTIMELO 0x0149
#define IEVENT 0x014a
#  define IEVENT_RFCIF 0x8000
   /* #  define IEVENT_ 0x4000 */
#  define IEVENT_BREIF 0x2000
#  define IEVENT_RXEIF 0x1000
#  define IEVENT_RXAOIF 0x0800
#  define IEVENT_RXBOIF 0x0400
#  define IEVENT_RXACIF 0x0200
#  define IEVENT_RXBCIF 0x0100
#  define IEVENT_MMCIF 0x0080
   /* #  define IEVENT_ 0x0040 */
#  define IEVENT_LCIF 0x0020
#  define IEVENT_ECIF 0x0010
   /* #  define IEVENT_ 0x0008 */
   /* #  define IEVENT_ 0x0004 */
#  define IEVENT_TXCIF 0x0002
   /* #  define IEVENT_ 0x0001 */
#define IEVENTHI 0x014a
#  define IEVENTHI_RFCIF 0x80
   /* #  define IEVENTHI_ 0x40 */
#  define IEVENTHI_BREIF 0x20
#  define IEVENTHI_RXEIF 0x10
#  define IEVENTHI_RXAOIF 0x08
#  define IEVENTHI_RXBOIF 0x04
#  define IEVENTHI_RXACIF 0x02
#  define IEVENTHI_RXBCIF 0x01
#define IEVENTLO 0x014b
#  define IEVENTLO_MMCIF 0x80
   /* #  define IEVENTLO_ 0x40 */
#  define IEVENTLO_LCIF 0x20
#  define IEVENTLO_ECIF 0x10
   /* #  define IEVENTLO_ 0x08 */
   /* #  define IEVENTLO_ 0x04 */
#  define IEVENTLO_TXCIF 0x02
   /* #  define IEVENTLO_ 0x01 */
#define IMASK 0x014c
#  define IMASK_RFCIE 0x8000
   /* #  define IMASK_ 0x4000 */
#  define IMASK_BREIE 0x2000
#  define IMASK_RXEIE 0x1000
#  define IMASK_RXAOIE 0x0800
#  define IMASK_RXBOIE 0x0400
#  define IMASK_RXACIE 0x0200
#  define IMASK_RXBCIE 0x0100
#  define IMASK_MMCIE 0x0080
   /* #  define IMASK_ 0x0040 */
#  define IMASK_LCIE 0x0020
#  define IMASK_ECIE 0x0010
   /* #  define IMASK_ 0x0008 */
   /* #  define IMASK_ 0x0004 */
#  define IMASK_RXCIE 0x0002
   /* #  define IMASK_ 0x0001 */
#define IMASKHI 0x014c
#  define IMASKHI_RFCIE 0x80
   /* #  define IMASKHI_ 0x40 */
#  define IMASKHI_BREIE 0x20
#  define IMASKHI_RXEIE 0x10
#  define IMASKHI_RXAOIE 0x08
#  define IMASKHI_RXBOIE 0x04
#  define IMASKHI_RXACIE 0x02
#  define IMASKHI_RXBCIE 0x01
#define IMASKLO 0x014d
#  define IMASKLO_MMCIE 0x80
   /* #  define IMASKLO_ 0x40 */
#  define IMASKLO_LCIE 0x20
#  define IMASKLO_ECIE 0x10
   /* #  define IMASKLO_ 0x08 */
   /* #  define IMASKLO_ 0x04 */
#  define IMASKLO_RXCIE 0x02
   /* #  define IMASKLO_ 0x01 */
#define SWRST 0x014e
#  define SWRST_MACRST 0x80
   /* #  define SWRST_ 0x40 */
   /* #  define SWRST_ 0x20 */
   /* #  define SWRST_ 0x10 */
   /* #  define SWRST_ 0x08 */
   /* #  define SWRST_ 0x04 */
   /* #  define SWRST_ 0x02 */
   /* #  define SWRST_ 0x01 */
/* #define RESERVED_0x014f 0x014f */
#define MPADR 0x0150
#  define MPADR_PADDR_MASK 0x1f
#define MRADR 0x0151
#  define MRADR_RADDR_MASK 0x1f
#define MWDATA 0x0152
#define MWDATAHI 0x0152
#define MWDATALO 0x0153
#define MRDATA 0x0154
#define MRDATAHI 0x0154
#define MRDATALO 0x0155
#define MCMST 0x0156
#  define MCMST_OP1 0x80
#  define MCMST_OP0 0x40
#  define MCMST_BUSY 0x20
#  define MCMST_NOPRE 0x10
#  define MCMST_MDCSEL3 0x08
#  define MCMST_MDCSEL2 0x04
#  define MCMST_MDCSEL1 0x02
#  define MCMST_MDCSEL0 0x01
#  define MCMST_MDCSEL_MASK 0x0f
/* #define RESERVED_0x0157 0x0157 */
#define BUFCFG 0x0158
#  define BUFCFG_BUFMAP_MASK 0x7000
#  define BUFCFG_MAXFL_MASK  0x07ff
#define BUFCFGHI 0x0158
#  define BUFCFGHI_BUFMAP_MASK 0x70
#  define BUFCFGHI_MAXFL_MASK  0x07
#define BUFCFGLO 0x0159
#  define BUFCFGLO_MAXFL_MASK 0xff
#define RXAEFP 0x015a
#  define RXAEFP_MASK 0x07ff
#define RXAEFPHI 0x015a
#define RXAEFPLO 0x015b
#define RXBEFP 0x015c
#  define RXBEFP_MASK 0x07ff
#define RXBEFPHI 0x015c
#define RXBEFPLO 0x015d
#define TXEFP 0x015e
#  define TXEFP_MASK 0x07ff
#define TXEFPHI 0x015e
#define TXEFPLO 0x015f
#define MCHASH 0x0160 /* 0x0160..0x0167 */
#define MACAD  0x0168 /* 0x0168..0x016d */
#define EMISC 0x016e
#  define EMISC_INDEX_MASK 0xe000
#  define EMISC_MISC_MASK  0x07ff
#define EMISCHI 0x016e
#  define EMISCHI_INDEX_MASK 0xe0
#  define EMISCHI_MISC_MASK  0x07
#define EMISCLO 0x016f
#  define EMISCLO_MISC_MASK  0xff

/* reserved: 0x0170..0x23f */

/* PIM */

#define PTT 0x0240
#  define PTT_PTT7 0x80
#  define PTT_PTT6 0x40
#  define PTT_PTT5 0x20
#  define PTT_PTT4 0x10
   /* #  define PTT_ 0x08 */
   /* #  define PTT_ 0x04 */
   /* #  define PTT_ 0x02 */
   /* #  define PTT_ 0x01 */
#define PTIT 0x0241
#  define PTIT_PTIT7 0x80
#  define PTIT_PTIT6 0x40
#  define PTIT_PTIT5 0x20
#  define PTIT_PTIT4 0x10
   /* #  define PTIT_ 0x08 */
   /* #  define PTIT_ 0x04 */
   /* #  define PTIT_ 0x02 */
   /* #  define PTIT_ 0x01 */
#define DDRT 0x0242
#  define DDRT_DDRT7 0x80
#  define DDRT_DDRT6 0x40
#  define DDRT_DDRT5 0x20
#  define DDRT_DDRT4 0x10
   /* #  define DDRT_ 0x08 */
   /* #  define DDRT_ 0x04 */
   /* #  define DDRT_ 0x02 */
   /* #  define DDRT_ 0x01 */
#define RDRT 0x0243
#  define RDRT_RDRT7 0x80
#  define RDRT_RDRT6 0x40
#  define RDRT_RDRT5 0x20
#  define RDRT_RDRT4 0x10
   /* #  define RDRT_ 0x08 */
   /* #  define RDRT_ 0x04 */
   /* #  define RDRT_ 0x02 */
   /* #  define RDRT_ 0x01 */
#define PERT 0x0244
#  define PERT_PERT7 0x80
#  define PERT_PERT6 0x40
#  define PERT_PERT5 0x20
#  define PERT_PERT4 0x10
   /* #  define PERT_ 0x08 */
   /* #  define PERT_ 0x04 */
   /* #  define PERT_ 0x02 */
   /* #  define PERT_ 0x01 */
#define PPST 0x0245
#  define PPST_PPST7 0x80
#  define PPST_PPST6 0x40
#  define PPST_PPST5 0x20
#  define PPST_PPST4 0x10
   /* #  define PPST_ 0x08 */
   /* #  define PPST_ 0x04 */
   /* #  define PPST_ 0x02 */
   /* #  define PPST_ 0x01 */
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
#define PTG 0x0250
#  define PTG_PTG7 0x80
#  define PTG_PTG6 0x40
#  define PTG_PTG5 0x20
#  define PTG_PTG4 0x10
#  define PTG_PTG3 0x08
#  define PTG_PTG2 0x04
#  define PTG_PTG1 0x02
#  define PTG_PTG0 0x01
#define PTIG 0x0251
#  define PTIG_PTIG7 0x80
#  define PTIG_PTIG6 0x40
#  define PTIG_PTIG5 0x20
#  define PTIG_PTIG4 0x10
#  define PTIG_PTIG3 0x08
#  define PTIG_PTIG2 0x04
#  define PTIG_PTIG1 0x02
#  define PTIG_PTIG0 0x01
#define DDRG 0x0252
#  define DDRG_DDRG7 0x80
#  define DDRG_DDRG6 0x40
#  define DDRG_DDRG5 0x20
#  define DDRG_DDRG4 0x10
#  define DDRG_DDRG3 0x08
#  define DDRG_DDRG2 0x04
#  define DDRG_DDRG1 0x02
#  define DDRG_DDRG0 0x01
#define RDRG 0x0253
#  define RDRG_RDRG7 0x80
#  define RDRG_RDRG6 0x40
#  define RDRG_RDRG5 0x20
#  define RDRG_RDRG4 0x10
#  define RDRG_RDRG3 0x08
#  define RDRG_RDRG2 0x04
#  define RDRG_RDRG1 0x02
#  define RDRG_RDRG0 0x01
#define PERG 0x0254
#  define PERG_PERG7 0x80
#  define PERG_PERG6 0x40
#  define PERG_PERG5 0x20
#  define PERG_PERG4 0x10
#  define PERG_PERG3 0x08
#  define PERG_PERG2 0x04
#  define PERG_PERG1 0x02
#  define PERG_PERG0 0x01
#define PPSG 0x0255
#  define PPSG_PPSG7 0x80
#  define PPSG_PPSG6 0x40
#  define PPSG_PPSG5 0x20
#  define PPSG_PPSG4 0x10
#  define PPSG_PPSG3 0x08
#  define PPSG_PPSG2 0x04
#  define PPSG_PPSG1 0x02
#  define PPSG_PPSG0 0x01
#define PIEG 0x0256
#  define PIEG_PIEG7 0x80
#  define PIEG_PIEG6 0x40
#  define PIEG_PIEG5 0x20
#  define PIEG_PIEG4 0x10
#  define PIEG_PIEG3 0x08
#  define PIEG_PIEG2 0x04
#  define PIEG_PIEG1 0x02
#  define PIEG_PIEG0 0x01
#define PIFG 0x0257
#  define PIFG_PIFG7 0x80
#  define PIFG_PIFG6 0x40
#  define PIFG_PIFG5 0x20
#  define PIFG_PIFG4 0x10
#  define PIFG_PIFG3 0x08
#  define PIFG_PIFG2 0x04
#  define PIFG_PIFG1 0x02
#  define PIFG_PIFG0 0x01
#define PTH 0x0258
   /* #  define PTH_ 0x80 */
#  define PTH_PTH6 0x40
#  define PTH_PTH5 0x20
#  define PTH_PTH4 0x10
#  define PTH_PTH3 0x08
#  define PTH_PTH2 0x04
#  define PTH_PTH1 0x02
#  define PTH_PTH0 0x01
#define PTIH 0x0259
   /* #  define PTIH_ 0x80 */
#  define PTIH_PTIH6 0x40
#  define PTIH_PTIH5 0x20
#  define PTIH_PTIH4 0x10
#  define PTIH_PTIH3 0x08
#  define PTIH_PTIH2 0x04
#  define PTIH_PTIH1 0x02
#  define PTIH_PTIH0 0x01
#define DDRH 0x025a
   /* #  define DDRH_ 0x80 */
#  define DDRH_DDRH6 0x40
#  define DDRH_DDRH5 0x20
#  define DDRH_DDRH4 0x10
#  define DDRH_DDRH3 0x08
#  define DDRH_DDRH2 0x04
#  define DDRH_DDRH1 0x02
#  define DDRH_DDRH0 0x01
#define RDRH 0x025b
   /* #  define RDRH_ 0x80 */
#  define RDRH_RDRH6 0x40
#  define RDRH_RDRH5 0x20
#  define RDRH_RDRH4 0x10
#  define RDRH_RDRH3 0x08
#  define RDRH_RDRH2 0x04
#  define RDRH_RDRH1 0x02
#  define RDRH_RDRH0 0x01
#define PERH 0x025c
   /* #  define PERH_ 0x80 */
#  define PERH_PERH6 0x40
#  define PERH_PERH5 0x20
#  define PERH_PERH4 0x10
#  define PERH_PERH3 0x08
#  define PERH_PERH2 0x04
#  define PERH_PERH1 0x02
#  define PERH_PERH0 0x01
#define PPSH 0x025d
   /* #  define PPSH_ 0x80 */
#  define PPSH_PPSH6 0x40
#  define PPSH_PPSH5 0x20
#  define PPSH_PPSH4 0x10
#  define PPSH_PPSH3 0x08
#  define PPSH_PPSH2 0x04
#  define PPSH_PPSH1 0x02
#  define PPSH_PPSH0 0x01
#define PIEH 0x025e
   /* #  define PIEH_ 0x80 */
#  define PIEH_PIEH6 0x40
#  define PIEH_PIEH5 0x20
#  define PIEH_PIEH4 0x10
#  define PIEH_PIEH3 0x08
#  define PIEH_PIEH2 0x04
#  define PIEH_PIEH1 0x02
#  define PIEH_PIEH0 0x01
#define PIFH 0x025f
   /* #  define PIFH_ 0x80 */
#  define PIFH_PIFH6 0x40
#  define PIFH_PIFH5 0x20
#  define PIFH_PIFH4 0x10
#  define PIFH_PIFH3 0x08
#  define PIFH_PIFH2 0x04
#  define PIFH_PIFH1 0x02
#  define PIFH_PIFH0 0x01
#define PTJ 0x0260
#  define PTJ_PTJ7 0x80
#  define PTJ_PTJ6 0x40
   /* #  define PTJ_ 0x20 */
   /* #  define PTJ_ 0x10 */
#  define PTJ_PTJ3 0x08
#  define PTJ_PTJ2 0x04
#  define PTJ_PTJ1 0x02
#  define PTJ_PTJ0 0x01
#define PTIJ 0x0261
#  define PTIJ_PTIJ7 0x80
#  define PTIJ_PTIJ6 0x40
   /* #  define PTIJ_ 0x20 */
   /* #  define PTIJ_ 0x10 */
#  define PTIJ_PTIJ3 0x08
#  define PTIJ_PTIJ2 0x04
#  define PTIJ_PTIJ1 0x02
#  define PTIJ_PTIJ0 0x01
#define DDRJ 0x0262
#  define DDRJ_DDRJ7 0x80
#  define DDRJ_DDRJ6 0x40
   /* #  define DDRJ_ 0x20 */
   /* #  define DDRJ_ 0x10 */
#  define DDRJ_DDRJ3 0x08
#  define DDRJ_DDRJ2 0x04
#  define DDRJ_DDRJ1 0x02
#  define DDRJ_DDRJ0 0x01
#define RDRJ 0x0263
#  define RDRJ_RDRJ7 0x80
#  define RDRJ_RDRJ6 0x40
   /* #  define RDRJ_ 0x20 */
   /* #  define RDRJ_ 0x10 */
#  define RDRJ_RDRJ3 0x08
#  define RDRJ_RDRJ2 0x04
#  define RDRJ_RDRJ1 0x02
#  define RDRJ_RDRJ0 0x01
#define PERJ 0x0264
#  define PERJ_PERJ7 0x80
#  define PERJ_PERJ6 0x40
   /* #  define PERJ_ 0x20 */
   /* #  define PERJ_ 0x10 */
#  define PERJ_PERJ3 0x08
#  define PERJ_PERJ2 0x04
#  define PERJ_PERJ1 0x02
#  define PERJ_PERJ0 0x01
#define PPSJ 0x0265
#  define PPSJ_PPSJ7 0x80
#  define PPSJ_PPSJ6 0x40
   /* #  define PPSJ_ 0x20 */
   /* #  define PPSJ_ 0x10 */
#  define PPSJ_PPSJ3 0x08
#  define PPSJ_PPSJ2 0x04
#  define PPSJ_PPSJ1 0x02
#  define PPSJ_PPSJ0 0x01
#define PIEJ 0x0266
#  define PIEJ_PIEJ7 0x80
#  define PIEJ_PIEJ6 0x40
   /* #  define PIEJ_ 0x20 */
   /* #  define PIEJ_ 0x10 */
#  define PIEJ_PIEJ3 0x08
#  define PIEJ_PIEJ2 0x04
#  define PIEJ_PIEJ1 0x02
#  define PIEJ_PIEJ0 0x01
#define PIFJ 0x0267
#  define PIFJ_PIFJ7 0x80
#  define PIFJ_PIFJ6 0x40
   /* #  define PIFJ_ 0x20 */
   /* #  define PIFJ_ 0x10 */
#  define PIFJ_PIFJ3 0x08
#  define PIFJ_PIFJ2 0x04
#  define PIFJ_PIFJ1 0x02
#  define PIFJ_PIFJ0 0x01
#define PTL 0x0268
   /* #  define PTL_ 0x80 */
#  define PTL_PTL6 0x40
#  define PTL_PTL5 0x20
#  define PTL_PTL4 0x10
#  define PTL_PTL3 0x08
#  define PTL_PTL2 0x04
#  define PTL_PTL1 0x02
#  define PTL_PTL0 0x01
#define PTIL 0x0269
   /* #  define PTIL_ 0x80 */
#  define PTIL_PTIL6 0x40
#  define PTIL_PTIL5 0x20
#  define PTIL_PTIL4 0x10
#  define PTIL_PTIL3 0x08
#  define PTIL_PTIL2 0x04
#  define PTIL_PTIL1 0x02
#  define PTIL_PTIL0 0x01
#define DDRL 0x026a
   /* #  define DDRL_ 0x80 */
#  define DDRL_DDRL6 0x40
#  define DDRL_DDRL5 0x20
#  define DDRL_DDRL4 0x10
#  define DDRL_DDRL3 0x08
#  define DDRL_DDRL2 0x04
#  define DDRL_DDRL1 0x02
#  define DDRL_DDRL0 0x01
#define RDRL 0x026b
   /* #  define RDRL_ 0x80 */
#  define RDRL_RDRL6 0x40
#  define RDRL_RDRL5 0x20
#  define RDRL_RDRL4 0x10
#  define RDRL_RDRL3 0x08
#  define RDRL_RDRL2 0x04
#  define RDRL_RDRL1 0x02
#  define RDRL_RDRL0 0x01
#define PERL 0x026c
   /* #  define PERL_ 0x80 */
#  define PERL_PERL6 0x40
#  define PERL_PERL5 0x20
#  define PERL_PERL4 0x10
#  define PERL_PERL3 0x08
#  define PERL_PERL2 0x04
#  define PERL_PERL1 0x02
#  define PERL_PERL0 0x01
#define PPSL 0x026d
   /* #  define PPSL_ 0x80 */
#  define PPSL_PPSL6 0x40
#  define PPSL_PPSL5 0x20
#  define PPSL_PPSL4 0x10
#  define PPSL_PPSL3 0x08
#  define PPSL_PPSL2 0x04
#  define PPSL_PPSL1 0x02
#  define PPSL_PPSL0 0x01
#define WOML 0x026e
   /* #  define WOML_ 0x80 */
#  define WOML_WOML6 0x40
#  define WOML_WOML5 0x20
#  define WOML_WOML4 0x10
#  define WOML_WOML3 0x08
#  define WOML_WOML2 0x04
#  define WOML_WOML1 0x02
#  define WOML_WOML0 0x01
/* #define RESERVED_0x026f 0x026f */

/* reserved: 0x0270..0x03ff */

#endif /* __IO_MC9S12NE64_H */
