/*
    hcs12mem - HC12/S12 memory reader & writer
    hcs12bdm.h: HCS12 BDM target access routines
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

#ifndef __HCS12BDM_H
#define __HCS12BDM_H

/* chunk sizes for direct memory read/write via BDM */

#define HCS12BDM_RAM_LOAD_CHUNK    256
#define HCS12BDM_EEPROM_READ_CHUNK 256
#define HCS12BDM_FLASH_READ_CHUNK  512
#define HCS12BDM_FLASH_WRITE_CHUNK  16 /* for direct writing ! */

/* HCS12 CPU registers */

#define HCS12BDM_REG_PC  0
#define HCS12BDM_REG_D   1
#define HCS12BDM_REG_X   2
#define HCS12BDM_REG_Y   3
#define HCS12BDM_REG_SP  4
#define HCS12BDM_REG_CCR 5

/* HCS12 BDM registers */

#define HCS12BDM_REG_INSTRUCTION 0xff00 /* 1 byte */
#define HCS12BDM_REG_STATUS      0xff01 /* 1 byte */
#define HCS12BDM_REG_SHIFTER     0xff02 /* 2 bytes */
#define HCS12BDM_REG_ADDRESS     0xff04 /* 2 bytes */
#define HCS12BDM_REG_CCRSAV      0xff06 /* 1 byte */

/* BDM12 STATUS register bits */

#define HCS12BDM_REG_STATUS_ENBDM  0x80
#define HCS12BDM_REG_STATUS_BDMACT 0x40
#define HCS12BDM_REG_STATUS_ENTAG  0x20
#define HCS12BDM_REG_STATUS_SDV    0x10
#define HCS12BDM_REG_STATUS_TRACE  0x08
#define HCS12BDM_REG_STATUS_CLKSW  0x04
#define HCS12BDM_REG_STATUS_UNSEC  0x02

/* BDM12 hardware commands */

#define HCS12BDM_CMD_HW_BACKGROUND    0x90
#define HCS12BDM_CMD_HW_ACK_ENABLE    0xd5
#define HCS12BDM_CMD_HW_ACK_DISABLE   0xd6
#define HCS12BDM_CMD_HW_READ_BD_BYTE  0xe4
#define HCS12BDM_CMD_HW_READ_BD_WORD  0xec
#define HCS12BDM_CMD_HW_READ_BYTE     0xe0
#define HCS12BDM_CMD_HW_READ_WORD     0xe8
#define HCS12BDM_CMD_HW_WRITE_BD_BYTE 0xc4
#define HCS12BDM_CMD_HW_WRITE_BD_WORD 0xcc
#define HCS12BDM_CMD_HW_WRITE_BYTE    0xc0
#define HCS12BDM_CMD_HW_WRITE_WORD    0xc8

/* BDM12 firmware commands */

#define HCS12BDM_CMD_FW_READ_NEXT  0x62
#define HCS12BDM_CMD_FW_READ_PC    0x63
#define HCS12BDM_CMD_FW_READ_D     0x64
#define HCS12BDM_CMD_FW_READ_X     0x65
#define HCS12BDM_CMD_FW_READ_Y     0x66
#define HCS12BDM_CMD_FW_READ_SP    0x67
#define HCS12BDM_CMD_FW_WRITE_NEXT 0x42
#define HCS12BDM_CMD_FW_WRITE_PC   0x43
#define HCS12BDM_CMD_FW_WRITE_D    0x44
#define HCS12BDM_CMD_FW_WRITE_X    0x45
#define HCS12BDM_CMD_FW_WRITE_Y    0x46
#define HCS12BDM_CMD_FW_WRITE_SP   0x47
#define HCS12BDM_CMD_FW_GO         0x08
#define HCS12BDM_CMD_FW_GO_UNTIL   0x0c
#define HCS12BDM_CMD_FW_TRACE1     0x10
#define HCS12BDM_CMD_FW_TAGGO      0x18

/* operation timeouts */

#define HCS12_EEPROM_CMD_TIMEOUT   1000
#define HCS12_FLASH_CMD_TIMEOUT    1000
#define HCS12BDM_RUN_TIMEOUT        5000

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
	int (*read_mem)(uint16_t addr, void *buf, size_t len);
	int (*write_mem)(uint16_t addr, const void *buf, size_t len);

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
hcs12bdm_handler_t;

extern hcs12mem_target_handler_t hcs12mem_target_handler_bdm12pod;
extern hcs12mem_target_handler_t hcs12mem_target_handler_tbdml;

#endif /* __HCS12BDM_H */
