/*
    hc12mem - HC12 memory reader & writer
    hc12sm.c: Freescale AN2548 serial monitor
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
#include "hc12sm.h"
#include "hc12bdm.h"
#include "serial.h"
#include "srec.h"
#include "../target/agent.h"


/* globals */

static serial_t hc12sm_serial;


/*
 *  receive data from target
 *
 *  in:
 *    buf - data buffer
 *    len - data length
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_rx(void *buf, size_t len, unsigned long timeout)
{
	int ret;

	ret = serial_read(&hc12sm_serial, buf, &len, timeout);
	if (ret == ETIMEDOUT)
	{
		error("timeout - no connection with target\n");
		return ret;
	}

	return 0;
}


/*
 *  send command
 *
 *  in:
 *    cmd - command to send
 *    tx - data to send
 *    ntx - size of data to send
 *    rx - buffer for received data
 *    nrx - size of data to receive
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_cmd(uint8_t cmd, const void *tx, size_t ntx, void *rx, size_t nrx)
{
	size_t n;
	int ret;

	n = 1;
	ret = serial_write(&hc12sm_serial, &cmd, &n, HC12SM_TX_TIMEOUT);
	if (ret != 0)
		return ret;

	if (ntx != 0)
	{
		ret = serial_write(&hc12sm_serial, tx, &ntx, HC12SM_TX_TIMEOUT);
		if (ret != 0)
			return ret;
	}

	if (nrx != 0)
	{
		ret = hc12sm_rx(rx, nrx, HC12SM_RX_TIMEOUT);
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  get prompt
 *
 *  in:
 *    start - flag set when called on start (to enable recognition of
 *            already started monitor)
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_prompt(int start)
{
	uint8_t b[3];
	int ret;

	ret = hc12sm_rx(b, 3, HC12SM_RX_TIMEOUT);
	if (ret != 0)
		return ret;

	while (start && b[2] != HC12SM_PROMPT_SYMBOL)
	{
		b[0] = b[1];
		b[1] = b[2];

		ret = hc12sm_rx(b + 2, 1, HC12SM_FLUSH_TIMEOUT);
		if (ret != 0)
			return ret;
	}

	if (start && b[0] == HC12SM_ERROR_CMD_UNKNOWN)
		b[0] = HC12SM_ERROR_NONE;

	switch (b[0])
	{
		case HC12SM_ERROR_NONE:
			break;

		case HC12SM_ERROR_CMD_UNKNOWN:
			error("SM command not recognized\n");
			return EIO;

		case HC12SM_ERROR_CMD_NOT_ALLOWED:
			error("SM command not allowed in run mode\n");
			return EIO;

		case HC12SM_ERROR_SP_OUT_OF_RANGE:
			error("SM stack pointer out of range\n");
			return EIO;

		case HC12SM_ERROR_INVALID_SP_VALUE:
			error("SM invalid write SP value\n");
			return EIO;

		case HC12SM_ERROR_NVM_BYTE_WRITE:
			error("SM byte write access to nonvolatile memory\n");
			return EIO;

		case HC12SM_ERROR_FLASH_ERROR:
			error("SM FLASH error\n");
			return EIO;

		case HC12SM_ERROR_NA1:
		case HC12SM_ERROR_NA2:
			error("SM error code not implemented\n");
			return EIO;

		case HC12SM_ERROR_EEPROM_ERROR:
			error("SM EEPROM error\n");
			return EIO;

		default:
			error("SM unknown error code 0x%02x\n", (unsigned int)b[0]);
			return EIO;
	}

	if (b[2] != HC12SM_PROMPT_SYMBOL)
	{
		error("unexpected target prompt char\n");
		return EIO;
	}

	return 0;
}


static int hc12sm_cmd_read_byte(uint16_t addr, uint8_t *v)
{
	uint8_t cmd[2];
	int ret;

	uint16_host2be_buf(cmd, addr);
	ret = hc12sm_cmd(HC12SM_CMD_READ_BYTE, cmd, 2, v, 1);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	return 0;
}


static int hc12sm_cmd_write_byte(uint16_t addr, uint8_t v)
{
	uint8_t cmd[3];
	int ret;

	uint16_host2be_buf(cmd + 0, addr);
	cmd[2] = v;
	ret = hc12sm_cmd(HC12SM_CMD_WRITE_BYTE, cmd, 3, NULL, 0);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	return 0;
}


static int hc12sm_cmd_read_word(uint16_t addr, uint16_t *v)
{
	uint8_t cmd[2];
	int ret;

	uint16_host2be_buf(cmd, addr);
	ret = hc12sm_cmd(HC12SM_CMD_READ_WORD, cmd, 2, v, 2);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	*v = uint16_be2host(*v);

	return 0;
}


static int hc12sm_cmd_write_word(uint16_t addr, uint16_t v)
{
	uint8_t cmd[4];
	int ret;

	uint16_host2be_buf(cmd + 0, addr);
	uint16_host2be_buf(cmd + 2, v);
	ret = hc12sm_cmd(HC12SM_CMD_WRITE_WORD, cmd, 4, NULL, 0);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	return 0;
}


static int hc12sm_cmd_read_block(uint16_t addr, size_t len, void *buf)
{
	uint8_t cmd[3];
	int ret;

	if (len == 0 || len > HC12SM_BLOCK_SIZE_MAX)
		return EINVAL;

	uint16_host2be_buf(cmd + 0, addr);
	cmd[2] = (uint8_t)(len - 1);
	ret = hc12sm_cmd(HC12SM_CMD_READ_BLOCK, cmd, 3, buf, (size_t)len);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	return 0;
}


static int hc12sm_cmd_write_block(uint16_t addr, size_t len, const void *buf)
{
	uint8_t cmd[3 + HC12SM_BLOCK_SIZE_MAX];
	int ret;

	if (len == 0 || len > HC12SM_BLOCK_SIZE_MAX)
		return EINVAL;

	uint16_host2be_buf(cmd + 0, addr);
	cmd[2] = (uint8_t)(len - 1);
	memcpy(cmd + 3, buf, len);
	ret = hc12sm_cmd(HC12SM_CMD_WRITE_BLOCK, cmd, 3 + len, NULL, 0);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	return 0;
}


static int hc12sm_cmd_device_info(uint16_t *partid)
{
	uint8_t info[3];
	uint8_t id[HC12SM_FLASH_ID_SIZE];
	int ret;

	ret = hc12sm_cmd(HC12SM_CMD_DEVICE_INFO, NULL, 0, info, 3);
	if (ret != 0)
		return ret;

	if (info[0] != HC12SM_DEVICE_INFO_CODE)
	{
		error("unexpected device code received: 0x%02x\n",
		      (unsigned int)info[0]);
		return EIO;
	}

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	*partid = uint16_be2host_buf(info + 1);

	ret = hc12sm_cmd_read_block(HC12SM_FLASH_ID_ADDR,
		HC12SM_FLASH_ID_SIZE, id);
	if (ret != 0)
		return ret;

	if (options.verbose)
	{
		printf("SM version <%u.%02u> date <%02x%02x-%02x-%02x>\n",
		       (unsigned int)id[6],
		       (unsigned int)id[7],
		       (unsigned int)id[4],
		       (unsigned int)id[5],
		       (unsigned int)id[2],
		       (unsigned int)id[3]);
	}

	return 0;
}


/*
 *  open connection with target via LRAE bootloader
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_open(void)
{
	serial_cfg_t cfg;
	uint8_t b;
	size_t size;
	uint16_t id;
	int ret;

	if (options.port == NULL)
	{
		error("serial port not specified (-p option required)\n");
		return EINVAL;
	}

	/*
	if (options.osc == 0)
	{
		error("oscillator frequency not specified (-o option required)\n");
		return EINVAL;
	}
	*/

	hc12mcu_target.read_byte = hc12sm_cmd_read_byte;
	hc12mcu_target.read_word = hc12sm_cmd_read_word;
	hc12mcu_target.write_byte = hc12sm_cmd_write_byte;
	hc12mcu_target.write_word = hc12sm_cmd_write_word;

	ret = serial_open(&hc12sm_serial, options.port);
	if (ret != 0)
		return ret;

	if (options.baud == 0)
		options.baud = HC12SM_BAUD_RATE;
	cfg.baud_rate = options.baud;
	cfg.char_size = SERIAL_CFG_CHAR_SIZE_8;
	cfg.parity = SERIAL_CFG_PARITY_NONE;
	cfg.stop_bits = SERIAL_CFG_STOP_BITS_1;
	cfg.handshake = SERIAL_CFG_HANDSHAKE_NONE;

	ret = serial_set_cfg(&hc12sm_serial, &cfg);
	if (ret != 0)
	{
		serial_close(&hc12sm_serial);
		return ret;
	}

	if (options.verbose)
	{
		printf("SM serial port <%s> baud rate <%lu bps>\n",
		       (const char *)options.port,
		       (unsigned long)options.baud);
	}

	b = HC12SM_SYNC_QUERY;
	size = 1;
	ret = serial_write(&hc12sm_serial, &b, &size, HC12SM_TX_TIMEOUT);
	if (ret != 0)
	{
		serial_close(&hc12sm_serial);
		return ret;
	}

	ret = hc12sm_prompt(TRUE);
	if (ret != 0)
	{
		serial_close(&hc12sm_serial);
		return ret;
	}

	if (options.verbose)
		printf("SM target connected\n");

	ret = hc12sm_cmd_device_info(&id);
	if (ret != 0)
	{
		serial_close(&hc12sm_serial);
		return ret;
	}

	hcs12mcu_partid(id, TRUE);

	ret = hc12mcu_identify(TRUE);
	if (ret != 0)
	{
		serial_close(&hc12sm_serial);
		return ret;
	}

	return 0;
}


/*
 *  close connection with target via sm bootloader
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_close(void)
{
	return serial_close(&hc12sm_serial);
}


/*
 *  load code and run from target RAM
 *
 *  in:
 *    file - data file name to read
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_ram_run(const char *file)
{
	error("RAM run: operation not supported\n");
	return EINVAL;
}


/*
 *  verify erasure of EEPROM
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_eeprom_erase_verify(void)
{
	int ret;
	uint32_t i;
	uint8_t buf[HC12SM_BLOCK_SIZE_MAX];
	int j;

	for (i = 0; i < hc12mcu_target.eeprom_size; i += HC12SM_BLOCK_SIZE_MAX)
	{
		ret = hc12sm_cmd_read_block(
			(uint16_t)(i + hc12mcu_target.eeprom_base),
			HC12SM_BLOCK_SIZE_MAX, buf);
		if (ret != 0)
			return ret;
		for (j = 0; j < HC12SM_BLOCK_SIZE_MAX; ++j)
		{
			if (buf[j] != 0xff)
			{
				error("EEPROM memory not erased: value 0x%02x @ 0x%04x\n",
				      (unsigned int)buf[j],
				      (unsigned int)(i + hc12mcu_target.eeprom_base));
				return EIO;
			}
		}
	}

	if (options.verbose)
		printf("EEPROM erase: memory verified\n");

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

static int hc12sm_eeprom_erase(void)
{
	int ret;

	if (hc12mcu_target.eeprom_size == 0)
	{
		error("EEPROM erase not possible - no EEPROM memory\n");
		return EINVAL;
	}

	ret = hc12sm_cmd(HC12SM_CMD_ERASE_EEPROM, NULL, 0, NULL, 0);
	if (ret != 0)
		return ret;

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("EEPROM erase: memory erased\n");

	if (options.verify)
	{
		ret = hc12sm_eeprom_erase_verify();
		if (ret != 0)
			return ret;
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

static int hc12sm_eeprom_read(const char *file)
{
	return hc12mcu_eeprom_read(file, HC12SM_BLOCK_SIZE_MAX, hc12sm_cmd_read_block);
}


/*
 *  write target EEPROM
 *
 *  in:
 *    file - file name with data to write
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_eeprom_write(const char *file)
{
	return hc12mcu_eeprom_write(file, HC12SM_BLOCK_SIZE_MAX, hc12sm_cmd_write_block);
}


/*
 *  protect target EEPROM
 *
 *  in:
 *    opt - protection option string
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_eeprom_protect(const char *opt)
{
	return hc12mcu_eeprom_protect(opt, hc12sm_cmd_write_word);
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

static int hc12sm_flash_read_cb(uint32_t addr, size_t size, void *buf)
{
	int ret;

	ret = hc12sm_cmd_write_byte(HCS12_IO_PPAGE, hc12mcu_linear_to_ppage(addr));
	if (ret != 0)
		return ret;

	ret = hc12sm_cmd_read_block((uint16_t)
		(HCS12_FLASH_BANK_WINDOW_ADDR + (addr % HCS12_FLASH_BANK_WINDOW_SIZE)),
		size, buf);
	if (ret != 0)
		return ret;

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

static int hc12sm_flash_read(const char *file)
{
	return hc12mcu_flash_read(file, HC12SM_BLOCK_SIZE_MAX, hc12sm_flash_read_cb);
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

static int hc12sm_flash_write_cb(uint32_t addr, size_t size, const void *buf)
{
	int ret;
	uint8_t ppage;
	uint16_t a;

	ppage = hc12mcu_linear_to_ppage(addr);
	a = (uint16_t)hc12mcu_flash_addr_window(addr);

	ret = hc12sm_cmd_write_byte(HCS12_IO_FCNFG, hc12mcu_linear_to_block(addr));
	if (ret != 0)
		return ret;

	ret = hc12sm_cmd_write_byte(HCS12_IO_PPAGE, ppage);
	if (ret != 0)
		return ret;

	/* if requested address covers serial monitor image in FLASH,
	   change address into direct address from last FLASH page */

	if (ppage == hc12mcu_target.ppage_base + hc12mcu_target.ppage_count - 1 &&
	    a >= hc12mcu_flash_addr_window(HC12SM_FLASH_IMAGE_START))
		a += HCS12_FLASH_BANK_WINDOW_SIZE;

	ret = hc12sm_cmd_write_block(a, size, buf);
	if (ret != 0)
		return ret;

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

static int hc12sm_flash_write(const char *file)
{
	return hc12mcu_flash_write(file, HC12SM_BLOCK_SIZE_MAX, hc12sm_flash_write_cb);
}


/*
 *  verify erasure of FLASH
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_flash_erase_verify(void)
{
	int ret;
	uint32_t size;
	unsigned long t;
	uint32_t i;
	uint8_t buf[HC12SM_BLOCK_SIZE_MAX];
	int j;
	int fa;

	size = hc12mcu_target.flash_size - HC12SM_FLASH_IMAGE_SIZE;
	t = progress_start("FLASH erase: verify");
	for (i = 0; i < size; i += HC12SM_BLOCK_SIZE_MAX)
	{
		fa = options.flash_addr;
		options.flash_addr = HC12MEM_FLASH_ADDR_BANKED_LINEAR;
		ret = hc12sm_flash_read_cb(i, HC12SM_BLOCK_SIZE_MAX, buf);
		options.flash_addr = fa;
		if (ret != 0)
		{
			progress_stop(t, NULL, 0);
			return ret;
		}

		for (j = 0; j < HC12SM_BLOCK_SIZE_MAX; j += sizeof(uint32_t))
		{
			if (*((uint32_t *)(buf + j)) != 0xffffffff)
			{
				progress_stop(t, NULL, 0);
				error("FLASH memory not erased\n");
				return EIO;
			}
		}

		progress_report(i + HC12SM_BLOCK_SIZE_MAX, size);
	}
	progress_stop(t, "FLASH erase: verify", size);

	if (options.verbose)
		printf("FLASH erase: memory verified\n");

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

static int hc12sm_flash_erase(int unsecure)
{
	int ret;

	if (hc12mcu_target.flash_size == 0)
	{
		error("FLASH erase not possible - no FLASH memory\n");
		return EINVAL;
	}

	ret = hc12sm_cmd(HC12SM_CMD_ERASE_ALL, NULL, 0, NULL, 0);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("FLASH erase: wait ...\n");

	ret = hc12sm_prompt(FALSE);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("FLASH erase: memory erased\n");

	if (options.verify)
	{
		ret = hc12sm_flash_erase_verify();
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  reset target
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int hc12sm_reset(void)
{
	int ret;

	ret = hc12sm_cmd(HC12SM_CMD_RESET, NULL, 0, NULL, 0);
	if (ret != 0)
		return ret;

	if (options.verbose)
		printf("target reset\n");

	return 0;
}


/*
 *  unsupported operations
 */

static int hc12sm_unsecure(void)
{
	error("unsecure: operation not supported\n");
	return EINVAL;
}


static int hc12sm_secure(void)
{
	error("secure: operation not supported\n");
	return EINVAL;
}


/* handler for serial monitor */

hc12mem_target_handler_t hc12mem_target_handler_sm =
{
	"sm",
	hc12sm_open,
	hc12sm_close,
	hc12sm_ram_run,
	hc12sm_unsecure,
	hc12sm_secure,
	hc12sm_eeprom_read,
	hc12sm_eeprom_erase,
	hc12sm_eeprom_write,
	hc12sm_eeprom_protect,
	hc12sm_flash_read,
	hc12sm_flash_erase,
	hc12sm_flash_write,
	NULL,
	hc12sm_reset
};
