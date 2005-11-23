/*
    hc12mem - HC12 memory reader & writer
    hc12mcu.h: MCU target definitions
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

#ifndef __HC12MCU_H
#define __HC12MCU_H

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

/* target characteristics */

typedef struct
{
	/* values read from target description */

	const char *info_str;
	const char *mcu_str;
	const char *family_str;
	int family;
	uint32_t ram_size;
	const char *eeprom_module_str;
	int eeprom_module;
	uint32_t eeprom_size;
	const char *flash_module_str;
	int flash_module;
	int flash_blocks;
	uint32_t flash_size;
	uint32_t flash_sector;
	uint32_t flash_nb_base;
	uint32_t flash_nb_size;
	uint32_t flash_linear_base;
	uint32_t flash_block_size;
	uint32_t ppage_base;
	uint32_t ppage_count;
	uint32_t ppage_default;

	/* values read from target device */

	uint32_t reg_base;
	uint32_t reg_space;
	uint32_t reg_size;
	uint32_t ram_base;
	uint32_t ram_space;
	uint32_t eeprom_base;
	uint32_t eeprom_space;
	int secured;

	/* direct memory access */

	int (*read_byte)(uint16_t addr, uint8_t *v);
	int (*read_word)(uint16_t addr, uint16_t *v);
	int (*write_byte)(uint16_t addr, uint8_t v);
	int (*write_word)(uint16_t addr, uint16_t v);
}
hc12mcu_target_t;

extern hc12mcu_target_t hc12mcu_target;

extern int hc12mcu_target_parse(void);
extern void hc12mcu_show_partid(uint16_t id);
extern void hcs12mcu_partid(uint16_t id, int verbose);
extern int hc12mcu_identify(int verbose);

#define hc12mcu_flash_addr_window(addr) \
	(HCS12_FLASH_BANK_WINDOW_ADDR + ((addr) % HCS12_FLASH_BANK_WINDOW_SIZE))

extern uint32_t hc12mcu_flash_read_address_nb(uint32_t addr);
extern uint32_t hc12mcu_flash_read_address_bl(uint32_t addr);
extern uint32_t hc12mcu_flash_read_address_bp(uint32_t addr);
extern uint32_t hc12mcu_flash_write_address_nb(uint32_t addr);
extern uint32_t hc12mcu_flash_write_address_bl(uint32_t addr);
extern uint32_t hc12mcu_flash_write_address_bp(uint32_t addr);
extern uint8_t hc12mcu_linear_to_ppage(uint32_t linear);
extern uint8_t hc12mcu_linear_to_block(uint32_t linear);
extern uint8_t hc12mcu_block_to_ppage_base(uint32_t block);

extern int hc12mcu_flash_read(const char *file, size_t chunk,
	int (*f)(uint32_t addr, size_t size, void *buf));
extern int hc12mcu_flash_write(const char *file, size_t chunk,
	int (*f)(uint32_t addr, size_t size, const void *buf));
extern int hc12mcu_eeprom_read(const char *file, size_t chunk,
	int (*f)(uint16_t addr, size_t size, void *buf));
extern int hc12mcu_eeprom_write(const char *file, size_t chunk,
	int (*f)(uint16_t addr, size_t size, const void *buf));
extern int hc12mcu_eeprom_protect(const char *opt,
	int (*eeww)(uint16_t addr, uint16_t v));

#endif /* __HC12MCU_H */
