/*
    hc12mem - HC12 memory reader & writer
    hc12lrae.c: load ram and execute boot loader routines
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
#include "hc12lrae.h"
#include "hc12bdm.h"
#include "serial.h"
#include "srec.h"
#include "../target/agent.h"


/* globals */

static serial_t hc12lrae_serial;
static uint32_t hc12lrae_flash_size;
static uint32_t hc12lrae_ram_base;
static int hc12lrae_agent_loaded;

static const unsigned long hc12lrae_baud_table[] =
{
#ifdef SYS_TYPE_UNIX
#	ifdef B115200
	115200,
#	endif
#	ifdef B57600
	57600,
#	endif
	38400,
	19200,
	9600,
	4800,
	2400,
	1200
#endif /* SYS_TYPE_UNIX */
#ifdef SYS_TYPE_WIN32
	115200,
	57600,
	38400,
	28800,
	19200,
	14400,
	9600,
	7200,
	4800,
	2400,
	1200
#endif /* SYS_TYPE_WIN */
};

#define HC12LRAE_BAUD_TABLE_SIZE \
	(sizeof(hc12lrae_baud_table) / sizeof(hc12lrae_baud_table[0]))

static const unsigned long hc12lrae_prescaler_table[] =
{
	1, 2, 4, 9, 7, 13, 26, 52
};

#define HC12LRAE_PRESCALER_TABLE_SIZE \
	(sizeof(hc12lrae_prescaler_table) / sizeof(hc12lrae_prescaler_table[0]))


/*
 *  get best baud rate for given oscillator frequency
 *
 *  in:
 *    osc - target oscillator frequency
 *  out:
 *    selected baud rate, 0 when not found
 */

static unsigned long hc12lrae_get_baud(unsigned long osc)
{
	int i, j;
	int found_baud;
	int found_prescaler;
	unsigned long baud[HC12LRAE_PRESCALER_TABLE_SIZE];
	unsigned long e;
	unsigned int found_error;

	found_baud = -1;
	found_prescaler = -1;
	found_error = 0;
	for (i = 0; i < HC12LRAE_PRESCALER_TABLE_SIZE; ++ i)
	{
		baud[i] = osc / (2 * 16 * hc12lrae_prescaler_table[i]);
		for (j = 0; j < HC12LRAE_BAUD_TABLE_SIZE; ++ j)
		{
			if (baud[i] >= hc12lrae_baud_table[j])
				e = baud[i] - hc12lrae_baud_table[j];
			else
				e = hc12lrae_baud_table[j] - baud[i];
			e = e * 10000 / hc12lrae_baud_table[j];
			if (found_baud == -1 && e < HC12LRAE_BAUD_ERROR_LIMIT)
			{
				found_error = (unsigned int)((e + 5) / 10);
				found_baud = j;
				found_prescaler = i;
			}
			if (options.debug)
			{
				printf("target <%lu bps> local <%lu bps> error <%u.%u%%>%s\n",
				       (unsigned long)baud[i],
				       (unsigned long)hc12lrae_baud_table[j],
				       (unsigned int)(((e + 5) / 10) / 10),
				       (unsigned int)(((e + 5) / 10) % 10),
				       (const char *)(e <= HC12LRAE_BAUD_ERROR_LIMIT ? " <ok>" : ""));
			}
		}
	}

	if (found_baud == -1)
	{
		error("satisfactory baud rate not found\n"
		      "Consider specifying one with -b option\n");
		return 0;
	}

	if (options.verbose)
	{
		printf("LRAE selected local baud rate <%lu bps> target <%lu bps> error <%u.%u%%>\n",
		       (unsigned long)hc12lrae_baud_table[found_baud],
		       (unsigned long)baud[found_prescaler],
		       (unsigned int)(found_error / 10),
		       (unsigned int)(found_error % 10));
	}

	return hc12lrae_baud_table[found_baud];
}


/*
 *  open connection with target via LRAE bootloader
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_open(void)
{
	serial_cfg_t cfg;
	uint8_t b;
	size_t size;
	int i;
	int ret;

	if (hc12mem_target_info("lrae_agent", TRUE) == NULL)
	{
		error("target LRAE RAM agent not available, cannot proceed\n");
		return EINVAL;
	}

	if (hc12mem_target_param("lrae_size", &hc12lrae_flash_size, 0) != 0)
		return EINVAL;
	if (hc12lrae_flash_size == 0)
	{
		error("target LRAE FLASH usage unknown (missing lrae_size)\n");
		return EINVAL;
	}

	if (options.port == NULL)
	{
		error("serial port not specified (-p option required)\n");
		return EINVAL;
	}

	if (options.osc == 0)
	{
		error("oscillator frequency not specified (-o option required)\n");
		return EINVAL;
	}

	if (options.baud == 0)
	{
		options.baud = hc12lrae_get_baud(options.osc);
		if (options.baud == 0)
			return EINVAL;
	}

	ret = serial_open(&hc12lrae_serial, options.port);
	if (ret != 0)
		return ret;

	cfg.baud_rate = options.baud;
	cfg.char_size = SERIAL_CFG_CHAR_SIZE_8;
	cfg.parity = SERIAL_CFG_PARITY_NONE;
	cfg.stop_bits = SERIAL_CFG_STOP_BITS_1;
	cfg.handshake = SERIAL_CFG_HANDSHAKE_NONE;

	ret = serial_set_cfg(&hc12lrae_serial, &cfg);
	if (ret != 0)
	{
		serial_close(&hc12lrae_serial);
		return ret;
	}

	if (options.verbose)
	{
		printf("LRAE serial port <%s> baud rate <%lu bps>\n",
		       (const char *)options.port,
		       (unsigned long)options.baud);
	}

	for (i = 0; i < HC12LRAE_SYNC_RETRIES; ++ i)
	{
		b = HC12LRAE_SYNC_MSG;
		size = 1;
		ret = serial_write(&hc12lrae_serial, &b, &size, HC12LRAE_TX_TIMEOUT);
		if (ret != 0)
		{
			serial_close(&hc12lrae_serial);
			return ret;
		}

		size = 1;
		ret = serial_read(&hc12lrae_serial, &b, &size, HC12LRAE_SYNC_TIMEOUT);
		if (ret == ETIMEDOUT)
			continue;
		if (ret != 0)
		{
			serial_close(&hc12lrae_serial);
			return ret;
		}

		if (b == HC12LRAE_SYNC_ACK)
			break;
	}

	if (i == HC12LRAE_SYNC_RETRIES)
	{
		error("no connection with target\n");
		serial_close(&hc12lrae_serial);
		return ETIMEDOUT;
	}

	if (options.verbose)
		printf("LRAE target synchronized\n");

	if (options.verbose)
	{
		printf("target info <%s>\n"
		       "target mcu <%s> family <%s> osc <%lu.%06lu MHz>\n",
		       (const char *)hc12mcu_target.info_str,
		       (const char *)hc12mcu_target.mcu_str,
		       (const char *)hc12mcu_target.family_str,
		       (unsigned long)(options.osc / 1000000),
		       (unsigned long)(options.osc % 1000000));
	}

	hc12lrae_ram_base = HC12LRAE_RAM_END + 1 - hc12mcu_target.ram_size;
	if (options.verbose)
	{
		printf("target LRAE RAM area <0x%04x-0x%04x> size <0x%04x>\n",
		       (unsigned int)hc12lrae_ram_base,
		       (unsigned int)HC12LRAE_RAM_TOP,
		       (unsigned int)(HC12LRAE_RAM_TOP - hc12lrae_ram_base + 1));
	}

	hc12lrae_agent_loaded = FALSE;

	return 0;
}


/*
 *  close connection with target via LRAE bootloader
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_close(void)
{
	return serial_close(&hc12lrae_serial);
}


/*
 *  RAM address translation (for reading S-record file)
 *
 *  in:
 *    addr - address to translate
 *  out:
 *    translated address
 */

static uint32_t hc12lrae_ram_address(uint32_t addr)
{
	if (addr < hc12lrae_ram_base ||
	    addr >= hc12lrae_ram_base + hc12mcu_target.ram_size)
		return hc12mcu_target.ram_size; /* out of range value as error mark */
	return addr - hc12lrae_ram_base;
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

static int hc12lrae_ram_load(const char *file, int agent)
{
	int ret;
	uint8_t *buf;
	char info[256];
	uint32_t entry;
	uint32_t addr_min;
	uint32_t addr_max;
	uint8_t h[4];
	uint8_t b;
	uint8_t sum;
	uint32_t len;
	uint32_t i;
	unsigned long t;
	size_t size;

	buf = malloc(hc12mcu_target.ram_size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	if (options.verbose)
	{
		printf("RAM load: %s file <%s>\n",
		       (const char *)(agent ? "agent" : "image"),
		       (const char *)file);
	}

	entry = 0xffffffff;
	ret = srec_read(file, info, sizeof(info), buf, hc12mcu_target.ram_size,
		&entry, &addr_min, &addr_max, hc12lrae_ram_address);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	if (entry == 0xffffffff)
	{
		error("start address not specified\n");
		free(buf);
		return EINVAL;
	}

	len = addr_max - addr_min + 1;
	addr_min += hc12lrae_ram_base;
	addr_max += hc12lrae_ram_base;
	entry += hc12lrae_ram_base;

	if (options.verbose)
	{
		if (agent)
		{
			printf("RAM load: address range <0x%04x-0x%04x> length <0x%04x> start <0x%04x>\n",
			       (unsigned int)addr_min,
			       (unsigned int)addr_max,
			       (unsigned int)len,
			       (unsigned int)entry);
		}
		else
		{
			printf("RAM load: image info <%s>\n"
			       "RAM load: address range <0x%04x-0x%04x> length <0x%04x> start <0x%04x>\n",
			       (const char *)info,
			       (unsigned int)addr_min,
			       (unsigned int)addr_max,
			       (unsigned int)len,
			       (unsigned int)entry);
		}
	}

	if ((addr_min < hc12lrae_ram_base || addr_min > HC12LRAE_RAM_TOP) ||
	    (addr_max < hc12lrae_ram_base || addr_max > HC12LRAE_RAM_TOP))
	{
		if (!options.force)
		{
			error("RAM address outside allowable range\n");
			return EINVAL;
		}
	}

	uint16_host2be_to_buf(h + 0, (uint16_t)entry);
	uint16_host2be_to_buf(h + 2, (uint16_t)len);
	sum = h[0] + h[1] + h[2] + h[3];

	size = sizeof(h);
	ret = serial_write(&hc12lrae_serial, h, &size, HC12LRAE_TX_TIMEOUT);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	t = progress_start("RAM load: data");
	for (i = 0; i < len; ++ i)
	{
		b = buf[addr_min - hc12lrae_ram_base + i];
		sum += b;

		size = 1;
		ret = serial_write(&hc12lrae_serial, &b, &size, HC12LRAE_TX_TIMEOUT);
		if (ret != 0)
		{
			free(buf);
			return ret;
		}

		progress_report(i + 1, len);
	}
	progress_stop(t, NULL, 0);

	free(buf);

	size = 1;
	ret = serial_write(&hc12lrae_serial, &sum, &size, HC12LRAE_TX_TIMEOUT);
	if (ret != 0)
		return ret;

	size = 1;
	ret = serial_read(&hc12lrae_serial, &b, &size, HC12LRAE_CHECKSUM_TIMEOUT);
	if (ret == ETIMEDOUT)
	{
		error("checksum acknowledge reception timed out\n"
		      "Please reset target and try again\n");
		return ret;
	}
	if (ret != 0)
		return ret;
	if (b != HC12LRAE_CHECKSUM_ACK)
	{
		error("invalid checksum acknowledge received\n"
		      "Please reset target and try again\n");
		return EINVAL;
	}

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

static int hc12lrae_ram_run(const char *file)
{
	int ret;

	ret = hc12lrae_ram_load(file, FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("RAM run: image loaded and started\n");

	return 0;
}


/*
 *  send data to target
 *
 *  in:
 *    buf - data buffer
 *    len - data length
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_tx(const void *buf, size_t len)
{
	return serial_write(&hc12lrae_serial, buf, &len,
		(unsigned long)(len * HC12LRAE_TX_TIMEOUT));
}


/*
 *  receive data from target
 *
 *  in:
 *    buf - data buffer
 *    len - data length
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_rx(void *buf, size_t len)
{
	int ret;

	ret = serial_read(&hc12lrae_serial, buf, &len,
		HC12LRAE_AGENT_TIMEOUT * 10);
	if (ret == ETIMEDOUT)
	{
		error("timeout - no connection with target\n");
		return ret;
	}

	return 0;
}


/*
 *  calculate data block sum
 *
 *  in:
 *    b - data buffer
 *    n - data size
 *  out:
 *    sum
 */

static uint8_t hc12lrae_sum(const uint8_t *b, int n)
{
	int i;
	uint8_t sum;

	sum = 0;
	for (i = 0; i < n; ++ i)
		sum += b[i];

	return sum;
}


/*
 *  send command to target
 *
 *  in:
 *    cmd - command
 *    param - parameter block
 *    n - parameter block size
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_cmd(uint8_t cmd, const uint8_t *param, size_t n)
{
	int ret;
	uint8_t b;
	uint8_t sum;

	ret = hc12lrae_tx(&cmd, 1);
	if (ret != 0)
		return ret;

	b = (uint8_t)(n + 3);
	ret = hc12lrae_tx(&b, 1);
	if (ret != 0)
		return ret;

	sum = cmd + b + hc12lrae_sum(param, n);

	for (; n > 0; --n)
	{
		ret = hc12lrae_tx(param++, 1);
		if (ret != 0)
			return ret;
	}

	ret = hc12lrae_tx(&sum, 1);
	if (ret != 0)
		return ret;

	ret = hc12lrae_rx(&b, 1);
	if (ret != 0)
		return ret;

	if (b == HC12_AGENT_ERROR_SUM)
	{
		error("communication failed, invalid checksum\n");
		return EIO;
	}
	if (b != HC12_AGENT_ERROR_NONE)
	{
		error("communication failed, unexpected answer\n");
		return EIO;
	}

	return 0;
}


/*
 *  get acknowledge from target
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_ack(void)
{
	uint8_t b;
	int ret;

	ret = hc12lrae_rx(&b, 1);
	if (ret != 0)
		return ret;

	if (b != HC12_AGENT_ERROR_NONE)
	{
		error("communication failed, invalid response received\n");
		return EIO;
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

static int hc12lrae_load_agent(void)
{
	const char *ptr;
	char file[SYS_MAX_PATH + 1];
	int ret;
	uint8_t osc[2];
	uint8_t b;

	if (hc12lrae_agent_loaded)
		return 0;

	ptr = hc12mem_target_info("lrae_agent", TRUE);
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

	ret = hc12lrae_ram_load(file, TRUE);
	if (ret != 0)
		return ret;

	uint16_host2be_to_buf(osc, (uint16_t)(options.osc / 1000));

	ret = hc12lrae_tx(osc, sizeof(osc));
	if (ret != 0)
		return ret;

	ret = hc12lrae_rx(&b, 1);
	if (ret != 0)
		return ret;

	if (b == HC12_AGENT_ERROR_XTAL)
	{
		error("invalid oscillator frequency for target programming\n");
		return EINVAL;
	}
	if (b != HC12_AGENT_ERROR_NONE)
	{
		error("unknown response\n");
		return EINVAL;
	}

	hc12lrae_agent_loaded = TRUE;

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

static int hc12lrae_eeprom_erase(void)
{
	int ret;
	uint8_t b;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM erase not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (options.verbose)
		printf("EEPROM erase: not supported\n");

	return 0;

	ret = hc12lrae_load_agent();
	if (ret != 0)
		return ret;

	b = HC12_AGENT_CMD_EEPROM_MASS_ERASE;
	ret = hc12lrae_tx(&b, 1);
	if (ret != 0)
		return ret;

	ret = hc12lrae_ack();
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("EEPROM erase: memory erased %x\n", b);

	if (options.verify)
	{
		b = HC12_AGENT_CMD_EEPROM_ERASE_VERIFY;
		ret = hc12lrae_tx(&b, 1);
		if (ret != 0)
			return ret;

		ret = hc12lrae_rx(&b, 1);
		if (ret != 0)
			return ret;

		if (b == HC12_AGENT_ERROR_NONE)
		{
			if (options.verbose)
				printf("EEPROM erase: verify ok\n");
		}
		else
		{
			if (b == HC12_AGENT_ERROR_VERIFY)
				error("EEPROM erase verify failed - memory not clear\n");
			else
				error("unknown response\n");
			return EIO;
		}
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

static int hc12lrae_eeprom_read(const char *file)
{
	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM read not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (options.verbose)
		printf("EEPROM read: not supported\n");

	return 0;
}


/*
 *  write target EEPROM
 *
 *  in:
 *    file - file name with data to write
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_eeprom_write(const char *file)
{
	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM write not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (options.verbose)
		printf("EEPROM write: not supported\n");

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

static int hc12lrae_eeprom_protect(const char *opt)
{
	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM protect not possible - no EEPROM memory\n");
		return EINVAL;
	}

	if (options.verbose)
		printf("EEPROM protect: not supported\n");

	return 0;
}


/*
 *  write single FLASH word
 *
 *  in:
 *    block, ppage, addr - address of word to write
 *    value - word value to write
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_flash_write_word(uint8_t block, uint8_t ppage, uint16_t addr, uint16_t value)
{
	uint8_t cmd[6];
	uint8_t w[2];
	uint8_t b;
	int ret;

	cmd[0] = block;
	cmd[1] = ppage;
	uint16_host2be_to_buf(cmd + 2, addr);
	uint16_host2be_to_buf(cmd + 4, 2);

	ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_WRITE, cmd, sizeof(cmd));
	if (ret != 0)
		return ret;

	uint16_host2be_to_buf(w, value);
	ret = hc12lrae_tx(w, sizeof(w));
	if (ret != 0)
		return ret;

	b = w[0] + w[1];
	ret = hc12lrae_tx(&b, 1);
	if (ret != 0)
		return ret;

	ret = hc12lrae_rx(&b, 1);
	if (ret != 0)
		return ret;

	if (b != HC12_AGENT_ERROR_NONE)
	{
		if (b == HC12_AGENT_ERROR_SUM)
			error("communication failed, checksum error\n");
		else
			error("invalid response\n");
		return EIO;
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

static int hc12lrae_flash_erase(int unsecure)
{
	int ret;
	uint8_t b;
	uint32_t i;
	uint8_t cmd[4];
	unsigned long t;
	uint32_t block_start;

	ret = hc12lrae_load_agent();
	if (ret != 0)
		return ret;

	block_start = 0;

	if (options.keep_lrae)
	{
		t = progress_start("FLASH erase: block #0");
		for (i = 0; i < hc12mcu_target.flash_block_size;
		     i += hc12mcu_target.flash_sector)
		{
			cmd[0] = hc12mcu_linear_to_block(i);
			cmd[1] = hc12mcu_linear_to_ppage(i);
			uint16_host2be_to_buf(cmd + 2, (uint16_t)
				(HCS12_FLASH_BANK_WINDOW_ADDR +
				(i % HCS12_FLASH_BANK_WINDOW_SIZE)));

			if (cmd[1] == (uint8_t)(hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 2) &&
			    (i % HCS12_FLASH_BANK_WINDOW_SIZE) < hc12lrae_flash_size)
			{
				/* skip LRAE area */
				if (options.debug)
					printf("skip LRAE area: 0x%04x\n",
					       (unsigned int)i);
			}
			else
			{
				ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_ERASE_SECTOR, cmd, 4);
				if (ret != 0)
					return ret;

				ret = hc12lrae_rx(&b, 1);
				if (ret != 0)
					return ret;

				if (b != HC12_AGENT_ERROR_NONE)
				{
					error("invalid response\n");
					return EIO;
				}
			}
			progress_report(i + hc12mcu_target.flash_sector,
				hc12mcu_target.flash_size);
		}
		progress_stop(t, NULL, 0);

		ret = hc12lrae_flash_write_word(0, (uint8_t)
			(hc12mcu_target.ppage_base +
			hc12mcu_target.ppage_count - 1),
			0xfffe, HC12LRAE_FLASH_START);
		if (ret != 0)
			return ret;

		if (options.verbose)
			printf("FLASH erase: LRAE retained, start vector restored\n");

		block_start = 1;
		unsecure = TRUE;
	}

	for (i = block_start; i < (uint32_t)hc12mcu_target.flash_blocks; ++ i)
	{
		cmd[0] = (uint8_t)i;
		cmd[1] = hc12mcu_block_to_ppage_base(i);
		ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_MASS_ERASE, cmd, 2);
		if (ret != 0)
			return ret;

		ret = hc12lrae_ack();
		if (ret != 0)
			return ret;

		if (options.verbose)
		{
			printf("FLASH erase: block #%u bulk erased\n",
			       (unsigned int)i);
		}

		if (!options.verify)
			continue;

		cmd[0] = (uint8_t)i;
		cmd[1] = hc12mcu_block_to_ppage_base(i);
		ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_ERASE_VERIFY, cmd, 2);
		if (ret != 0)
			return ret;

		ret = hc12lrae_rx(&b, 1);
		if (ret != 0)
			return ret;

		if (b == HC12_AGENT_ERROR_NONE)
		{
			if (options.verbose)
			{
				printf("FLASH erase: block #%u verified\n",
				       (unsigned int)i);
			}
		}
		else
		{
			if (b == HC12_AGENT_ERROR_VERIFY)
				error("FLASH erase verify failed - memory not clear\n");
			else
				error("unknown response\n");
			return EIO;
		}
	}

	if (options.verbose)
		printf("FLASH erase: memory erased\n");

	if (unsecure)
	{
		ret = hc12lrae_flash_write_word(0, (uint8_t)
			(hc12mcu_target.ppage_base +
			hc12mcu_target.ppage_count - 1),
			0xff0e, 0xfffe);
		if (ret != 0)
			return ret;

		if (options.verbose)
			printf("FLASH erase: security byte set to unsecure state\n");
	}

	return 0;
}


/*
 *  FLASH read callback
 *
 *  in:
 *    addr - FLASH linear address
 *    size - block size
 *    buf - data buffer
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_flash_read_cb(uint32_t addr, void *buf, size_t size)
{
	int ret;
	uint8_t cmd[5];
	uint8_t b;

	if ((addr % HCS12_FLASH_BANK_WINDOW_SIZE) == 0)
	{
		cmd[0] = hc12mcu_linear_to_block(addr);
		cmd[1] = hc12mcu_linear_to_ppage(addr);
		uint16_host2be_to_buf(cmd + 2, HCS12_FLASH_BANK_WINDOW_ADDR);
		uint16_host2be_to_buf(cmd + 4, HCS12_FLASH_BANK_WINDOW_SIZE);

		ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_READ, cmd, sizeof(cmd));
		if (ret != 0)
			return ret;
	}

	ret = hc12lrae_rx(buf, 1);
	if (ret != 0)
		return ret;

	if (((addr + 1) % HCS12_FLASH_BANK_WINDOW_SIZE) == 0)
	{
		ret = hc12lrae_rx(&b, 1);
		if (ret != 0)
			return ret;

		if (hc12lrae_sum((uint8_t *)buf -
			(addr % HCS12_FLASH_BANK_WINDOW_SIZE),
			HCS12_FLASH_BANK_WINDOW_SIZE) != b)
		{
			error("invalid checksum received\n");
			return ret;
		}
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

static int hc12lrae_flash_read(const char *file)
{
	int ret;

	ret = hc12lrae_load_agent();
	if (ret != 0)
		return ret;

	ret = hc12mcu_flash_read(file, 1, hc12lrae_flash_read_cb);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  FLASH write callback
 *
 *  in:
 *    addr - FLASH linear address
 *    size - block size
 *    buf - data buffer
 *  out:
 *    status code (errno-like)
 */

static int hc12lrae_flash_write_cb(uint32_t addr, const void *buf, size_t size)
{
	int ret;
	uint8_t cmd[6];
	uint8_t b;

	cmd[0] = hc12mcu_linear_to_block(addr);
	cmd[1] = hc12mcu_linear_to_ppage(addr);
	uint16_host2be_to_buf(cmd + 2, (uint16_t)hc12mcu_flash_addr_window(addr));
	uint16_host2be_to_buf(cmd + 4, (uint16_t)size);

	ret = hc12lrae_cmd(HC12_AGENT_CMD_FLASH_WRITE, cmd, sizeof(cmd));
	if (ret != 0)
		return ret;

	ret = hc12lrae_tx(buf, size);
	if (ret != 0)
		return ret;

	b = hc12lrae_sum(buf, size);
	ret = hc12lrae_tx(&b, 1);
	if (ret != 0)
		return ret;

	ret = hc12lrae_rx(&b, 1);
	if (ret != 0)
		return ret;

	if (b != HC12_AGENT_ERROR_NONE)
	{
		if (b == HC12_AGENT_ERROR_SUM)
			error("communication failed, checksum error\n");
		else
			error("invalid response\n");
		return ret;
	}

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

static int hc12lrae_flash_write(const char *file)
{
	int ret;

	ret = hc12lrae_load_agent();
	if (ret != 0)
		return ret;

	ret = hc12mcu_flash_write(file, HC12LRAE_BUFFER_SIZE, hc12lrae_flash_write_cb);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  unsupported operations
 */

static int hc12lrae_unsecure(void)
{
	error("unsecure: operation not supported\n");
	return EINVAL;
}


static int hc12lrae_secure(void)
{
	error("secure: operation not supported\n");
	return EINVAL;
}


static int hc12lrae_reset(void)
{
	error("reset: operation not supported\n");
	return EINVAL;
}


/* handler for LRAE boot-loader */

hc12mem_target_handler_t hc12mem_target_handler_lrae =
{
	"lrae",
	hc12lrae_open,
	hc12lrae_close,
	hc12lrae_ram_run,
	hc12lrae_unsecure,
	hc12lrae_secure,
	hc12lrae_eeprom_read,
	hc12lrae_eeprom_erase,
	hc12lrae_eeprom_write,
	hc12lrae_eeprom_protect,
	hc12lrae_flash_read,
	hc12lrae_flash_erase,
	hc12lrae_flash_write,
	NULL,
	hc12lrae_reset
};
