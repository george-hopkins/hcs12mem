/*
    hc12mem - HC12 memory reader & writer
    hc12bdm.c: HC12 BDM target access routines
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
#include "hc12bdm.h"
#include "bdm12pod.h"
#include "srec.h"
#include "../target/agent.h"


/* globals */

static hc12bdm_handler_t *hc12bdm_handler;
static uint32_t hc12bdm_ram_entry;
static int hc12bdm_agent_loaded;
static uint16_t hc12bdm_agent_param;
static uint16_t hc12bdm_agent_buf_addr;
static uint16_t hc12bdm_agent_buf_len;
static uint8_t hc12bdm_ppage;

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


/*
 *  wait for active BDM
 *
 *  in:
 *    timeout - wait timeout, milliseconds
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_wait_active(unsigned long timeout)
{
	unsigned long start;
	uint8_t b;
	int ret;

	start = sys_get_ms();
	while (sys_get_ms() - start < timeout)
	{
		ret = (*hc12bdm_handler->read_bd_byte)(
			HC12BDM_REG_STATUS, &b);
		if (ret != 0)
			return ret;

		if (b & HC12BDM_REG_STATUS_BDMACT)
			return 0;
	}

	error("BDM active wait timed out\n");
	return ETIMEDOUT;
}


/*
 *  initialize FLASH/EEPROM clock
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_clkdiv_init(void)
{
	unsigned long clk;
	uint8_t b;
	unsigned long div;
	int ret;

	if (options.osc < 2000000)
	{
		error("FLASH/EEPROM routines cannot work with oscillator below 2MHz\n");
		return EINVAL;
	}

	if (options.osc < 12800000)
	{
		b = 0;
		clk = options.osc;
	}
	else
	{
		b = HCS12_IO_FCLKDIV_PRDIV8;
		clk = options.osc / 8;
	}

	div = clk / HCS12_FCLK_MAX + 2;
	if ((clk % HCS12_FCLK_MAX) == 0)
		-- div;
	clk /= (1 + div);
	if (clk < HCS12_FCLK_MIN ||
	    ((options.osc + 2 * clk) / clk < options.osc / HCS12_FCLK_MAX))
	{
		error("unable to determine proper clock divider for FLASH/EEPROM programming\n");
		return EINVAL;
	}

	b |= (uint8_t)div;

	if (options.debug)
		printf("FCLK = %lu\n", (unsigned long)clk);

	ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FCLKDIV, b);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(HCS12_IO_ECLKDIV, b);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  initialize target connection
 *
 *  in:
 *    verbose - flag for showing target data info
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_init(int verbose)
{
	int ret;
	const char *ptr;
	char *end;
	unsigned long addr;
	unsigned long val;
	uint8_t bdm;
	uint16_t id;
	uint32_t v;

	/* reset target into single chip special mode */

	ret = (*hc12bdm_handler->reset_special)();
	if (ret != 0)
		return ret;

	/* delay for BDM firmware init */

	if (hc12mem_target_param("bdm_startup_delay", &v, 0) != 0)
		return EINVAL;
	sys_delay((unsigned long)v);

	/* get BDM status */

	ret = (*hc12bdm_handler->read_bd_byte)(HC12BDM_REG_STATUS, &bdm);
	if (ret != 0)
		return ret;

	if (verbose)
	{
		printf("BDM status <%s,%s> clock <%s>",
		       (const char *)((bdm & HC12BDM_REG_STATUS_ENBDM) ?
				"enabled" : "disabled"),
		       (const char *)((bdm & HC12BDM_REG_STATUS_BDMACT) ?
				"active" : "inactive"),
		       (const char *)((bdm & HC12BDM_REG_STATUS_CLKSW) ?
				"pll" : "bus"));
		if (hc12mcu_target.family >= HC12_FAMILY_HCS12)
		{
			printf(" security <%s>",
			       (const char *)((bdm & HC12BDM_REG_STATUS_UNSEC) ?
					"unsecured" : "secured"));
		}
		printf("\n");
	}

	if (hc12mcu_target.family >= HC12_FAMILY_HCS12)
	{
		ret = (*hc12bdm_handler->read_word)(HCS12_IO_PARTID, &id);
		if (ret != 0)
			return ret;

		hcs12mcu_partid(id, verbose);
	}

	hc12mem_target_info(NULL, TRUE);
	while ((ptr = hc12mem_target_info("bdm_init_byte", FALSE)) != NULL)
	{
		addr = strtoul(ptr, &end, 0);
		if (*end != ' ')
		{
		  err_param:
			error("invalid target bdm_init_byte parameter\n");
			return EINVAL;
		}
		val = strtoul(end + 1, &end, 0);
		if (*end != '\0')
			goto err_param;

		ret = (*hc12bdm_handler->write_byte)(
			(uint16_t)addr, (uint8_t)val);
		if (ret != 0)
			return ret;

	}

	if (hc12mcu_target.family >= HC12_FAMILY_HCS12)
	{
		ret = hc12bdm_clkdiv_init();
		if (ret != 0)
			return ret;
	}

	hc12mcu_identify(verbose);
	if (ret != 0)
		return ret;

	if (verbose)
		printf("\n");

	hc12bdm_agent_loaded = FALSE;

	return 0;
}


/*
 *  reset target into normal mode
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_reset(void)
{
	if (options.verbose)
		printf("reset: normal mode\n");

	return (*hc12bdm_handler->reset_normal)();
}


/*
 *  RAM address translation (for reading S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

static uint32_t hc12bdm_ram_address(uint32_t addr)
{
	if (addr < hc12mcu_target.ram_base ||
	    addr >= hc12mcu_target.ram_base + hc12mcu_target.ram_size)
		return hc12mcu_target.ram_size;
	return addr - hc12mcu_target.ram_base;
}


/*
 *  load data into RAM target
 *
 *  in:
 *    file - data file name to read
 *    agent - flag when loading target RAM agent
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_ram_load(const char *file, int agent)
{
	int ret;
	uint8_t *buf;
	char info[256];
	uint32_t entry;
	uint32_t addr_min;
	uint32_t addr_max;
	uint32_t len;
	uint32_t i;
	unsigned long t;

	if (!agent)
		hc12bdm_agent_loaded = FALSE;

	buf = malloc(hc12mcu_target.ram_size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	entry = 0xffffffff;

	if (options.verbose)
	{
		printf("RAM load: %s file <%s>\n",
		       (const char *)(agent ? "agent" : "image"),
		       (const char *)file);
	}

	ret = srec_read(file, info, sizeof(info), buf, hc12mcu_target.ram_size,
		&entry, &addr_min, &addr_max, hc12bdm_ram_address);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	if (entry == 0xffffffff && !options.start_valid)
	{
		error("start address not specified\n");
		free(buf);
		return EINVAL;
	}

	len = addr_max - addr_min + 1;
	addr_min += hc12mcu_target.ram_base;
	addr_max += hc12mcu_target.ram_base;
	hc12bdm_ram_entry = entry + hc12mcu_target.ram_base;

	if (options.verbose)
	{
		if (agent)
		{
			printf("RAM load: address range <0x%04x-0x%04x> length <0x%04x> start <0x%04x>\n",
			       (unsigned int)addr_min,
			       (unsigned int)addr_max,
			       (unsigned int)len,
			       (unsigned int)hc12bdm_ram_entry);
		}
		else
		{
			printf("RAM load: image info <%s>\n"
			       "RAM load: address range <0x%04x-0x%04x> length <0x%04x> start <0x%04x>\n",
			       (const char *)info,
			       (unsigned int)addr_min,
			       (unsigned int)addr_max,
			       (unsigned int)len,
			       (unsigned int)hc12bdm_ram_entry);
		}
	}

	t = progress_start("RAM load: data");
	for (i = 0; i < len; ++ i)
	{
		ret = (*hc12bdm_handler->write_byte)(
			(uint16_t)(addr_min + i),
			buf[addr_min - hc12mcu_target.ram_base + i]);
		if (ret != 0)
		{
			progress_stop(t, NULL, 0);
			free(buf);
			return ret;
		}

		progress_report(i + 1, len);
	}
	progress_stop(t, NULL, 0);

	if (agent)
		hc12bdm_agent_param = (uint16_t)addr_min;

	free(buf);
	return 0;
}


/*
 *  load code and run from target RAM
 *
 *  in:
 *    file - data file name to read
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_ram_run(const char *file)
{
	int ret;

	if (hc12mcu_target.secured && !options.force)
	{
		printf("RAM run: MCU secured (-f option forces loading and running the program)\n");
		return 0;
	}

	ret = hc12bdm_ram_load(file, FALSE);
	if (ret != 0)
		return ret;

	if (options.start_valid)
		hc12bdm_ram_entry = options.start;

	ret = (*hc12bdm_handler->write_reg)(
		HC12BDM_REG_PC, (uint16_t)hc12bdm_ram_entry);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->go)();
	if (ret != 0)
		return ret;

	if (options.verbose)
	{
		printf("RAM run: image loaded and started from adrress <0x%04x>\n",
		       (unsigned int)hc12bdm_ram_entry);
	}

	return 0;
}


/*
 *  perform operation on target EEPROM module
 *
 *  in:
 *    command - command to perform
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hc12_eeprom_command(uint8_t command)
{
	int ret;
	uint8_t b;
	unsigned long ms;

	ret = (*hc12bdm_handler->write_byte)(HCS12_IO_ECMD, command);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_ESTAT, HCS12_IO_ESTAT_CBEIF);
	if (ret != 0)
		return ret;

	ms = sys_get_ms();
	do
	{
		ret = (*hc12bdm_handler->read_byte)(HCS12_IO_ESTAT, &b);
		if (ret != 0)
			return ret;
	}
	while (!(b & (HCS12_IO_ESTAT_CCIF | HCS12_IO_ESTAT_PVIOL | HCS12_IO_ESTAT_ACCERR)) &&
	       (sys_get_ms() - ms < HCS12_EEPROM_CMD_TIMEOUT));

	if (b & HCS12_IO_ESTAT_PVIOL)
	{
		error("EEPROM protection violation (PVIOL bit set)\n");
		return EIO;
	}
	if (b & HCS12_IO_ESTAT_ACCERR)
	{
		error("EEPROM access error (ACCERR bit set)\n");
		return EIO;
	}
	if (!(b & HCS12_IO_ESTAT_CCIF))
	{
		error("EEPROM operation timed out\n");
		return ETIMEDOUT;
	}

	return 0;
}


/*
 *  mass erase target EEPROM module
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_eeprom_mass_erase(void)
{
	int ret;

	ret = (*hc12bdm_handler->write_byte)(HCS12_IO_EPROT, 0xff);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_ESTAT, HCS12_IO_ESTAT_PVIOL | HCS12_IO_ESTAT_ACCERR);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_EDATA, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_EADDR, 0);
	if (ret != 0)
		return ret;
	ret = hc12bdm_hc12_eeprom_command(HCS12_IO_ECMD_MASS_ERASE);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  verify erasure of target EEPROM module
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_eeprom_erase_verify(int *state)
{
	int ret;
	uint8_t b;

	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_ESTAT, HCS12_IO_ESTAT_PVIOL | HCS12_IO_ESTAT_ACCERR);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_EDATA, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_EADDR, 0);
	if (ret != 0)
		return ret;
	ret = hc12bdm_hc12_eeprom_command(HCS12_IO_ECMD_ERASE_VERIFY);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->read_byte)(HCS12_IO_ESTAT, &b);
	if (ret != 0)
		return ret;

	*state = (b & HCS12_IO_ESTAT_BLANK) ? TRUE : FALSE;
	return 0;
}


/*
 *  program 16-bit word into target EEPROM
 *
 *  in:
 *    addr - word address
 *    value - value to program
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_eeprom_program(uint16_t addr, uint16_t value)
{
	int ret;

	ret = (*hc12bdm_handler->write_word)(addr, value);
	if (ret != 0)
		return ret;

	ret = hc12bdm_hc12_eeprom_command(HCS12_IO_ECMD_PROGRAM);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  wait for FLASH operation completion
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hc12_flash_ccif_wait(void)
{
	int ret;
	uint8_t b;
	unsigned long ms;

	ms = sys_get_ms();
	do
	{
		ret = (*hc12bdm_handler->read_byte)(HCS12_IO_FSTAT, &b);
		if (ret != 0)
			return ret;
	}
	while (!(b & (HCS12_IO_FSTAT_CCIF | HCS12_IO_FSTAT_PVIOL | HCS12_IO_FSTAT_ACCERR)) &&
	       (sys_get_ms() - ms < HCS12_FLASH_CMD_TIMEOUT));

	if (b & HCS12_IO_FSTAT_PVIOL)
	{
		error("FLASH protection violation (PVIOL bit set)\n");
		return EIO;
	}
	if (b & HCS12_IO_FSTAT_ACCERR)
	{
		error("FLASH access error (ACCERR bit set)\n");
		return EIO;
	}
	if (!(b & HCS12_IO_FSTAT_CCIF))
	{
		error("FLASH operation timed out\n");
		return ETIMEDOUT;
	}

	return 0;
}


/*
 *  mass erase target FLASH
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_flash_mass_erase(void)
{
	int i;
	int ret;

	if (hc12mcu_target.flash_blocks > 1)
	{
		ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FCNFG, 0x00);
		if (ret != 0)
			return ret;
		ret = (*hc12bdm_handler->write_byte)(
			HCS12_IO_FTSTMOD, HCS12_IO_FTSTMOD_WRALL);
		if (ret != 0)
			return ret;
	}

	ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FPROT, 0xff);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FSTAT, HCS12_IO_FSTAT_PVIOL | HCS12_IO_FSTAT_ACCERR);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_FDATA, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_FADDR, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FCMD, HCS12_IO_FCMD_MASS_ERASE);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FSTAT, HCS12_IO_FSTAT_CBEIF);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.flash_blocks > 1)
	{
		ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FTSTMOD, 0);
		if (ret != 0)
			return ret;
	}

	for (i = 0; i < hc12mcu_target.flash_blocks; ++ i)
	{
		if (hc12mcu_target.flash_blocks > 1)
		{
			ret = (*hc12bdm_handler->write_byte)(
				HCS12_IO_FCNFG, (uint8_t)i);
			if (ret != 0)
				return ret;
		}

		ret = hc12bdm_hc12_flash_ccif_wait();
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  verify erasure of target FLASH
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_flash_erase_verify(int *state)
{
	int i;
	int ret;
	uint8_t b;

	if (hc12mcu_target.flash_blocks > 1)
	{
		ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FCNFG, 0x00);
		if (ret != 0)
			return ret;
		ret = (*hc12bdm_handler->write_byte)(
			HCS12_IO_FTSTMOD, HCS12_IO_FTSTMOD_WRALL);
		if (ret != 0)
			return ret;
	}

	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FSTAT, HCS12_IO_FSTAT_PVIOL | HCS12_IO_FSTAT_ACCERR);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_FDATA, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_word)(HCS12_IO_FADDR, 0);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FCMD, HCS12_IO_FCMD_ERASE_VERIFY);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FSTAT, HCS12_IO_FSTAT_CBEIF);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.flash_blocks > 1)
	{
		ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FTSTMOD, 0);
		if (ret != 0)
			return ret;
	}

	*state = TRUE;

	for (i = 0; i < hc12mcu_target.flash_blocks; ++ i)
	{
		if (hc12mcu_target.flash_blocks > 1)
		{
			ret = (*hc12bdm_handler->write_byte)(
				HCS12_IO_FCNFG, (uint8_t)i);
			if (ret != 0)
				return ret;
		}

		ret = hc12bdm_hc12_flash_ccif_wait();
		if (ret != 0)
			return ret;

		ret = (*hc12bdm_handler->read_byte)(HCS12_IO_FSTAT, &b);
		if (ret != 0)
			return ret;

		if (!(b & HCS12_IO_ESTAT_BLANK))
		{
			*state = FALSE;
			break;
		}
	}

	return 0;
}


/*
 *  program 16-bit word into target FLASH
 *
 *  in:
 *    addr - word address
 *    value - value to program
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_hcs12_flash_program(uint16_t addr, uint16_t value)
{
	int ret;

	ret = (*hc12bdm_handler->write_word)(addr, value);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FCMD, HCS12_IO_FCMD_PROGRAM);
	if (ret != 0)
		return ret;
	ret = (*hc12bdm_handler->write_byte)(
		HCS12_IO_FSTAT, HCS12_IO_FSTAT_CBEIF);
	if (ret != 0)
		return ret;
	ret = hc12bdm_hc12_flash_ccif_wait();
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  unsecure target
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_unsecure(void)
{
	int ret;

	if (hc12mcu_target.family < HC12_FAMILY_HCS12)
	{
		error("unsecuring procedure not available for this MCU family\n");
		return EINVAL;
	}

	if (!hc12mcu_target.secured && !options.force)
	{
		printf("unsecure: MCU not secured (-f option forces unsecuring procedure)\n");
		return 0;
	}

	if (hc12mcu_target.eeprom_size != 0)
	{
		ret = hc12bdm_hcs12_eeprom_mass_erase();
		if (ret != 0)
			return ret;

		if (options.verbose)
			printf("unsecure: EEPROM erased\n");
	}

	ret = hc12bdm_hcs12_flash_mass_erase();
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("unsecure: FLASH erased\n");

	ret = hc12bdm_init(FALSE);
	if (ret != 0)
		return ret;

	ret = hc12bdm_hcs12_flash_program(0xff0e, 0xfffe);
	if (ret != 0)
		return ret;

	ret = hc12bdm_init(FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("unsecure: MCU unsecured\n");

	return 0;
}


/*
 *  secure target
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_secure(void)
{
	int ret;

	if (hc12mcu_target.family < HC12_FAMILY_HCS12)
	{
		error("securing procedure not available for this family\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		printf("secure: MCU already secured (-f option forces securing procedure)\n");
		return 0;
	}

	ret = hc12bdm_hcs12_flash_program(0xff0e, 0xfffd);
	if (ret != 0)
		return ret;

	ret = hc12bdm_init(FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("secure: MCU secured\n");

	return 0;
}


/*
 *  execute command, using target RAM agent
 *
 *  in:
 *    cmd - command to execute
 *    status - on return, operation status
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_agent_cmd(int cmd, int *status)
{
	int ret;
	uint8_t b;

	ret = (*hc12bdm_handler->write_byte)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_CMD),
		(uint8_t)cmd);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_byte)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_STATUS),
		HC12_AGENT_ERROR_CMD);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_reg)(
		HC12BDM_REG_PC, (uint16_t)hc12bdm_ram_entry);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->go)();
	if (ret != 0)
		return ret;

	ret = hc12bdm_wait_active(HC12BDM_RUN_TIMEOUT);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->read_byte)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_STATUS), &b);
	if (ret != 0)
		return ret;

	if (b == HC12_AGENT_ERROR_XTAL)
	{
		error("unable to proceed - oscillator frequency invalid\n");
		return EINVAL;
	}
	if (b == HC12_AGENT_ERROR_CMD)
	{
		error("unable to proceed - command not supported\n");
		return ENOTSUP;
	}

	if (status != NULL)
		*status = (int)b;
	else if (b != HC12_AGENT_ERROR_NONE)
	{
		error("unable to proceed - unknown response\n");
		return EINVAL;
	}

	return 0;
}


/*
 *  load agent into target RAM
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_agent_load(void)
{
	const char *ptr;
	char file[SYS_MAX_PATH + 1];
	int ret;

	if (hc12bdm_agent_loaded)
		return 0;

	ptr = hc12mem_target_info("bdm_agent", TRUE);
	if (ptr == NULL)
	{
		error("target RAM agent not available, cannot proceed\n");
		return EINVAL;
	}

	if (access(ptr, R_OK) == -1 &&
	    strchr(ptr, SYS_PATH_SEPARATOR) == NULL)
	{
		snprintf(file, sizeof(file), "%s%c%s",
			 (const char *)hc12mem_data_dir,
			 (char)SYS_PATH_SEPARATOR,
			 (const char *)ptr);
	}
	else
		strlcpy(file, ptr, sizeof(file));

	ret = hc12bdm_ram_load(file, TRUE);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_word)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM),
		(uint16_t)(options.osc / 1000));
	if (ret != 0)
		return ret;

	ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_INIT, NULL);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->read_word)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
		&hc12bdm_agent_buf_addr);
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->read_word)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2),
		&hc12bdm_agent_buf_len);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("RAM agent data buffer address <0x%04x> length <0x%04x>\n",
		       (unsigned int)hc12bdm_agent_buf_addr,
		       (unsigned int)hc12bdm_agent_buf_len);
	}

	hc12bdm_agent_loaded = TRUE;

	return 0;
}


/*
 *  get mode of performing given operation, based on target info data
 *
 *  in:
 *    key - target info key name for querying target info data
 *    agent - on output, set to TRUE when operation is to be performed via RAM agent,
 *            FALSE when operation is to be performed directly via BDM
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_get_mode(const char *key, int *agent)
{
	const char *ptr;

	ptr = hc12mem_target_info(key, TRUE);
	if (ptr == NULL || strcmp(ptr, "bdm") == 0)
		*agent = FALSE;
	else if (strcmp(ptr, "agent") == 0)
		*agent = TRUE;
	else
	{
		error("invalid %s method: %s\n",
		      (const char *)key,
		      (const char *)ptr);
		return EINVAL;
	}

	return 0;
}


/*
 *  erase target EEPROM
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_eeprom_erase(void)
{
	int ret;
	int agent;
	int state;

	ret = hc12bdm_get_mode("bdm_eeprom_erase", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM erase not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("EEPROM erase not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;
		ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_EEPROM_MASS_ERASE, NULL);
		if (ret != 0)
			return ret;
	}
	else
	{
		ret = hc12bdm_hcs12_eeprom_mass_erase();
		if (ret != 0)
			return ret;
	}

	if (options.verbose)
		printf("EEPROM erase: memory erased\n");

	if (options.verify)
	{
		if (agent)
		{
			ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_EEPROM_ERASE_VERIFY, &state);
			if (ret != 0)
				return ret;

			if (state == HC12_AGENT_ERROR_NONE)
				state = TRUE;
			else if (state == HC12_AGENT_ERROR_VERIFY)
				state = FALSE;
			else
			{
				error("EEPROM erase verify failed - unknown response\n");
				return EINVAL;
			}
		}
		else
		{
			ret = hc12bdm_hcs12_eeprom_erase_verify(&state);
			if (ret != 0)
				return ret;
		}

		if (!state)
		{
			error("EEPROM erase verify failed - memory not clear\n");
			return EIO;
		}

		if (options.verbose)
			printf("EEPROM erase: verify ok\n");
	}

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

static int hc12bdm_eeprom_read(const char *file)
{
	int ret;
	uint8_t *buf;
	uint32_t i;
	uint32_t chunk;
	unsigned long t;
	int agent;

	ret = hc12bdm_get_mode("bdm_eeprom_read", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM read not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("EEPROM read not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;
	}

	buf = malloc(hc12mcu_target.eeprom_size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	/* single read chunk size */

	chunk = hc12mcu_target.eeprom_size / 128;

	/* read loop */

	t = progress_start("EEPROM read: data");
	for (i = 0; i < hc12mcu_target.eeprom_size; i += chunk)
	{
		if (agent)
		{
			/* param + 0: EEPROM address (word) */

			ret = (*hc12bdm_handler->write_word)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
				(uint16_t)(hc12mcu_target.eeprom_base + i));
			if (ret != 0)
				goto error_nl;

			/* param + 2: data length (word) */

			ret = (*hc12bdm_handler->write_word)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2),
				(uint16_t)chunk);
			if (ret != 0)
				goto error_nl;

			/* execute command via agent */

			ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_EEPROM_READ, NULL);
			if (ret != 0)
				goto error_nl;

			/* transfer returned data */

			ret = (*hc12bdm_handler->read_mem)(
				(uint16_t)hc12bdm_agent_buf_addr,
				buf + i, (uint16_t)chunk);
			if (ret != 0)
				goto error_nl;
		}
		else
		{
			/* read data from EEPROM directly */

			ret = (*hc12bdm_handler->read_mem)(
				(uint16_t)(hc12mcu_target.eeprom_base + i),
				buf + i, (uint16_t)chunk);
			if (ret != 0)
			{
			  error_nl:
				progress_stop(t, NULL, 0);
				free(buf);
				return ret;
			}
		}

		progress_report(i + chunk, hc12mcu_target.eeprom_size);
	}
	progress_stop(t, "EEPROM read: data", hc12mcu_target.eeprom_size);

	/* write S-record file */

	ret = srec_write(file, "EEPROM data", hc12mcu_target.eeprom_base,
		hc12mcu_target.eeprom_size, buf, hc12mcu_target.eeprom_base, NULL,
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
 *  EEPROM address translation (for reading S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

static uint32_t hc12bdm_eeprom_address(uint32_t addr)
{
	if (addr < hc12mcu_target.eeprom_base ||
	    addr >= hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size)
		return hc12mcu_target.eeprom_size;
	return addr - hc12mcu_target.eeprom_base;
}


/*
 *  write target EEPROM
 *
 *  in:
 *    file - file name with data to write
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_eeprom_write(const char *file)
{
	int ret;
	uint8_t *buf;
	char info[256];
	uint32_t addr_min;
	uint32_t addr_max;
	uint32_t len;
	uint32_t i;
	int cnt;
	unsigned long t;
	int agent;
	uint16_t w;

	ret = hc12bdm_get_mode("bdm_eeprom_write", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM write not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("EEPROM write not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;
	}

	buf = malloc(hc12mcu_target.eeprom_size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}
	memset(buf, 0xff, (size_t)hc12mcu_target.eeprom_size);

	if (options.verbose)
	{
		printf("EEPROM write: data file <%s>\n",
		       (const char *)file);
	}

	/* read EEPROM data from S-record file */

	ret = srec_read(file, info, sizeof(info),
		buf, hc12mcu_target.eeprom_size,
		NULL, &addr_min, &addr_max, hc12bdm_eeprom_address);
	if (ret != 0)
		goto error;

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

	/* align to word boundaries */

	if ((addr_min & 1) != 0)
	{
		-- addr_min;
		++ len;
	}
	if ((addr_max & 1) == 0)
	{
		++ addr_max;
		++ len;
	}

	/* write loop */

	cnt = 0;
	t = progress_start("EEPROM write: data");
	for (i = 0; i < len; i += 2)
	{
		w = uint16_be2host_buf(&buf[addr_min - hc12mcu_target.eeprom_base + i]);

		if (agent)
		{
			/* write word for programming */

			ret = (*hc12bdm_handler->write_word)(
				(uint16_t)(hc12bdm_agent_buf_addr + cnt), w);
			if (ret != 0)
				goto error_nl;

			cnt += 2;
			if (cnt == hc12bdm_agent_buf_len || i + 2 == len)
			{
				/* param + 0: EEPROM address (word) */

				ret = (*hc12bdm_handler->write_word)(
					(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
					(uint16_t)(addr_min + i + 2 - cnt));
				if (ret != 0)
					goto error_nl;

				/* param + 2: data length (word) */

				ret = (*hc12bdm_handler->write_word)(
					(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2),
					(uint16_t)cnt);
				if (ret != 0)
					goto error_nl;

				/* execute command via agent */

				ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_EEPROM_WRITE, NULL);
				if (ret != 0)
					goto error_nl;

				cnt = 0;
			}
		}
		else
		{
			ret = hc12bdm_hcs12_eeprom_program((uint16_t)(addr_min + i), w);
			if (ret != 0)
			{
			  error_nl:
				progress_stop(t, NULL, 0);
				goto error;
			}
		}

		progress_report(i + 2, len);
	}
	progress_stop(t, "EEPROM write: data", hc12mcu_target.eeprom_size);

	if (options.verify)
	{
		t = progress_start("EEPROM write: verify");
		for (i = 0; i < len; i += 2)
		{
			ret = (*hc12bdm_handler->read_word)(
				(uint16_t)(addr_min + i), &w);
			if (ret != 0)
			{
				progress_stop(t, NULL, 0);
				goto error;
			}

			if (w != uint16_be2host_buf(&buf[addr_min - hc12mcu_target.eeprom_base + i]))
			{
				progress_stop(t, NULL, 0);
				error("EEPROM data verify error at address <0x%04x> value <0x%04x> expected <0x%04x>\n",
				      (unsigned int)(addr_min + i),
				      (unsigned int)w,
				      (unsigned int)uint16_be2host_buf(&buf[addr_min - hc12mcu_target.eeprom_base + i]));
				goto error;
			}

			progress_report(i + 2, len);
		}
		progress_stop(t, "EEPROM write: verify", hc12mcu_target.eeprom_size);
		if (options.verbose)
			printf("EEPROM write: verify ok\n");
	}

	free(buf);
	return 0;

error:
	free(buf);
	return ret;
}


/*
 *  protect target EEPROM
 *
 *  in:
 *    opt - protection option string
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_eeprom_protect(const char *opt)
{
	int ret;
	int i;
	int n;
	int prot;
	uint8_t b;
	uint8_t eprot;
	uint8_t buf[HC12_EEPROM_RESERVED_SIZE];
	uint16_t addr;
	uint16_t w;
	int agent;

	ret = hc12bdm_get_mode("bdm_eeprom_protect", &agent);
	if (ret != 0)
		return ret;

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

	ret = (*hc12bdm_handler->read_byte)(HCS12_IO_EPROT, &eprot);
	if (ret != 0)
		return ret;

	if (eprot != 0xff && !options.force)
	{
		error("EEPROM protection already set\n");
		return EINVAL;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;
	}

	addr = (uint16_t)(hc12mcu_target.eeprom_base + hc12mcu_target.eeprom_size - HC12_EEPROM_RESERVED_SIZE);

	ret = (*hc12bdm_handler->read_mem)(
		addr, buf, HC12_EEPROM_RESERVED_SIZE);
	if (ret != 0)
		return ret;

	buf[HC12_EEPROM_RESERVED_EPROT_OFFSET] = b;

	if (agent)
	{
		ret = (*hc12bdm_handler->write_mem)(
			hc12bdm_agent_buf_addr, buf, HC12_EEPROM_RESERVED_SIZE);
		if (ret != 0)
			return ret;
		ret = (*hc12bdm_handler->write_word)(
			(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
			addr);
		if (ret != 0)
			return ret;
		ret = (*hc12bdm_handler->write_word)(
			(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2),
			HC12_EEPROM_RESERVED_SIZE);
		if (ret != 0)
			return ret;
		ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_EEPROM_WRITE, NULL);
		if (ret != 0)
			return ret;
	}
	else
	{
		w = uint16_be2host_buf(&buf[HC12_EEPROM_RESERVED_EPROT_OFFSET & 0xfffe]);
		ret = hc12bdm_hcs12_eeprom_program(
			(uint16_t)(addr + (HC12_EEPROM_RESERVED_EPROT_OFFSET & 0xfffe)), w);
 		if (ret != 0)
			return ret;
	}

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
		ret = (*hc12bdm_handler->read_byte)(
			(uint16_t)(addr + HC12_EEPROM_RESERVED_EPROT_OFFSET), &b);
		if (ret != 0)
			return ret;

		if (b == buf[HC12_EEPROM_RESERVED_EPROT_OFFSET])
		{
			if (options.verbose)
				printf("EEPROM protect: verify ok\n");
		}
		else
		{
			error("EEPROM protection error\n");
		}
	}

	return 0;
}


/*
 *  erase target FLASH
 *
 *  in:
 *    unsecure - flag for unsecuring part after FLASH erasure
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_flash_erase(int unsecure)
{
	int ret;
	int agent;
	int state;
	int i;

	ret = hc12bdm_get_mode("bdm_flash_erase", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH erase not possible - no FLASH memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("FLASH erase not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;

		for (i = 0; i < hc12mcu_target.flash_blocks; ++ i)
		{
			/* param + 0: FLASH block number (byte)
			   param + 1: PPAGE (byte) */

			ret = (*hc12bdm_handler->write_byte)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
				(uint8_t)i);
			if (ret != 0)
				return ret;

			ret = (*hc12bdm_handler->write_byte)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 1),
				hc12mcu_block_to_ppage_base(i));
			if (ret != 0)
				return ret;

			ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_FLASH_MASS_ERASE, NULL);
			if (ret != 0)
				return ret;

			if (options.verbose)
			{
				printf("FLASH erase: block #%u bulk erased\n",
				       (unsigned int)i);
			}
		}
	}
	else
	{
		ret = hc12bdm_hcs12_flash_mass_erase();
		if (ret != 0)
			return ret;
	}

	if (options.verbose)
		printf("FLASH erase: memory erased\n");

	if (options.verify)
	{
		if (agent)
		{
			for (i = 0; i < hc12mcu_target.flash_blocks; ++ i)
			{
				/* param + 0: FLASH block number (byte)
				   param + 1: PPAGE (byte) */

				ret = (*hc12bdm_handler->write_byte)(
					(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
					(uint8_t)i);
				if (ret != 0)
					return ret;

				ret = (*hc12bdm_handler->write_byte)(
					(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 1),
					hc12mcu_block_to_ppage_base(i));
				if (ret != 0)
					return ret;

				ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_FLASH_ERASE_VERIFY, &state);
				if (ret != 0)
					return ret;

				if (state == HC12_AGENT_ERROR_NONE)
					state = TRUE;
				else if (state == HC12_AGENT_ERROR_VERIFY)
					state = FALSE;
				else
				{
					error("FLASH erase verify failed - unknown response\n");
					return EINVAL;
				}

				if (options.verbose)
				{
					printf("FLASH erase: block #%u verified\n",
					       (unsigned int)i);
				}
			}
		}
		else
		{
			ret = hc12bdm_hcs12_flash_erase_verify(&state);
			if (ret != 0)
				return ret;
		}

		if (state)
		{
			if (options.verbose)
				printf("FLASH erase: verify ok\n");
		}
		else
		{
			error("FLASH erase verify failed - memory not clear\n");
			return EIO;
		}
	}

	if (unsecure)
	{
		if (agent)
		{
			ret = (*hc12bdm_handler->write_word)(
				hc12bdm_agent_buf_addr, 0xfffe);
			if (ret != 0)
				return ret;

			/* param + 0: FLASH block number (byte)
			   param + 1: PPAGE (byte)
			   param + 2: address (word)
			   param + 4: data length (word) */

			ret = (*hc12bdm_handler->write_byte)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0), 0);
			if (ret != 0)
				return ret;

			ret = (*hc12bdm_handler->write_byte)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 1),
				(uint8_t)(hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 1));
			if (ret != 0)
				return ret;

			ret = (*hc12bdm_handler->write_word)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2), 0xff0e);
			if (ret != 0)
				return ret;

			ret = (*hc12bdm_handler->write_word)(
				(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 4), 2);
			if (ret != 0)
				return ret;

			ret = hc12bdm_agent_cmd(HC12_AGENT_CMD_FLASH_WRITE, NULL);
			if (ret != 0)
				return ret;
		}
		else
		{
			ret = (*hc12bdm_handler->write_byte)(HCS12_IO_FCNFG, 0);
			if (ret != 0)
				return ret;

			ret = (*hc12bdm_handler->write_byte)(HCS12_IO_PPAGE,
				(uint8_t)(hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 1));
			if (ret != 0)
				return ret;

			ret = hc12bdm_hcs12_flash_program(0xff0e, 0xfffe);
			if (ret != 0)
				return ret;
		}

		if (options.verbose)
			printf("FLASH erase: security byte set to unsecure state\n");
	}

	return 0;
}


/*
 *  read or write FLASH via agent command
 *
 *  in:
 *    cmd - command to execute
 *    addr - linear address
 *    size - data size
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_agent_cmd_flash(uint8_t cmd, uint32_t addr, uint16_t size)
{
	int ret;

	/* param + 0: FLASH block number (byte)
	   param + 1: PPAGE (byte)
	   param + 2: address (word)
	   param + 4: data length (word) */

	ret = (*hc12bdm_handler->write_byte)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 0),
		hc12mcu_linear_to_block(addr));
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_byte)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 1),
		hc12mcu_linear_to_ppage(addr));
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_word)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 2),
		(uint16_t)(HCS12_FLASH_BANK_WINDOW_ADDR +
			   (addr % HCS12_FLASH_BANK_WINDOW_SIZE)));
	if (ret != 0)
		return ret;

	ret = (*hc12bdm_handler->write_word)(
		(uint16_t)(hc12bdm_agent_param + HC12_AGENT_PARAM + 4),
		(uint16_t)size);
	if (ret != 0)
		return ret;

	ret = hc12bdm_agent_cmd(cmd, NULL);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  set FLASH bank and ppage from linear address
 *
 *  in:
 *    addr - FLASH linear address
 *  out:
 *    status code (errno-like)
 */

static int hc12bdp_flash_set_bank_ppage(uint32_t addr)
{
	uint8_t p;
	int ret;

	if (hc12bdm_ppage == 0xff)
	{
		ret = (*hc12bdm_handler->read_byte)(
			HCS12_IO_PPAGE, &hc12bdm_ppage);
		if (ret != 0)
			return ret;
	}

	p = hc12mcu_linear_to_ppage(addr);
	if (p != hc12bdm_ppage)
	{
		ret = (*hc12bdm_handler->write_byte)(
			HCS12_IO_FCNFG,
			hc12mcu_linear_to_block(addr));
		if (ret != 0)
			return ret;

		ret = (*hc12bdm_handler->write_byte)(
			HCS12_IO_PPAGE, p);
		if (ret != 0)
			return ret;

		hc12bdm_ppage = p;
	}

	return 0;
}


/*
 *  read target FLASH
 *
 *  in:
 *    file - file name to write
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_flash_read(const char *file)
{
	int ret;
	uint8_t *buf;
	uint32_t i;
	uint32_t chunk;
	uint32_t size;
	unsigned long t;
	uint32_t (*adc)(uint32_t addr);
	int agent;

	ret = hc12bdm_get_mode("bdm_flash_read", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH read not possible - no FLASH memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("FLASH read not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;
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

	if (agent)
		chunk = hc12bdm_agent_buf_len;
	else
		chunk = 512;

	hc12bdm_ppage = 0xff; /* invalid ppage to start with, and force proper init */

	t = progress_start("FLASH read: image");
	for (i = 0; i < size; i += chunk)
	{
		if (agent)
		{
			ret = hc12bdm_agent_cmd_flash(
				HC12_AGENT_CMD_FLASH_READ, i, (uint16_t)chunk);
			if (ret != 0)
				goto error_nl;

			ret = (*hc12bdm_handler->read_mem)(
				(uint16_t)hc12bdm_agent_buf_addr,
				buf + i, (uint16_t)chunk);
			if (ret != 0)
				goto error_nl;
		}
		else
		{
			ret = hc12bdp_flash_set_bank_ppage(i);
			if (ret != 0)
				goto error_nl;

			ret = (*hc12bdm_handler->read_mem)(
				(uint16_t)(HCS12_FLASH_BANK_WINDOW_ADDR +
					   (i % HCS12_FLASH_BANK_WINDOW_SIZE)),
				buf + i, (uint16_t)chunk);
			if (ret != 0)
			{
			  error_nl:
				progress_stop(t, NULL, 0);
				free(buf);
				return ret;
			}
		}

		progress_report(i + chunk, size);
	}
	progress_stop(t, "FLASH read: image", size);

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
		printf("FLASH read: image file <%s> written\n",
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

static int hc12bdm_flash_write(const char *file)
{
	uint32_t (*adc)(uint32_t addr);
	uint32_t size;
	uint8_t *buf;
	char info[256];
	uint32_t addr_min;
	uint32_t addr_max;
	uint32_t len;
	uint32_t i, j;
	uint32_t chunk;
	uint32_t cnt;
	unsigned long t;
	uint16_t addr;
	uint16_t w;
	int ret;
	int agent;

	ret = hc12bdm_get_mode("bdm_flash_write", &agent);
	if (ret != 0)
		return ret;

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH write not possible - no FLASH memory\n");
		return EINVAL;
	}

	if (hc12mcu_target.secured && !options.force)
	{
		error("FLASH write not possible - MCU secured (-f option forces the operation)\n");
		return EIO;
	}

	if (agent)
	{
		ret = hc12bdm_agent_load();
		if (ret != 0)
			return ret;

		chunk = hc12bdm_agent_buf_len;
	}
	else
		chunk = 16;

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
		goto error;

	len = 0;
	for (i = 0; i < size; i += chunk)
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

	if (options.verbose)
	{
		if (info[0] != '\0')
		{
			printf("FLASH write: image info <%s>\n",
			       (const char *)info);
		}
#if 0
		if (options.flash_addr == HC12MEM_FLASH_ADDR_NON_BANKED)
		{
			printf("FLASH write: size <0x%04x> address range <0x%04x-0x%04x>\n",
			       (unsigned int)len,
			       (unsigned int)addr_min,
			       (unsigned int)addr_max);
		}
		else
#endif
		{
			printf("FLASH write: size <0x%04x> linear address range <0x%05x-0x%05x>\n",
			       (unsigned int)len,
			       (unsigned int)(addr_min + hc12mcu_target.flash_linear_base),
			       (unsigned int)(addr_max + hc12mcu_target.flash_linear_base));
		}
	}

	hc12bdm_ppage = 0xff; /* invalid ppage to start with, and force proper init */
	cnt = 0;
	t = progress_start("FLASH write: image");
	for (i = 0; i < size; i += chunk)
	{
		for (j = 0; j < chunk; j += 4)
		{
			if (*((uint32_t *)&buf[i + j]) != 0xffffffff)
				break;
		}
		if (j == chunk)
			continue;
		cnt += chunk;

		if (agent)
		{
			ret = (*hc12bdm_handler->write_mem)(
				hc12bdm_agent_buf_addr, buf + i, (uint16_t)chunk);
			if (ret != 0)
			{
			  error_nl:
				progress_stop(t, NULL, 0);
				goto error;
			}

			ret = hc12bdm_agent_cmd_flash(
				HC12_AGENT_CMD_FLASH_WRITE, i, (uint16_t)chunk);
			if (ret != 0)
				goto error_nl;
		}
		else
		{
			ret = hc12bdp_flash_set_bank_ppage(i);
			if (ret != 0)
				goto error_nl;

			for (j = 0; j < chunk; j += 2)
			{
				ret = hc12bdm_hcs12_flash_program(
					(uint16_t)(HCS12_FLASH_BANK_WINDOW_ADDR +
					((i + j) % HCS12_FLASH_BANK_WINDOW_SIZE)),
					uint16_be2host_buf(buf + i + j));
				if (ret != 0)
					goto error_nl;
			}
		}

		progress_report(cnt, len);
	}
	progress_stop(t, "FLASH write: image", len);

	if (options.verify)
	{
		hc12bdm_ppage = 0xff; /* invalid ppage to start with, and force proper init */
		cnt = 0;
		t = progress_start("FLASH write: verify");
		for (i = 0; i < size; i += chunk)
		{
			for (j = 0; j < chunk; j += 4)
			{
				if (*((uint32_t *)&buf[i + j]) != 0xffffffff)
					break;
			}
			if (j == chunk)
				continue;
			cnt += chunk;

			if (agent)
			{
				ret = hc12bdm_agent_cmd_flash(
					HC12_AGENT_CMD_FLASH_READ, i, (uint16_t)chunk);
				if (ret != 0)
				{
				  error_verify:
					progress_stop(t, NULL, 0);
					goto error;
				}

				addr = hc12bdm_agent_buf_addr;
			}
			else
			{
				ret = hc12bdp_flash_set_bank_ppage(i);
				if (ret != 0)
					goto error_nl;

				addr = (uint16_t)(HCS12_FLASH_BANK_WINDOW_ADDR +
					(i % HCS12_FLASH_BANK_WINDOW_SIZE));
			}

			for (j = 0; j < chunk; j += 2)
			{
				ret = (*hc12bdm_handler->read_word)((uint16_t)(addr + j), &w);
				if (ret != 0)
					goto error_verify;

				if (w != uint16_be2host_buf(buf + i + j))
				{
					progress_stop(t, NULL, 0);
					error("FLASH verify error at address <0x%04x> value <0x%04x> expected <0x%04x>\n",
					      (unsigned int)i,
					      (unsigned int)w,
					      (unsigned int)uint16_be2host_buf(buf +i + j));
					goto error;
				}
			}

			progress_report(cnt, len);
		}
		progress_stop(t, "FLASH write: verify", len);
		if (options.verbose)
			printf("FLASH write: verify ok\n");
	}

	free(buf);
	return 0;

error:
	free(buf);
	return ret;
}


/*
 *  protect target FLASH
 *
 *  in:
 *    opt - protection option string
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_flash_protect(const char *opt)
{
	return 0;
}


/*
 *  open BDM target connection
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_open(void)
{
	int ret;

	ret = (*hc12bdm_handler->open)();
	if (ret != 0)
		return ret;

	hc12mcu_target.read_byte = hc12bdm_handler->read_byte;
	hc12mcu_target.read_word = hc12bdm_handler->read_word;
	hc12mcu_target.write_byte = hc12bdm_handler->write_byte;
	hc12mcu_target.write_word = hc12bdm_handler->write_word;

	ret = hc12bdm_init(options.verbose);
	if (ret != 0)
	{
		(*hc12bdm_handler->close)();
		return ret;
	}

	return 0;
}


/*
 *  close BDM target connection
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm_close(void)
{
	return (*hc12bdm_handler->close)();
}


/*
 *  open BDM target connection via BDM12POD
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12bdm12pod_open(void)
{
	hc12bdm_handler = &bdm12pod_bdm_handler;
	return hc12bdm_open();
}


/* BDM handler for BDM12POD */

hc12mem_target_handler_t hc12mem_target_handler_bdm12pod =
{
	"bdm12pod",
	hc12bdm12pod_open,
	hc12bdm_close,
	hc12bdm_ram_run,
	hc12bdm_unsecure,
	hc12bdm_secure,
	hc12bdm_eeprom_read,
	hc12bdm_eeprom_erase,
	hc12bdm_eeprom_write,
	hc12bdm_eeprom_protect,
	hc12bdm_flash_read,
	hc12bdm_flash_erase,
	hc12bdm_flash_write,
	hc12bdm_flash_protect,
	hc12bdm_reset
};
