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

#include "hc12mem.h"
#include "hc12mcu.h"

static const struct
{
	const char *name;
	int type;
}
hc12_cpu_family_table[] =
{
	{ "HC12",   HC12_FAMILY_HC12 },
	{ "HCS12",  HC12_FAMILY_HCS12 },
	{ "HCS12X", HC12_FAMILY_HCS12X },
	{ NULL,     HC12_FAMILY_UNKNOWN }
};

static const struct
{
	const char *name;
	int type;
	uint32_t size;
}
hc12_eeprom_module_table[] =
{
	{ "NONE",      HCS12_EEPROM_MODULE_NONE,        0 },
	{ "OTHER",     HCS12_EEPROM_MODULE_OTHER,       0 },
	{ "EETS1K",    HCS12_EEPROM_MODULE_EETS1K,   1024 },
	{ "EETS2K",    HCS12_EEPROM_MODULE_EETS2K,   2048 },
	{ "EETS4K",    HCS12_EEPROM_MODULE_EETS4K,   4096 },
	{ NULL,        HCS12_EEPROM_MODULE_UNKNOWN,     0 }
};

static const struct
{
	const char *name;
	int type;
	int blocks;
	uint32_t size;
	uint32_t sector;
	uint32_t nb_size;
	uint32_t nb_base;
	uint8_t ppage_base;
	uint8_t ppage_count;
}
hc12_flash_module_table[] =
{
	/* name        type                    blocks        size sector  lsize   lbase ppbase ppcnt */
	{ "NONE",      HCS12_FLASH_MODULE_NONE,     0,          0,    0,      0,      0,    0,  0 },
	{ "OTHER",     HCS12_FLASH_MODULE_OTHER,    0,          0,    0,      0,      0,    0,  0 },
	{ "FTS16K",    HCS12_FLASH_MODULE_FTS16K,   1,  16 * 1024,  512, 0x4000, 0xc000, 0x3f,  1 },
	{ "FTS32K",    HCS12_FLASH_MODULE_FTS32K,   1,  32 * 1024,  512, 0x8000, 0x8000, 0x3e,  2 },
	{ "FTS64K",    HCS12_FLASH_MODULE_FTS64K,   1,  64 * 1024,  512, 0xc000, 0x4000, 0x3c,  4 },
	{ "FTS128K",   HCS12_FLASH_MODULE_FTS128K,  2, 128 * 1024,  512, 0xc000, 0x4000, 0x38,  8 },
	{ "FTS128K1",  HCS12_FLASH_MODULE_FTS128K1, 1, 128 * 1024, 1024, 0xc000, 0x4000, 0x38,  8 },
	{ "FTS256K",   HCS12_FLASH_MODULE_FTS256K,  4, 256 * 1024,  512, 0xc000, 0x4000, 0x30, 16 },
	{ "FTS512K4",  HCS12_FLASH_MODULE_FTS512K4, 4, 512 * 1024, 1024, 0xc000, 0x4000, 0x20, 32 },
	{ NULL,        HCS12_FLASH_MODULE_UNKNOWN,  0,          0,    0,      0,      0,    0,  0 }
};

hc12mcu_target_t hc12mcu_target;


int hc12mcu_target_parse(void)
{
	int i;

	/* get target info and MCU type */

	hc12mcu_target.info_str = hc12mem_target_info("info", TRUE);
	if (hc12mcu_target.info_str == NULL)
	{
		error("missing target info string\n");
		return EINVAL;
	}

	hc12mcu_target.mcu_str = hc12mem_target_info("mcu", TRUE);
	if (hc12mcu_target.mcu_str == NULL)
	{
		error("missing target MCU type\n");
		return EINVAL;
	}

	/* get MCU family */

	hc12mcu_target.family_str = hc12mem_target_info("family", TRUE);
	if (hc12mcu_target.family_str == NULL)
	{
		error("MCU family not specified in target description\n");
		return EINVAL;
	}
	for (i = 0; hc12_cpu_family_table[i].name != NULL; ++ i)
	{
		if (strcmp(hc12mcu_target.family_str, hc12_cpu_family_table[i].name) == 0)
		{
			hc12mcu_target.family = hc12_cpu_family_table[i].type;
			break;
		}
	}
	if (hc12_cpu_family_table[i].name == NULL)
	{
		error("MCU family unknown: %s\n",
		      (const char *)hc12mcu_target.family_str);
		return EINVAL;
	}

	/* get RAM info */

	if (hc12mem_target_param("ram_size", &hc12mcu_target.ram_size, 0) != 0)
		return EINVAL;
	if (hc12mcu_target.ram_size == 0)
	{
		error("unknown RAM size\n");
		return EINVAL;
	}

	/* get EEPROM info */

	hc12mcu_target.eeprom_module_str = hc12mem_target_info("eeprom_module", TRUE);
	if (hc12mcu_target.eeprom_module_str == NULL)
	{
		error("EEPROM module type not specified in target description\n");
		return EINVAL;
	}
	for (i = 0; hc12_eeprom_module_table[i].name != NULL; ++ i)
	{
		if (strcmp(hc12mcu_target.eeprom_module_str, hc12_eeprom_module_table[i].name) == 0)
		{
			hc12mcu_target.eeprom_module = hc12_eeprom_module_table[i].type;
			hc12mcu_target.eeprom_size = hc12_eeprom_module_table[i].size;
			break;
		}
	}
	if (hc12_eeprom_module_table[i].name == NULL)
	{
		error("EEPROM module type unknown: %s\n",
		      (const char *)hc12mcu_target.eeprom_module_str);
		return EINVAL;
	}

	if (hc12mem_target_param("eeprom_size", &hc12mcu_target.eeprom_size, hc12mcu_target.eeprom_size) != 0)
		return EINVAL;

	if ((hc12mcu_target.eeprom_module == HCS12_EEPROM_MODULE_NONE && hc12mcu_target.eeprom_size != 0) ||
	    (hc12mcu_target.eeprom_module == HCS12_EEPROM_MODULE_OTHER && hc12mcu_target.eeprom_size == 0))
	{
		error("invalid EEPROM size\n");
		return EINVAL;
	}

	/* get FLASH info */

	hc12mcu_target.flash_module_str = hc12mem_target_info("flash_module", TRUE);
	if (hc12mcu_target.flash_module_str == NULL)
	{
		error("FLASH module type not specified in target description\n");
		return EINVAL;
	}
	for (i = 0; hc12_flash_module_table[i].name != NULL; ++ i)
	{
		if (strcmp(hc12mcu_target.flash_module_str, hc12_flash_module_table[i].name) == 0)
		{
			hc12mcu_target.flash_module = hc12_flash_module_table[i].type;
			hc12mcu_target.flash_blocks = hc12_flash_module_table[i].blocks;
			hc12mcu_target.flash_size = hc12_flash_module_table[i].size;
			hc12mcu_target.flash_sector = hc12_flash_module_table[i].sector;
			hc12mcu_target.flash_nb_size = hc12_flash_module_table[i].nb_size;
			hc12mcu_target.flash_nb_base = hc12_flash_module_table[i].nb_base;
			hc12mcu_target.ppage_base = hc12_flash_module_table[i].ppage_base;
			hc12mcu_target.ppage_count = hc12_flash_module_table[i].ppage_count;
			break;
		}
	}
	if (hc12_flash_module_table[i].name == NULL)
	{
		error("FLASH module type unknown: %s\n",
		      (const char *)hc12mcu_target.flash_module_str);
		return EINVAL;
	}

	if (hc12mem_target_param("flash_size", &hc12mcu_target.flash_size, hc12mcu_target.flash_size) != 0)
		return EINVAL;
	if (hc12mcu_target.flash_module == HCS12_FLASH_MODULE_OTHER && hc12mcu_target.flash_size == 0)
	{
		error("invalid FLASH size\n");
		return EINVAL;
	}

	if (hc12mem_target_param("flash_nb_size", &hc12mcu_target.flash_nb_size, hc12mcu_target.flash_nb_size) != 0)
		return EINVAL;
	if (hc12mem_target_param("flash_nb_base", &hc12mcu_target.flash_nb_base, hc12mcu_target.flash_nb_base) != 0)
		return EINVAL;

	/* get PPAGE base and pages count */

	if (hc12mem_target_param("ppage_base", &hc12mcu_target.ppage_base, hc12mcu_target.ppage_base) != 0)
		return EINVAL;
	if (hc12mem_target_param("ppage_count", &hc12mcu_target.ppage_count, hc12mcu_target.ppage_count) != 0)
		return EINVAL;
	if (hc12mem_target_param("ppage_default", &hc12mcu_target.ppage_default, 0) != 0)
		return EINVAL;

	hc12mcu_target.flash_linear_base =
		hc12mcu_target.ppage_base * HCS12_FLASH_BANK_WINDOW_SIZE;
	hc12mcu_target.flash_block_size =
		hc12mcu_target.flash_size / hc12mcu_target.flash_blocks;

	return 0;
}


/*
 *  FLASH address translation (for writing S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

uint32_t hc12mcu_flash_write_address_nb(uint32_t addr)
{
	return addr + hc12mcu_target.flash_nb_base;
}


uint32_t hc12mcu_flash_write_address_bl(uint32_t addr)
{
	return addr + hc12mcu_target.flash_linear_base;
}


uint32_t hc12mcu_flash_write_address_bp(uint32_t addr)
{
	return (((uint32_t)hc12mcu_target.ppage_base + (addr / HCS12_FLASH_BANK_WINDOW_SIZE)) << 16) +
		HCS12_FLASH_BANK_WINDOW_ADDR + (addr % HCS12_FLASH_BANK_WINDOW_SIZE);
}


/*
 *  FLASH address translation (for reading S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

uint32_t hc12mcu_flash_read_address_nb(uint32_t addr)
{
	return addr - hc12mcu_target.flash_nb_base;

#if 0
	static const uint32_t table[4] =
	{ 0xffff0000, 0x00008000, 0x00000000, 0x0000c000 };

	if (addr > 0x0000ffff)
		return 0xffff0000;
	return table[addr / HCS12_FLASH_BANK_WINDOW_SIZE] +
		(addr % HCS12_FLASH_BANK_WINDOW_SIZE);
#endif
}


uint32_t hc12mcu_flash_read_address_bl(uint32_t addr)
{
	return addr - hc12mcu_target.flash_linear_base;
}


uint32_t hc12mcu_flash_read_address_bp(uint32_t addr)
{
	return ((addr >> 16) - (uint32_t)hc12mcu_target.ppage_base) * HCS12_FLASH_BANK_WINDOW_SIZE +
		(addr % HCS12_FLASH_BANK_WINDOW_SIZE);
}


/*
 *  convert FLASH linear address into PPAGE value
 *
 *  in:
 *    addr - linear address
 *  out:
 *    PPAGE value
 */

uint8_t hc12mcu_linear_to_ppage(uint32_t addr)
{
	uint32_t page;
	uint32_t p;

	page = addr / HCS12_FLASH_BANK_WINDOW_SIZE;
	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
	{
		if (hc12mcu_target.ppage_count <= 2)
			p = hc12mcu_target.ppage_base + page;
		else
		{
			if (page == 0)
				p = hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 2;
			else if (page == 1)
				p = hc12mcu_target.ppage_default;
			else if (page == 2)
				p = hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 1;
		}
	}
	else
		p = hc12mcu_target.ppage_base + page;

	return (uint8_t)p;
}


/*
 *  convert FLASH linear address into FLASH block number
 *
 *  in:
 *    addr - linear address
 *  out:
 *    FLASH block number
 */

uint8_t hc12mcu_linear_to_block(uint32_t addr)
{
	return (uint8_t)(hc12mcu_target.flash_blocks - addr / hc12mcu_target.flash_block_size - 1);
}


/*
 *  convert FLASH block number to PPAGE base value
 *
 *  in:
 *    block - FLASH block number
 *  out:
 *    ppage base
 */

uint8_t hc12mcu_block_to_ppage_base(uint32_t block)
{
	uint32_t ppb = hc12mcu_target.ppage_count / hc12mcu_target.flash_blocks;
	return (uint8_t)(hc12mcu_target.ppage_base +
		(hc12mcu_target.flash_blocks - block - 1) * ppb);
}
