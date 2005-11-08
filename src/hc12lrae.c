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
#include "hc12lrae.h"
#include "hc12bdm.h"
#include "serial.h"
#include "srec.h"
#include "../target/agent.h"


/* globals */

static serial_t hc12lrae_serial;
static uint32_t hc12lrae_ram_base;
static uint32_t hc12lrae_ram_size;
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
	int i;
	int ret;

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

	if (hc12mem_target_param("lrae_ram_base", &hc12lrae_ram_base, 0xffffffff) != 0)
		return EINVAL;
	if (hc12mem_target_param("lrae_ram_size", &hc12lrae_ram_size, 0) != 0)
		return EINVAL;
	if (hc12lrae_ram_base == 0xffffffff || hc12lrae_ram_size == 0)
	{
		error("unknown target RAM address and size for LRAE operation\n");
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
		ret = serial_write(&hc12lrae_serial, &b, 1, HC12LRAE_TX_TIMEOUT);
		if (ret != 0)
		{
			serial_close(&hc12lrae_serial);
			return ret;
		}

		ret = serial_read(&hc12lrae_serial, &b, 1, HC12LRAE_SYNC_TIMEOUT);
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


static uint32_t hc12lrae_ram_address(uint32_t addr)
{
	if (addr < hc12lrae_ram_base ||
	    addr >= hc12lrae_ram_base + hc12lrae_ram_size)
		return hc12lrae_ram_size; /* out of range value as error mark */
	return addr - hc12lrae_ram_base;
}


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

	buf = malloc(hc12lrae_ram_size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	entry = 0xffffffff;

	if (!agent && options.verbose)
	{
		printf("RAM image file <%s>\n",
		       (const char *)file);
	}

	ret = srec_read(file, info, sizeof(info), buf, hc12lrae_ram_size,
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
		printf("RAM %s info <%s> address range <0x%04x-0x%04x> length <0x%04x> start <0x%04x>\n",
		       (const char *)(agent ? "agent" : "image"),
		       (const char *)info,
		       (unsigned int)addr_min,
		       (unsigned int)addr_max,
		       (unsigned int)len,
		       (unsigned int)entry);
	}

	uint16_host2be_buf(h + 0, (uint16_t)entry);
	uint16_host2be_buf(h + 2, (uint16_t)len);
	sum = h[0] + h[1] + h[2] + h[3];

	ret = serial_write(&hc12lrae_serial, h, sizeof(h), HC12LRAE_TX_TIMEOUT);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}

	t = progress_start(agent ? "RAM agent upload" : "RAM image upload");
	for (i = 0; i < len; ++ i)
	{
		b = buf[addr_min - hc12lrae_ram_base + i];
		sum += b;

		ret = serial_write(&hc12lrae_serial, &b, 1, HC12LRAE_TX_TIMEOUT);
		if (ret != 0)
		{
			progress_stop(t, NULL, 0);
			free(buf);
			return ret;
		}

		progress_report(i + 1, len);
	}
	progress_stop(t, NULL, 0);

	free(buf);

	ret = serial_write(&hc12lrae_serial, &sum, 1, HC12LRAE_TX_TIMEOUT);
	if (ret != 0)
		return ret;

	ret = serial_read(&hc12lrae_serial, &b, 1, HC12LRAE_CHECKSUM_TIMEOUT);
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


static int hc12lrae_ram_run(const char *file)
{
	int ret;

	ret = hc12lrae_ram_load(file, FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("RAM image loaded and started\n");

	return 0;
}


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


static int hc12lrae_tx(const void *buf, size_t len)
{
	return serial_write(&hc12lrae_serial, buf, len, (unsigned long)(len * HC12LRAE_TX_TIMEOUT));
}


static int hc12lrae_rx(void *buf, size_t len)
{
	int ret;

	ret = serial_read(&hc12lrae_serial, buf, len, HC12LRAE_AGENT_TIMEOUT);
	if (ret == ETIMEDOUT)
	{
		error("timeout - no connection with target\n");
		return ret;
	}

	return 0;
}


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

	if (options.verbose)
		printf("RAM agent file <%s>\n", (const char *)file);

	ret = hc12lrae_ram_load(file, TRUE);
	if (ret != 0)
		return ret;

	uint16_host2be_buf(osc, (uint16_t)(options.osc / 1000));

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
		error("target connection data error\n");
		return EINVAL;
	}

	hc12lrae_agent_loaded = TRUE;

	return 0;
}


#if 0
static uint32_t hc12_bdm_flash_read_address_nb(uint32_t addr)
{
	return addr + hc12_bdm_flash_linear_base;
}


static uint32_t hc12_bdm_flash_read_address_bl(uint32_t addr)
{
	return addr + (uint32_t)hc12_bdm_ppage_base * HCS12_FLASH_BANK_WINDOW_SIZE;
}


static uint32_t hc12_bdm_flash_read_address_bp(uint32_t addr)
{
	return (((uint32_t)hc12_bdm_ppage_base + (addr / HCS12_FLASH_BANK_WINDOW_SIZE)) << 16) +
		HCS12_FLASH_BANK_WINDOW_ADDR + (addr % HCS12_FLASH_BANK_WINDOW_SIZE);
}

static int hc12lrae_flash_read(const char *file)
{
	uint32_t size;
	int ret;
	uint8_t *buf;
	uint32_t i;
	uint32_t chunk;
	uint32_t addr;
	unsigned long t;
	uint8_t ppage;
	uint8_t p;
	uint32_t (*adc)(uint32_t addr);

	ret = hc12lrae_load_agent();
	if (ret != 0)
		return ret;

	if (target.ppage_count == 0)
	{
		error("FLASH not available\n");
		return EINVAL;
	}

	size = target.ppage_count * HCS12_FLASH_BANK_WINDOW_SIZE;
	buf = malloc(size);
	if (buf == NULL)
	{
		error("not enough memory\n");
		return ENOMEM;
	}

	chunk = size / 128;

#if 0
	t = progress_start("FLASH image read");
	for (i = 0; i < size; i += chunk)
	{
		if (options.flash_addr == HC12LOAD_FLASH_ADDR_NON_BANKED)
			addr = hc12_bdm_flash_linear_base + i;
		else
		{
			p = hc12_bdm_ppage_base +
				(uint8_t)(i / HCS12_FLASH_BANK_WINDOW_SIZE);
			if (p != ppage)
			{
				ret = (*hc12_bdm_handler->write_byte)(
					HCS12_IO_PPAGE, p);
				if (ret != 0)
					return ret;
				ppage = p;
			}
			addr = HCS12_FLASH_BANK_WINDOW_ADDR +
				(i % HCS12_FLASH_BANK_WINDOW_SIZE);
		}

		ret = (*hc12_bdm_handler->read_mem)(addr, buf + i, chunk);
		if (ret != 0)
		{
			progress_stop(t, NULL, 0);
			free(buf);
			return ret;
		}

		progress_report(i + chunk, size);
	}
	progress_stop(t, "FLASH image read", size);

	if (options.flash_addr == HC12LOAD_FLASH_ADDR_NON_BANKED)
		adc = hc12_bdm_flash_read_address_nb;
	else if (options.flash_addr == HC12LOAD_FLASH_ADDR_BANKED_LINEAR)
		adc = hc12_bdm_flash_read_address_bl;
	else if (options.flash_addr == HC12LOAD_FLASH_ADDR_BANKED_PPAGE)
		adc = hc12_bdm_flash_read_address_bp;
	ret = file_srec_write(file, "FLASH image", 0, size, buf, size - 2, adc,
		!options.flash_all, options.srec_size);
	if (ret != 0)
	{
		free(buf);
		return ret;
	}
#endif

	if (options.verbose)
	{
		printf("FLASH image file <%s> written\n",
		       (const char *)file);
	}

	free(buf);
	return 0;
}

#endif


hc12mem_target_handler_t hc12mem_target_handler_lrae =
{
	"lrae",
	hc12lrae_open,
	hc12lrae_close,
	hc12lrae_ram_run,
	hc12lrae_unsecure,
	hc12lrae_secure,
	NULL,
	NULL,
	NULL,
	NULL,
	/*
	hc12_bdm_eeprom_read,
	hc12_bdm_eeprom_erase,
	hc12_bdm_eeprom_write,
	hc12_bdm_eeprom_protect,
	*/
#if 0
	hc12lrae_flash_read,
	/*
	hc12lrae_flash_erase,
	hc12lrae_flash_write
	*/
#endif
};
