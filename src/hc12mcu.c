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
#include "srec.h"

static const char *hcs12_family_table[] =
{
	"D",
	"H",
	"B",
	"C",
	"T",
	"E",
	"U",
	"reserved",
	"NE",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown",
	"unknown"
};

static uint16_t hcs12_reg_space_table[] =
{
	0x0400,
	0x0800
};

static uint16_t hcs12_eeprom_space_table[] =
{
	0x0000,
	0x0800,
	0x1000,
	0x2000
};

static uint16_t hcs12_ram_space_table[] =
{
	0x0800,
	0x1000,
	0x2000,
	0x2000,
	0x4000,
	0x4000,
	0x4000,
	0x4000
};

static uint16_t hcs12_ram_size_table[] =
{
	0x0800,
	0x1000,
	0x1800,
	0x2000,
	0x2800,
	0x3000,
	0x3800,
	0x4000
};

static int hcs12_memory_table[] =
{
	256,
	128,
	64,
	32,
	512,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0,
	0
};

static const char *hcs12_rom_space_table[] =
{
	"0kB",
	"16kB",
	"48kB",
	"64kB"
};

static const char *hcs12_page_space_table[] =
{
	"876kB/128kB",
	"768kB/256kB",
	"512kB/512kB",
	"0kB/1MB"
};

static const char *hcs12_flash_prot_area_table[] =
{
	"2kB",
	"4kB",
	"8kB",
	"16kB"
};

static const struct
{
	uint16_t size;
	const char *text;
}
hcs12_eeprom_prot_area_table[] =
{
	{  64,  "64B" },
	{ 128, "128B" },
	{ 192, "192B" },
	{ 256, "256B" },
	{ 320, "320B" },
	{ 384, "384B" },
	{ 448, "448B" },
	{ 512, "512B" }
};

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

	if (options.verbose)
	{
		if (options.osc != 0)
		{
			printf("target info <%s>\n"
			       "target mcu <%s> family <%s> osc <%lu.%06lu MHz>\n",
			       (const char *)hc12mcu_target.info_str,
			       (const char *)hc12mcu_target.mcu_str,
			       (const char *)hc12mcu_target.family_str,
			       (unsigned long)(options.osc / 1000000),
			       (unsigned long)(options.osc % 1000000));
		}
		else
		{
			printf("target info <%s>\n"
			       "target mcu <%s> family <%s>\n",
			       (const char *)hc12mcu_target.info_str,
			       (const char *)hc12mcu_target.mcu_str,
			       (const char *)hc12mcu_target.family_str);
		}
	}

	/* reg block info */

	hc12mcu_target.reg_base = 0;
	hc12mcu_target.reg_size = 0;

	/* get RAM info */

	hc12mcu_target.ram_base = 0;
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
			hc12mcu_target.eeprom_base = 0;
			hc12mcu_target.eeprom_size = hc12_eeprom_module_table[i].size;
			hc12mcu_target.eeprom_space = hc12_eeprom_module_table[i].size;
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
	if (hc12mem_target_param("ppage_default", &hc12mcu_target.ppage_default, hc12mcu_target.ppage_base) != 0)
		return EINVAL;

	hc12mcu_target.flash_linear_base =
		hc12mcu_target.ppage_base * HCS12_FLASH_BANK_WINDOW_SIZE;
	hc12mcu_target.flash_block_size =
		hc12mcu_target.flash_size / hc12mcu_target.flash_blocks;

	hc12mcu_target.read_byte = NULL;
	hc12mcu_target.read_word = NULL;
	hc12mcu_target.write_byte = NULL;
	hc12mcu_target.write_word = NULL;

	return 0;
}


void hcs12mcu_partid(uint16_t id, int verbose)
{
	if (!verbose)
		return;

	printf("HCS12 part id <0x%04x> family <%s> memory <%ukB> mask <%u.%u>\n",
	       (unsigned int)id,
	       (const char *)hcs12_family_table[(id >> 12) & 0x000f],
	       (unsigned int)(hcs12_memory_table[(id >> 8) & 0x000f]),
	       (unsigned int)((id >> 4) & 0x000f),
	       (unsigned int)(id & 0x000f));
}


int hc12mcu_identify(int verbose)
{
	int ret;
	uint8_t initrm;
	uint8_t initrg;
	uint8_t initee;
	uint8_t misc;
	uint8_t fprot;
	uint8_t fsec;
	uint8_t eprot;
	uint16_t mem;
	uint32_t ram_size;
	int i;

	ret = (*hc12mcu_target.read_byte)(HCS12_IO_INITRG, &initrg);
	if (ret != 0)
		return ret;
	ret = (*hc12mcu_target.read_byte)(HCS12_IO_INITRM, &initrm);
	if (ret != 0)
		return ret;
	ret = (*hc12mcu_target.read_byte)(HCS12_IO_INITEE, &initee);
	if (ret != 0)
		return ret;

	hc12mcu_target.reg_base = (uint16_t)((initrg & HCS12_IO_INITRG_REG) << 8);
	hc12mcu_target.ram_base = (uint32_t)((initrm & HCS12_IO_INITRM_RAM) << 8);
	hc12mcu_target.eeprom_base = (uint32_t)((initee & HCS12_IO_INITEE_EE) << 8);

	if (hc12mcu_target.family >= HC12_FAMILY_HCS12)
	{
		ret = (*hc12mcu_target.read_word)(HCS12_IO_MEMSIZ, &mem);
		if (ret != 0)
			return ret;
		ret = (*hc12mcu_target.read_byte)(HCS12_IO_MISC, &misc);
		if (ret != 0)
			return ret;
		ret = (*hc12mcu_target.read_byte)(HCS12_IO_FSEC, &fsec);
		if (ret != 0)
			return ret;

		hc12mcu_target.reg_space = hcs12_reg_space_table[(mem & HCS12_IO_MEMSIZ_REG_SW) >> 15];
		hc12mcu_target.ram_space = hcs12_ram_space_table[(mem & HCS12_IO_MEMSIZ_RAM_SW) >> 8];
		ram_size = (uint32_t)hcs12_ram_size_table[(mem & HCS12_IO_MEMSIZ_RAM_SW) >> 8];
		hc12mcu_target.ram_base &= ~(hc12mcu_target.ram_space - 1);
		if (initrm & HCS12_IO_INITRM_RAMHAL)
		{
			hc12mcu_target.ram_base =
				hc12mcu_target.ram_base +
				hc12mcu_target.ram_space -
				hc12mcu_target.ram_size;
		}
		hc12mcu_target.eeprom_space = hcs12_eeprom_space_table[(mem & HCS12_IO_MEMSIZ_EEP_SW) >> 12];
		hc12mcu_target.secured = (fsec & HCS12_FLASH_FSEC_SEC) == 0x02 ? FALSE : TRUE;
	}
	else
	{
		hc12mcu_target.secured = FALSE;
	}

	if (hc12mcu_target.family == HC12_FAMILY_HC12 && verbose)
	{
		printf("HC12 register base <0x%04x>\n",
		       (unsigned int)hc12mcu_target.reg_base);
		printf("HC12 RAM size <%ukB> address range <0x%04x-0x%04x>\n",
		       (unsigned int)(hc12mcu_target.ram_size / 1024),
		       (unsigned int)hc12mcu_target.ram_base,
		       (unsigned int)(hc12mcu_target.ram_base + hc12mcu_target.ram_size - 1));
		if (hc12mcu_target.eeprom_size == 0)
			printf("HC12 EEPROM not present\n");
		else
		{
			printf("HC12 EEPROM size <%ukB> state <%s> address range <0x%04x-0x%04x>\n",
			       (unsigned int)(hc12mcu_target.eeprom_size / 1024),
			       (const char *)((initee & HCS12_IO_INITEE_EEON) ? "enabled" : "disabled"),
			       (unsigned int)hc12mcu_target.eeprom_base,
			       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - 1));
		}
	}

	if (hc12mcu_target.family >= HC12_FAMILY_HCS12 && verbose)
	{
		printf("HCS12 part security <%s> backdoor key <%s>\n",
		       (const char *)(hc12mcu_target.secured ? "on" : "off"),
		       (const char *)((fsec & HCS12_FLASH_FSEC_KEYEN) ? "enabled" : "disabled"));
		printf("HCS12 register space <%ukB> address range <0x%04x-0x%04x>\n",
		       (unsigned int)(hc12mcu_target.reg_space / 1024),
		       (unsigned int)hc12mcu_target.reg_base,
		       (unsigned int)(hc12mcu_target.reg_base + hc12mcu_target.reg_space - 1));
		printf("HCS12 RAM size <%ukB> space <%ukB> align <%s> address range <0x%04x-0x%04x>\n",
		       (unsigned int)(hc12mcu_target.ram_size / 1024),
		       (unsigned int)(hc12mcu_target.ram_space / 1024),
		       (const char *)((initrm & HCS12_IO_INITRM_RAMHAL) ? "high" : "low"),
		       (unsigned int)hc12mcu_target.ram_base,
		       (unsigned int)(hc12mcu_target.ram_base + hc12mcu_target.ram_size - 1));
		if (hc12mcu_target.eeprom_size == 0)
			printf("HCS12 EEPROM not present\n");
		else
		{
			ret = (*hc12mcu_target.read_byte)(HCS12_IO_EPROT, &eprot);
			if (ret != 0)
				return ret;

			printf("HCS12 EEPROM module <%s> state <%s>\n",
			       (const char *)hc12mcu_target.eeprom_module_str,
			       (const char *)((initee & HCS12_IO_INITEE_EEON) ? "enabled" : "disabled"));
			printf("HCS12 EEPROM size <%ukB> space <%ukB> address range <0x%04x-0x%04x>\n",
			       (unsigned int)(hc12mcu_target.eeprom_size / 1024),
			       (unsigned int)(hc12mcu_target.eeprom_space / 1024),
			       (unsigned int)hc12mcu_target.eeprom_base,
			       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - 1));
			if (eprot & HCS12_IO_EPROT_EPDIS)
			{
				printf("HCS12 EEPROM protection all <%s> address range <off>\n",
				       (const char *)((eprot & HCS12_IO_EPROT_EPOPEN) ? "off" : "on"));
			}
			else
			{
				printf("HCS12 EEPROM protection all <%s> address range <0x%04x-0x%04x %uB%s>\n",
				       (const char *)((eprot & HCS12_IO_EPROT_EPOPEN) ? "off" : "on"),
				       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size -
						      hcs12_eeprom_prot_area_table[eprot & HCS12_IO_EPROT_EP].size),
				       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - 1),
				       (unsigned int)hcs12_eeprom_prot_area_table[eprot & HCS12_IO_EPROT_EP].size,
				       (const char *)((eprot & HCS12_IO_EPROT_EPOPEN) ? "" : " ignored"));
			}
		}
		printf("HCS12 FLASH module <%s> state <%s> ROMHM <%s>\n",
		       (const char *)hc12mcu_target.flash_module_str,
		       (const char *)((misc & HCS12_IO_MISC_ROMON) ? "enabled" : "disabled"),
		       (const char *)((misc & HCS12_IO_MISC_ROMHM) ? "yes" : "no"));
		printf("HCS12 FLASH size <%ukB> space <%s> off-chip/on-chip space <%s>\n",
		       (unsigned int)(hc12mcu_target.flash_size / 1024),
		       (const char *)hcs12_rom_space_table[(mem & HCS12_IO_MEMSIZ_ROM_SW) >> 6],
		       (const char *)hcs12_page_space_table[(mem & HCS12_IO_MEMSIZ_PAG_SW) >> 0]);

		for (i = 0; i < hc12mcu_target.flash_blocks; ++ i)
		{
			ret = (*hc12mcu_target.read_byte)((uint16_t)(HCS12_FLASH_FPROT - i), &fprot);
			if (ret != 0)
				return ret;

			if (hc12mcu_target.flash_blocks == 1)
			{
				printf("HCS12 FLASH protection all <%s> high area <%s> low area <%s>\n",
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPOPEN) ? "off" : "on"),
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPHS) ? "off" :
						      hcs12_flash_prot_area_table[(fprot & HCS12_FLASH_FPROT_FPHS) >> 3]),
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPLS) ? "off" :
						      hcs12_flash_prot_area_table[(fprot & HCS12_FLASH_FPROT_FPLS) >> 0]));
			}
			else
			{
				printf("HCS12 FLASH block <%u> protection all <%s> high area <%s> low area <%s>\n",
				       (unsigned int)i,
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPOPEN) ? "off" : "on"),
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPHS) ? "off" :
						      hcs12_flash_prot_area_table[(fprot & HCS12_FLASH_FPROT_FPHS) >> 3]),
				       (const char *)((fprot & HCS12_FLASH_FPROT_FPLS) ? "off" :
						      hcs12_flash_prot_area_table[(fprot & HCS12_FLASH_FPROT_FPLS) >> 0]));
			}
		}
	}

	return 0;
}


/*
 *  EEPROM address translation (for reading S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

static uint32_t hc12mcu_eeprom_address(uint32_t addr)
{
	if (addr < hc12mcu_target.eeprom_base ||
	    addr >= hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size)
		return hc12mcu_target.eeprom_size;
	return addr - hc12mcu_target.eeprom_base;
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
	uint8_t p;

	page = addr / HCS12_FLASH_BANK_WINDOW_SIZE;
	p = (uint8_t)(hc12mcu_target.ppage_base + page);

	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
	{
		if (hc12mcu_target.ppage_count <= 2)
			return p;
		else if (page == 0)
			return (uint8_t)(hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 2);
		else if (page == 1)
			return (uint8_t)(hc12mcu_target.ppage_default);
		else if (page == 2)
			return (uint8_t)(hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 1);
	}

	/* address out of non-banked space */

	return p;
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


/*
 *  read FLASH memory
 *
 *  in:
 *    file - file name to write
 *  out:
 *    status code (errno-like)
 */

int hc12mcu_flash_read(const char *file, size_t chunk,
	int (*f)(uint32_t addr, void *buf, size_t size))
{
	int ret;
	uint32_t size;
	uint8_t *buf;
	unsigned long t;
	uint32_t i;
	uint32_t (*adc)(uint32_t addr);

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH read not possible - no FLASH memory\n");
		return EINVAL;
	}

	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
		size = hc12mcu_target.flash_nb_size;
	else
		size = hc12mcu_target.flash_size;

	buf = malloc(size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	t = progress_start("FLASH read: data");
	for (i = 0; i < size; i += (uint32_t)chunk)
	{
		ret = (*f)(i, buf + i, chunk);
		if (ret != 0)
		{
			free(buf);
			return ret;
		}

		progress_report(i + (uint32_t)chunk, size);
	}
	progress_stop(t, "FLASH read: data", size);

	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
		adc = hc12mcu_flash_write_address_nb;
	else if (options.flash_addr == HC12MEM_FLASH_ADDR_BANKED_LINEAR)
		adc = hc12mcu_flash_write_address_bl;
	else if (options.flash_addr == HC12MEM_FLASH_ADDR_BANKED_PPAGE)
		adc = hc12mcu_flash_write_address_bp;
	else
		adc = NULL;

	ret = srec_write(file, "FLASH image", 0, size, buf, size - 2, adc,
		!options.include_erased, options.srec_size);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	if (options.verbose)
	{
		printf("FLASH read: data file <%s> written\n",
		       (const char *)file);
	}

	free(buf);
	return 0;
}


/*
 *  write target FLASH
 *
 *  in:
 *    file - file name with data for programming
 *  out:
 *    status code (errno-like)
 */

int hc12mcu_flash_write(const char *file, size_t chunk,
	int (*f)(uint32_t addr, const void *buf, size_t size))
{
	uint32_t (*adc)(uint32_t addr);
	uint32_t size;
	uint8_t *buf;
	char info[256];
	uint32_t addr_min;
	uint32_t addr_max;
	uint32_t len;
	uint32_t i, j;
	uint32_t end;
	uint32_t pend;
	uint32_t cnt;
	unsigned long t;
	int ret;

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH write not possible - no FLASH memory\n");
		return EINVAL;
	}

	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
		size = hc12mcu_target.flash_nb_size;
	else
		size = hc12mcu_target.flash_size;

	if (chunk < 2 || (chunk & 1) != 0 || (size % chunk) != 0 ||
	    chunk > hc12mcu_target.flash_sector)
	{
		error("invalid chunk size for FLASH write: %u\n",
		      (unsigned int)chunk);
		return EINVAL;
	}

	buf = malloc(size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}
	memset(buf, 0xff, (size_t)size);

	if (options.verbose)
	{
		printf("FLASH write: image file <%s>\n",
		       (const char *)file);
	}

	if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
		adc = hc12mcu_flash_read_address_nb;
	else if (options.flash_addr == HC12MEM_FLASH_ADDR_BANKED_LINEAR)
		adc = hc12mcu_flash_read_address_bl;
	else if (options.flash_addr == HC12MEM_FLASH_ADDR_BANKED_PPAGE)
		adc = hc12mcu_flash_read_address_bp;
	else
		adc = NULL;

	ret = srec_read(file, info, sizeof(info),
		buf, size, NULL, &addr_min, &addr_max, adc);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	if (options.verbose)
	{
		if (info[0] != '\0')
		{
			printf("FLASH write: image info <%s>\n",
			       (const char *)info);
		}
	}

	len = 0;
	for (i = 0; i < size;)
	{
		pend = i - (i % HCS12_FLASH_BANK_WINDOW_SIZE) +
			HCS12_FLASH_BANK_WINDOW_SIZE;

		end = i + HCS12_FLASH_BANK_WINDOW_SIZE;
		if (end > pend)
			end = pend;

		for (; i < end; i += sizeof(uint16_t))
		{
			if (uint16_be2host_from_buf(buf + i) != 0xffff)
				break;
		}
		if (i == end)
			continue;

		end = i + HCS12_FLASH_BANK_WINDOW_SIZE;
		if (end > pend)
			end = pend;

		for (j = i + sizeof(uint16_t); j < end; j += sizeof(uint16_t))
		{
			if (uint16_be2host_from_buf(buf + j) == 0xffff)
				break;
		}

		if (options.verbose)
		{
			if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
			{
				printf("FLASH write: address range <0x%04x-0x%04x> size <0x%04x>\n",
				       (unsigned int)(i + hc12mcu_target.flash_nb_base),
				       (unsigned int)(j - 1 + hc12mcu_target.flash_nb_base),
				       (unsigned int)(j - i));
			}
			else
			{
				printf("FLASH write: linear address range <0x%05x-0x%05x> size <0x%04x>\n",
				       (unsigned int)(i + hc12mcu_target.flash_linear_base),
				       (unsigned int)(j - 1 + hc12mcu_target.flash_linear_base),
				       (unsigned int)(j - i));
			}
		}

		len += j - i;
		i = j;
	}

	cnt = 0;
	t = progress_start("FLASH write: image");
	for (i = 0; i < size;)
	{
		pend = i - (i % hc12mcu_target.flash_sector) +
			hc12mcu_target.flash_sector;

		end = i + (uint32_t)chunk;
		if (end > pend)
			end = pend;

		for (; i < end; i += sizeof(uint16_t))
		{
			if (uint16_be2host_from_buf(buf + i) != 0xffff)
				break;
		}
		if (i == end)
			continue;

		end = i + (uint32_t)chunk;
		if (end > pend)
			end = pend;

		for (j = i + sizeof(uint16_t); j < end; j += sizeof(uint16_t))
		{
			if (uint16_be2host_from_buf(buf + j) == 0xffff)
				break;
		}

		ret = (*f)(i, buf + i, j - i);
		if (ret != 0)
		{
			free(buf);
			return ret;
		}

		cnt += j - i;
		progress_report(cnt, len);
		i = j;
	}
	progress_stop(t, "FLASH write: image", len);

	free(buf);
	return 0;
}


/*
 *  read target EEPROM
 *
 *  in:
 *    file - file name to write EEPROM data
 *  out:
 *    status code (errno-like)
 */

int hc12mcu_eeprom_read(const char *file, size_t chunk,
	int (*f)(uint16_t addr, void *buf, size_t size))
{
	int ret;
	uint8_t *buf;
	unsigned long t;
	uint32_t size;
	uint32_t i;

	size = hc12mcu_target.eeprom_size;
	if (size == 0)
	{
		error("EEPROM read not possible - no EEPROM memory\n");
		return EINVAL;
	}

	buf = malloc(size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	t = progress_start("EEPROM read: data");
	for (i = 0; i < size; i += (uint16_t)chunk)
	{
		ret = (*f)((uint16_t)(i + hc12mcu_target.eeprom_base), buf + i, chunk);
		if (ret != 0)
		{
			free(buf);
			return ret;
		}
		progress_report(i + chunk, size);
	}
	progress_stop(t, "EEPROM read: data", size);

	ret = srec_write(file, "EEPROM data",
		hc12mcu_target.eeprom_base, size, buf,
		hc12mcu_target.eeprom_base, NULL,
		!options.include_erased, options.srec_size);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	if (options.verbose)
	{
		printf("EEPROM read: data file <%s> written\n",
		       (const char *)file);
	}

	free(buf);
	return 0;
}


/*
 *  write target EEPROM
 *
 *  in:
 *    file - file name with data for programming
 *  out:
 *    status code (errno-like)
 */

int hc12mcu_eeprom_write(const char *file, size_t chunk,
	int (*f)(uint16_t addr, const void *buf, size_t size))
{
	uint16_t size;
	uint8_t *buf;
	char info[256];
	uint32_t addr_min;
	uint32_t addr_max;
	uint32_t len;
	uint32_t i;
	size_t j;
	uint32_t cnt;
	unsigned long t;
	int ret;

	size = (uint16_t)hc12mcu_target.eeprom_size;
	if (size == 0)
	{
		error("EEPROM write not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (chunk < 2 || (chunk & 1) != 0 || (size % chunk) != 0)
	{
		error("invalid chunk size for EEPROM write: %u\n",
		      (unsigned int)chunk);
		return EINVAL;
	}

	buf = malloc(size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}
	memset(buf, 0xff, (size_t)size);

	if (options.verbose)
	{
		printf("EEPROM write: image file <%s>\n",
		       (const char *)file);
	}

	ret = srec_read(file, info, sizeof(info),
		buf, size, NULL, &addr_min, &addr_max, hc12mcu_eeprom_address);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	len = 0;
	for (i = 0; i < size; i += (uint32_t)chunk)
	{
		for (j = 0; j < chunk; j += 4)
		{
			if (*((uint32_t *)&buf[i + j]) != 0xffffffff)
				break;
		}
		if (j == chunk)
			continue;
		len += chunk;
	}

	addr_min += hc12mcu_target.eeprom_base;
	addr_max += hc12mcu_target.eeprom_base;
	len = addr_max - addr_min + 1;

	if (options.verbose)
	{
		printf("EEPROM write: data info <%s>\n"
		       "EEPROM write: address range <0x%04x-0x%04x> length <0x%04x>\n",
		       (const char *)info,
		       (unsigned int)addr_min,
		       (unsigned int)addr_max,
		       (unsigned int)len);
	}

	if (addr_max >= hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - HC12_EEPROM_RESERVED_SIZE)
	{
		if (!options.force)
		{
			error("EEPROM data covers protected area. If this is intended, consider -f option.\n");
			return EINVAL;
		}
	}

	cnt = 0;
	t = progress_start("EEPROM write: data");
	for (i = 0; i < size; i += (uint32_t)chunk)
	{
		for (j = 0; j < chunk; j += 4)
		{
			if (*((uint32_t *)&buf[i + j]) != 0xffffffff)
				break;
		}
		if (j == chunk)
			continue;

		ret = (*f)((uint16_t)(i + hc12mcu_target.eeprom_base), buf + i, chunk);
		if (ret != 0)
		{
			free(buf);
			return ret;
		}

		cnt += (uint32_t)chunk;
		progress_report(cnt, len);
	}
	progress_stop(t, "EEPROM write: data", len);

	free(buf);
	return 0;
}


/*
 *  protect target EEPROM
 *
 *  in:
 *    opt - protection option string
 *  out:
 *    status code (errno-like)
 */

int hc12mcu_eeprom_protect(const char *opt, int (*eeww)(uint16_t addr, uint16_t v))
{
	int ret;
	int i;
	int n;
	int prot;
	uint8_t b;
	uint8_t eprot;
	uint16_t addr;
	uint8_t v;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM protect not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("EEPROM protect not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (strcmp(opt, "all") == 0)
	{
		b = 0xff & ~HCS12_IO_EPROT_EPOPEN;
		prot = -1;
	}
	else
	{
		n = sizeof(hcs12_eeprom_prot_area_table) /
		    sizeof(hcs12_eeprom_prot_area_table[0]);
		for (i = 0; i < n; ++ i)
		{
			if (strcasecmp(opt, hcs12_eeprom_prot_area_table[i].text) == 0)
				break;
		}
		if (i == n)
		{
			error("invalid EEPROM protection area: %s\n",
			      (const char *)opt);
			return EINVAL;
		}
		b = ((uint8_t)i | (uint8_t)(~HCS12_IO_EPROT_EP));
		b &= ~HCS12_IO_EPROT_EPDIS;
		prot = hcs12_eeprom_prot_area_table[i].size;
	}

	ret = (*hc12mcu_target.read_byte)(HCS12_IO_EPROT, &eprot);
	if (ret != 0)
		return ret;

	if (eprot != 0xff && !options.force)
	{
		error("EEPROM protection already set (-f option forces the operation)\n");
		return EINVAL;
	}

	addr = (uint16_t)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size -
		HC12_EEPROM_RESERVED_SIZE + HC12_EEPROM_RESERVED_EPROT_OFFSET);
	ret = (*eeww)((uint16_t)(addr & 0xfffe), (uint16_t)((uint16_t)0xff00 + (uint16_t)b));
	if (ret != 0)
		return ret;

	if (options.verbose)
	{
		if (eprot != 0xff && options.force)
			printf("EEPROM protect: protection already set - forced overwrite\n");
		if (prot == -1)
			printf("EEPROM protect: whole area\n");
		else
		{
			printf("EEPROM protect: address range <0x%04x-0x%04x> block size <%uB>\n",
			       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - prot),
			       (unsigned int)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - 1),
			       (unsigned int)prot);
		}
	}

	if (options.verify)
	{
		ret = (*hc12mcu_target.read_byte)(addr, &v);
		if (ret != 0)
			return ret;

		if (v != b)
		{
			error("EEPROM protection error: value 0x%02x, expected 0x%02x\n",
			      (unsigned int)v,
			      (unsigned int)b);
			return EIO;
		}

		if (options.verbose)
			printf("EEPROM protect: verify ok\n");
	}

	return 0;
}
