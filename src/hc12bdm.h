/*
    hc12mem - HC12 memory reader & writer
    hc12bdm.h: HC12 BDM target access routines
    $Id$

    Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef __HC12BDM_H
#define __HC12BDM_H

/* HC12 families */

#define HC12_FAMILY_UNKNOWN 0
#define HC12_FAMILY_HC12    1
#define HC12_FAMILY_HCS12   2
#define HC12_FAMILY_HCS12X  3

/* HCS12 EEPROM modules */

#define HCS12_EEPROM_MODULE_UNKNOWN 0
#define HCS12_EEPROM_MODULE_NONE    1
#define HCS12_EEPROM_MODULE_OTHER   2
#define HCS12_EEPROM_MODULE_EETS1K  3
#define HCS12_EEPROM_MODULE_EETS2K  4
#define HCS12_EEPROM_MODULE_EETS4K  5

/* HCS12 FLASH modules */

#define HCS12_FLASH_MODULE_UNKNOWN  0
#define HCS12_FLASH_MODULE_NONE     1
#define HCS12_FLASH_MODULE_OTHER    2
#define HCS12_FLASH_MODULE_FTS16K   3
#define HCS12_FLASH_MODULE_FTS32K   4
#define HCS12_FLASH_MODULE_FTS64K   5
#define HCS12_FLASH_MODULE_FTS128K  6
#define HCS12_FLASH_MODULE_FTS128K1 7
#define HCS12_FLASH_MODULE_FTS256K  8
#define HCS12_FLASH_MODULE_FTS512K4 9

/* HC12 CPU registers */

#define HC12BDM_REG_PC 0
#define HC12BDM_REG_D  1
#define HC12BDM_REG_X  2
#define HC12BDM_REG_Y  3
#define HC12BDM_REG_SP 4

/* HCS12 BDM registers */

#define HC12BDM_REG_INSTRUCTION 0xff00 /* 1 byte */
#define HC12BDM_REG_STATUS      0xff01 /* 1 byte */
#define HC12BDM_REG_SHIFTER     0xff02 /* 2 bytes */
#define HC12BDM_REG_ADDRESS     0xff04 /* 2 bytes */
#define HC12BDM_REG_CCRSAV      0xff06 /* 1 byte */

/* BDM12 STATUS register bits */

#define HC12BDM_REG_STATUS_ENBDM  0x80
#define HC12BDM_REG_STATUS_BDMACT 0x40
#define HC12BDM_REG_STATUS_ENTAG  0x20
#define HC12BDM_REG_STATUS_SDV    0x10
#define HC12BDM_REG_STATUS_TRACE  0x08
#define HC12BDM_REG_STATUS_CLKSW  0x04
#define HC12BDM_REG_STATUS_UNSEC  0x02

/* BDM12 hardware commands */

#define HC12BDM_CMD_HW_BACKGROUND    0x90
#define HC12BDM_CMD_HW_ACK_ENABLE    0xd5
#define HC12BDM_CMD_HW_ACK_DISABLE   0xd6
#define HC12BDM_CMD_HW_READ_BD_BYTE  0xe4
#define HC12BDM_CMD_HW_READ_BD_WORD  0xec
#define HC12BDM_CMD_HW_READ_BYTE     0xe0
#define HC12BDM_CMD_HW_READ_WORD     0xe8
#define HC12BDM_CMD_HW_WRITE_BD_BYTE 0xc4
#define HC12BDM_CMD_HW_WRITE_BD_WORD 0xcc
#define HC12BDM_CMD_HW_WRITE_BYTE    0xc0
#define HC12BDM_CMD_HW_WRITE_WORD    0xc8

/* BDM12 firmware commands */

#define HC12BDM_CMD_FW_READ_NEXT  0x62
#define HC12BDM_CMD_FW_READ_PC    0x63
#define HC12BDM_CMD_FW_READ_D     0x64
#define HC12BDM_CMD_FW_READ_X     0x65
#define HC12BDM_CMD_FW_READ_Y     0x66
#define HC12BDM_CMD_FW_READ_SP    0x67
#define HC12BDM_CMD_FW_WRITE_NEXT 0x42
#define HC12BDM_CMD_FW_WRITE_PC   0x43
#define HC12BDM_CMD_FW_WRITE_D    0x44
#define HC12BDM_CMD_FW_WRITE_X    0x45
#define HC12BDM_CMD_FW_WRITE_Y    0x46
#define HC12BDM_CMD_FW_WRITE_SP   0x47
#define HC12BDM_CMD_FW_GO         0x08
#define HC12BDM_CMD_FW_GO_UNTIL   0x0c
#define HC12BDM_CMD_FW_TRACE1     0x10
#define HC12BDM_CMD_FW_TAGGO      0x18

/* HCS12 I/O registers and bit flags */

#define HCS12_IO_INITRM         0x0010
#define HCS12_IO_INITRM_RAM           0xf8
#define HCS12_IO_INITRM_RAMHAL        0x01
#define HCS12_IO_INITRG         0x0011
#define HCS12_IO_INITRG_REG           0x78
#define HCS12_IO_INITEE         0x0012
#define HCS12_IO_INITEE_EE            0xf8
#define HCS12_IO_INITEE_EEON          0x01
#define HCS12_IO_MISC           0x0013
#define HCS12_IO_MISC_EXSTR           0x0c
#define HCS12_IO_MISC_ROMHM           0x02
#define HCS12_IO_MISC_ROMON           0x01
#define HCS12_IO_PARTID         0x001a
#define HCS12_IO_MEMSIZ         0x001c
#define HCS12_IO_MEMSIZ_REG_SW        0x8000
#define HCS12_IO_MEMSIZ_EEP_SW        0x3000
#define HCS12_IO_MEMSIZ_RAM_SW        0x0700
#define HCS12_IO_MEMSIZ_ROM_SW        0x00c0
#define HCS12_IO_MEMSIZ_PAG_SW        0x0003
#define HCS12_IO_PPAGE          0x0030

/* HCS12 FLASH/EEPROM I/O registers */

#define HCS12_IO_FCLKDIV        0x0100
#define HCS12_IO_FCLKDIV_FDIVLD       0x80
#define HCS12_IO_FCLKDIV_PRDIV8       0x40
#define HCS12_IO_FCLKDIV_FDIV         0x3f
#define HCS12_IO_FSEC           0x0101
#define HCS12_IO_FTSTMOD        0x0101
#define HCS12_IO_FTSTMOD_WRALL        0x10
#define HCS12_IO_FCNFG          0x0103
#define HCS12_IO_FPROT          0x0104
#define HCS12_IO_FSTAT          0x0105
#define HCS12_IO_FSTAT_CBEIF          0x80
#define HCS12_IO_FSTAT_CCIF           0x40
#define HCS12_IO_FSTAT_PVIOL          0x20
#define HCS12_IO_FSTAT_ACCERR         0x10
#define HCS12_IO_FSTAT_BLANK          0x04
#define HCS12_IO_FCMD           0x0106
#define HCS12_IO_FCMD_ERASE_VERIFY    0x05
#define HCS12_IO_FCMD_PROGRAM         0x20
#define HCS12_IO_FCMD_SECTOR_ERASE    0x40
#define HCS12_IO_FCMD_MASS_ERASE      0x41
#define HCS12_IO_FADDR          0x0108
#define HCS12_IO_FDATA          0x010a
#define HCS12_IO_ECLKDIV        0x0110
#define HCS12_IO_ECLKDIV_FDIVLD       0x80
#define HCS12_IO_ECLKDIV_PRDIV8       0x40
#define HCS12_IO_ECLKDIV_FDIV         0x3f
#define HCS12_IO_ECNFG          0x0113
#define HCS12_IO_EPROT          0x0114
#define HCS12_IO_EPROT_EPOPEN         0x80
#define HCS12_IO_EPROT_EPDIS          0x08
#define HCS12_IO_EPROT_EP             0x07
#define HCS12_IO_ESTAT          0x0115
#define HCS12_IO_ESTAT_CBEIF          0x80
#define HCS12_IO_ESTAT_CCIF           0x40
#define HCS12_IO_ESTAT_PVIOL          0x20
#define HCS12_IO_ESTAT_ACCERR         0x10
#define HCS12_IO_ESTAT_BLANK          0x04
#define HCS12_IO_ECMD           0x0116
#define HCS12_IO_ECMD_ERASE_VERIFY    0x05
#define HCS12_IO_ECMD_PROGRAM         0x20
#define HCS12_IO_ECMD_SECTOR_ERASE    0x40
#define HCS12_IO_ECMD_MASS_ERASE      0x41
#define HCS12_IO_EADDR          0x0118
#define HCS12_IO_EDATA          0x011a

/* HCS12 FLASH window */

#define HCS12_FLASH_BANK_WINDOW_ADDR 0x8000
#define HCS12_FLASH_BANK_WINDOW_SIZE 0x4000

/* HCS12 FLASH/EEPROM addresses and bit masks */

#define HCS12_FLASH_BACKDOOR   0xff00
#define HCS12_FLASH_FPROT      0xff0d
#define HCS12_FLASH_FPROT_FPOPEN 0x80
#define HCS12_FLASH_FPROT_NV6    0x40
#define HCS12_FLASH_FPROT_FPHDIS 0x20
#define HCS12_FLASH_FPROT_FPHS   0x18
#define HCS12_FLASH_FPROT_FPLDIS 0x04
#define HCS12_FLASH_FPROT_FPLS   0x03
#define HCS12_FLASH_FSEC       0xff0f
#define HCS12_FLASH_FSEC_KEYEN 0x80
#define HCS12_FLASH_FSEC_NV6   0x40
#define HCS12_FLASH_FSEC_NV5   0x40
#define HCS12_FLASH_FSEC_NV4   0x40
#define HCS12_FLASH_FSEC_NV3   0x40
#define HCS12_FLASH_FSEC_NV2   0x40
#define HCS12_FLASH_FSEC_SEC   0x03

#define HC12_EEPROM_RESERVED_SIZE 16
#define HC12_EEPROM_RESERVED_EPROT_OFFSET 13

/* HCS12 FLASH/EEPROM clock range */

#define HCS12_FCLK_MIN 150000
#define HCS12_FCLK_MAX 200000

/* operation timeouts */

#define HCS12_EEPROM_CMD_TIMEOUT   1000
#define HCS12_FLASH_CMD_TIMEOUT    1000
#define HC12BDM_RUN_TIMEOUT        5000

/* BDM handler */

typedef struct
{
	/* generic management */

	int (*open)(void);
	int (*close)(void);
	int (*reset_normal)(void);
	int (*reset_special)(void);

	/* hardware commands */

	int (*background)(void);
	int (*ack_enable)(void);
	int (*ack_disable)(void);
	int (*read_bd_byte)(uint16_t addr, uint8_t *v);
	int (*read_bd_word)(uint16_t addr, uint16_t *v);
	int (*read_byte)(uint16_t addr, uint8_t *v);
	int (*read_word)(uint16_t addr, uint16_t *v);
	int (*write_bd_byte)(uint16_t addr, uint8_t v);
	int (*write_bd_word)(uint16_t addr, uint16_t v);
	int (*write_byte)(uint16_t addr, uint8_t v);
	int (*write_word)(uint16_t addr, uint16_t v);
	int (*read_mem)(uint16_t addr, uint8_t *buf, uint16_t len);
	int (*write_mem)(uint16_t addr, const uint8_t *buf, uint16_t len);

	/* firmware commands */

	int (*read_next)(uint16_t *v);
	int (*read_reg)(int reg, uint16_t *v);
	int (*write_next)(uint16_t v);
	int (*write_reg)(int reg, uint16_t v);
	int (*go)(void);
	int (*go_until)(void);
	int (*go_trace1)(void);
	int (*go_taggo)(void);
}
hc12bdm_handler_t;

extern hc12mem_target_handler_t hc12mem_target_handler_bdm12pod;

#endif /* __HC12BDM_H */
