/*
    hc12mem - HC12 memory reader & writer
    bdm12pod.c: Kevin Ross' BDM12POD driver
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
#include "bdm12pod.h"
#include "serial.h"


/* globals */

static serial_t bdm12pod_serial;
static uint8_t bdm12pod_version;
static uint8_t bdm12pod_reset_delay;


/*
 *  send data to POD
 *
 *  in:
 *    data - data buffer
 *    len - data size
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_tx(const uint8_t *data, size_t len)
{
	size_t i;
	int state;
	unsigned long start;
	unsigned long now;
	int cnt;
	int ret;
	size_t size;

	for (i = 0; i < len; ++i)
	{
		state = TRUE;
		ret = serial_control(&bdm12pod_serial,
			SERIAL_CONTROL_SET_RTS, &state);
		if (ret != 0)
			return ret;

		start = sys_get_ms();
		now = start;
		cnt = BDM12POD_RETRIES;

		while (now - start < BDM12POD_CTS_TIMEOUT || cnt != 0)
		{
			ret = serial_control(&bdm12pod_serial,
				SERIAL_CONTROL_GET_CTS, &state);
			if (ret != 0)
				return ret;
			if (state)
				break;
			now = sys_get_ms();
			if (cnt != 0)
				--cnt;
		}

		if (!state)
		{
			error("BDM12POD not ready (CTS not asserted)\n");
			return EIO;
		}

		size = 1;
		ret = serial_write(&bdm12pod_serial, &data[i], &size, BDM12POD_TX_TIMEOUT);
		if (ret != 0)
			return ret;

		start = sys_get_ms();
		now = start;
		cnt = BDM12POD_RETRIES;

		while (now - start < BDM12POD_CTS_TIMEOUT || cnt != 0)
		{
			ret = serial_control(&bdm12pod_serial,
				SERIAL_CONTROL_GET_CTS, &state);
			if (ret != 0)
				return ret;
			if (!state)
				break;
			now = sys_get_ms();
			if (cnt != 0)
				--cnt;
		}

		if (state)
		{
			error("BDM12POD not ready (CTS not lowered)\n");
			return EIO;
		}

		state = FALSE;
		ret = serial_control(&bdm12pod_serial,
			SERIAL_CONTROL_SET_RTS, &state);
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  send command to POD and get answer
 *
 *  in:
 *    dtx - data buffer for send
 *    ntx - size of data to send
 *    drx - data buffer for read
 *    nrx - size of data to read
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_dialog(const uint8_t *dtx, size_t ntx,
	uint8_t *drx, size_t nrx)
{
	size_t i;
	int ret;
	size_t size;

	ret = bdm12pod_tx(dtx, ntx);
	if (ret != 0)
		return ret;

#if 0
	ret = serial_read(&bdm12pod_serial, drx, &nrx, BDM12POD_RX_TIMEOUT);
	if (ret == ETIMEDOUT)
	{
		error("connection timed out\n");
		return ret;
	}
	if (ret != 0)
		return ret;
#else
	for (i = 0; i < nrx; ++ i)
	{
		size = 1;
		ret = serial_read(&bdm12pod_serial, &drx[i], &size, BDM12POD_RX_TIMEOUT);
		if (ret == ETIMEDOUT)
		{
			error("connection timed out\n");
			return ret;
		}
		if (ret != 0)
			return ret;
	}
#endif

	return 0;
}


/*
 *  send 1-byte command to POD
 *
 *  in:
 *    cmd - command to send
 *    dbg - command name for debug output
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_cmd(uint8_t cmd, const char *dbg)
{
	int ret;

	ret = bdm12pod_dialog(&cmd, 1, NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
		printf("BDM12POD cmd <%s>\n", (const char *)dbg);

	return 0;
}


/*
 *  POD sync
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_sync(void)
{
	uint8_t q[1];
	int ret;

	q[0] = BDM12POD_CMD_SYNC;

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
		printf("BDM12POD sync\n");

	return 0;
}


/*
 *  reset target into special mode
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_reset_special(void)
{
	uint8_t q[1];
	int ret;

	q[0] = BDM12POD_CMD_RESET_CPU;

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
		printf("BDM12POD reset\n");

	sys_delay((unsigned long)bdm12pod_reset_delay);

	return 0;
}


/*
 *  output low level at reset line
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_reset_low(void)
{
	uint8_t q[1];
	int ret;

	q[0] = BDM12POD_CMD_RESET_LOW;

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
		printf("BDM12POD reset <low>\n");

	return 0;
}


/*
 *  output high level at reset line
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_reset_high(void)
{
	uint8_t q[1];
	int ret;

	q[0] = BDM12POD_CMD_RESET_HIGH;

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
		printf("BDM12POD reset <high>\n");

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

static int bdm12pod_reset_normal(void)
{
	int ret;

	ret = bdm12pod_reset_low();
	if (ret != 0)
		return ret;

	ret = bdm12pod_reset_high();
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  get POD version
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_get_version(void)
{
	uint8_t q[2];
	uint8_t a[1];
	int ret;

	q[0] = BDM12POD_CMD_EXT;
	q[1] = BDM12POD_CMD_EXT_GET_VERSION;

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	bdm12pod_version = a[0] & 0x7f;

	if (options.verbose)
	{
		printf("BDM12POD version <%u.%u%s>\n",
		       (unsigned int)(bdm12pod_version >> 4),
		       (unsigned int)(bdm12pod_version & 0x0f),
		       (const char *)((a[0] & 0x80) ? " +MODA/MODB" : ""));
	}

	return 0;
}


/*
 *  target register dump
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_reg_dump(void)
{
	uint8_t q[2];
	uint8_t a[13];
	int ret;

	q[0] = BDM12POD_CMD_EXT;
	q[1] = BDM12POD_CMD_EXT_REG_DUMP;

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD reg dump: <0x%02x> bdm status <0x%02x> pc <0x%04x>\n",
		       (unsigned int)a[0],
		       (unsigned int)a[1],
		       (unsigned int)uint16_be2host_from_buf(a + 2));
	}

	return 0;
}


/*
 *  target memory dump
 *
 *  in:
 *    addr - target memory address
 *    buf - buffer for data to read
 *    len - size of data to read
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_mem_dump(uint16_t addr, uint16_t *buf, uint16_t len)
{
	uint8_t q[6];
	int ret;

	q[0] = BDM12POD_CMD_EXT;
	q[1] = BDM12POD_CMD_EXT_MEM_DUMP;
	uint16_host2be_to_buf(q + 2, addr);
	uint16_host2be_to_buf(q + 4, len);

	ret = bdm12pod_dialog(q, sizeof(q), (uint8_t *)buf, len * 2);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		int i;

		printf("BDM12POD mem dump address <0x%04x> len <%u>\n",
		       (unsigned int)addr,
		       (unsigned int)(len * 2));
		for (i = 0; i < len * 2; ++ i)
			printf("%02x  ", (unsigned int)(((uint8_t *)buf)[i]));
		printf("\n");
	}

	return 0;
}


/*
 *  target memory write
 *
 *  in:
 *    addr - target memory address
 *    buf - buffer with data to write
 *    len - size of data to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_mem_put(uint16_t addr, const uint16_t *buf, uint16_t len)
{
	uint8_t q[6];
	int ret;

	if (options.debug)
	{
		int i;

		printf("BDM12POD mem put address <0x%04x> len <%u>\n",
		       (unsigned int)addr,
		       (unsigned int)(len * 2));
		for (i = 0; i < len * 2; ++ i)
			printf("%02x  ", (unsigned int)(((uint8_t *)buf)[i]));
		printf("\n");
	}

	if (bdm12pod_version < 0x46)
		return ENOTSUP;

	q[0] = BDM12POD_CMD_EXT;
	q[1] = BDM12POD_CMD_EXT_MEM_PUT;
	uint16_host2be_to_buf(q + 2, addr);
	uint16_host2be_to_buf(q + 4, len);

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	ret = bdm12pod_tx((uint8_t *)buf, len * 2);
	if (ret != 0)
		return ret;

	return 0;
}


/*
 *  set POD operating parameters
 *
 *  in:
 *    osc - target oscillator frequency
 *    trace_delay - delay after TRACE command
 *    reset_delay - delay after reset
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_set_param(unsigned long osc,
	uint8_t trace_delay, uint8_t reset_delay)
{
	unsigned long eclk;
	uint8_t q[5];
	int ret;

	q[0] = BDM12POD_CMD_EXT;
	q[1] = BDM12POD_CMD_EXT_SET_PARAM;
	q[3] = trace_delay;
	q[4] = reset_delay;

	eclk = osc / 2UL;
	if (options.podex_25)
		q[2] = BDM12POD_PARAM_ECLK_8M;
	else if (eclk == 1000000UL)
		q[2] = BDM12POD_PARAM_ECLK_1M;
	else if (eclk == 2000000UL)
		q[2] = BDM12POD_PARAM_ECLK_2M;
	else if (eclk == 4000000UL)
		q[2] = BDM12POD_PARAM_ECLK_4M;
	else if (eclk == 8000000UL)
		q[2] = BDM12POD_PARAM_ECLK_8M;
	else if (bdm12pod_version >= 0x47)
	{
		uint16_t v;
		uint8_t eq[5];

		v = (uint16_t)((128000000UL / (eclk / 1000UL) - 1400UL + 400UL) / 800UL); /* +400: round up */

		eq[0] = BDM12POD_CMD_EXT;
		eq[1] = BDM12POD_CMD_EXT_SPEED;
		eq[2] = 0; /* FIXME: podex ignores it, KR does not ? */
		uint16_host2be_to_buf(eq + 3, v);

		ret = bdm12pod_dialog(eq, sizeof(eq), NULL, 0);
		if (ret != 0)
			return ret;
	}
	else
	{
		error("this BDM12POD version supports only 1/2/4/8 MHz E-clock\n");
		return ENOTSUP;
	}

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.verbose)
	{
		printf("BDM12POD E-clock <%lu.%06lu MHz> trace delay <%u ms> reset delay <%u ms>\n",
		       (unsigned long)(eclk / 1000000UL),
		       (unsigned long)(eclk % 1000000UL),
		       (unsigned int)trace_delay,
		       (unsigned int)reset_delay);
	}

	bdm12pod_reset_delay = reset_delay;

	return 0;
}


/*
 *  execute BACKGROUND command
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_background(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_HW_BACKGROUND, "BACKGROUND");
}


/*
 *  execute ACK ENABLE/DISABLE command
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_ack_enable(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_HW_ACK_ENABLE, "ACK_ENABLE");
}


static int bdm12pod_ack_disable(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_HW_ACK_DISABLE, "ACK_DISABLE");
}


/*
 *  read BD byte
 *
 *  in:
 *    addr - byte address
 *    v - value read (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_bd_byte(uint16_t addr, uint8_t *v)
{
	uint8_t q[3];
	uint8_t a[2];
	int ret;

	q[0] = HC12BDM_CMD_HW_READ_BD_BYTE;
	uint16_host2be_to_buf(q + 1, addr);

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = ((addr & 1) == 0 ? a[0] : a[1]);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_BD_BYTE> addr <0x%04x> value <0x%02x>\n",
		       (unsigned int)addr,
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  read BD word
 *
 *  in:
 *    addr - word address
 *    v - value read (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_bd_word(uint16_t addr, uint16_t *v)
{
	uint8_t q[3];
	uint8_t a[2];
	int ret;

	q[0] = HC12BDM_CMD_HW_READ_BD_WORD;
	uint16_host2be_to_buf(q + 1, addr);

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = uint16_be2host_from_buf(a);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_BD_WORD> addr <0x%04x> value <0x%04x>\n",
		       (unsigned int)addr,
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  read target memory byte
 *
 *  in:
 *    addr - byte address
 *    v - value read (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_byte(uint16_t addr, uint8_t *v)
{
	uint8_t q[3];
	uint8_t a[2];
	int ret;

	q[0] = HC12BDM_CMD_HW_READ_BYTE;
	uint16_host2be_to_buf(q + 1, addr);

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = ((addr & 1) == 0 ? a[0] : a[1]);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_BYTE> addr <0x%04x> value <0x%02x>\n",
		       (unsigned int)addr,
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  read target memory word
 *
 *  in:
 *    addr - word address
 *    v - value read (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_word(uint16_t addr, uint16_t *v)
{
	uint8_t q[3];
	uint8_t a[2];
	int ret;

	q[0] = HC12BDM_CMD_HW_READ_WORD;
	uint16_host2be_to_buf(q + 1, addr);

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = uint16_be2host_from_buf(a);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_WORD> addr <0x%04x> value <0x%04x>\n",
		       (unsigned int)addr,
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  write BD byte
 *
 *  in:
 *    addr - byte address
 *    v - value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_bd_byte(uint16_t addr, uint8_t v)
{
	uint8_t q[5];
	int ret;

	q[0] = HC12BDM_CMD_HW_WRITE_BD_BYTE;
	uint16_host2be_to_buf(q + 1, addr);
	if ((addr & 0x0001) == 0)
	{
		q[3] = v;
		q[4] = 0;
	}
	else
	{
		q[3] = 0;
		q[4] = v;
	}

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_BD_BYTE> addr <0x%04x> value <0x%02x>\n",
		       (unsigned int)addr,
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  write BD word
 *
 *  in:
 *    addr - word address
 *    v - value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_bd_word(uint16_t addr, uint16_t v)
{
	uint8_t q[5];
	int ret;

	q[0] = HC12BDM_CMD_HW_WRITE_BD_WORD;
	uint16_host2be_to_buf(q + 1, addr);
	uint16_host2be_to_buf(q + 3, v);

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_BD_WORD> addr <0x%04x> value <0x%04x>\n",
		       (unsigned int)addr,
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  write target memory byte
 *
 *  in:
 *    addr - byte address
 *    v - value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_byte(uint16_t addr, uint8_t v)
{
	uint8_t q[5];
	int ret;

	q[0] = HC12BDM_CMD_HW_WRITE_BYTE;
	uint16_host2be_to_buf(q + 1, addr);
	if ((addr & 0x0001) == 0)
	{
		q[3] = v;
		q[4] = 0;
	}
	else
	{
		q[3] = 0;
		q[4] = v;
	}

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_BYTE> addr <0x%04x> value <0x%02x>\n",
		       (unsigned int)addr,
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  write target memory word
 *
 *  in:
 *    addr - word address
 *    v - value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_word(uint16_t addr, uint16_t v)
{
	uint8_t q[5];
	int ret;

	q[0] = HC12BDM_CMD_HW_WRITE_WORD;
	uint16_host2be_to_buf(q + 1, addr);
	uint16_host2be_to_buf(q + 3, v);

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_WORD> addr <0x%04x> value <0x%04x>\n",
		       (unsigned int)addr,
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  read next memory word
 *
 *  in:
 *    v - value (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_next(uint16_t *v)
{
	uint8_t q[1];
	uint8_t a[2];
	int ret;

	q[0] = HC12BDM_CMD_FW_READ_NEXT;
	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = uint16_be2host_from_buf(a);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_NEXT> value <0x%04x>\n",
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  read target CPU register
 *
 *  in:
 *    reg - register selection
 *    v - register value (on return)
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_reg(int reg, uint16_t *v)
{
	uint8_t q[1];
	uint8_t a[2];
	int ret;

	switch (reg)
	{
		case HC12BDM_REG_PC:
			q[0] = HC12BDM_CMD_FW_READ_PC;
			break;
		case HC12BDM_REG_D:
			q[0] = HC12BDM_CMD_FW_READ_D;
			break;
		case HC12BDM_REG_X:
			q[0] = HC12BDM_CMD_FW_READ_X;
			break;
		case HC12BDM_REG_Y:
			q[0] = HC12BDM_CMD_FW_READ_Y;
			break;
		case HC12BDM_REG_SP:
			q[0] = HC12BDM_CMD_FW_READ_SP;
			break;
		case HC12BDM_REG_CCR:
			/* not supported - skip to default: */
		default:
			return EINVAL;
	}

	ret = bdm12pod_dialog(q, sizeof(q), a, sizeof(a));
	if (ret != 0)
		return ret;

	*v = uint16_be2host_from_buf(a);

	if (options.debug)
	{
		printf("BDM12POD cmd <READ_REG> value <0x%04x>\n",
		       (unsigned int)(*v));
	}

	return 0;
}


/*
 *  write next memory word
 *
 *  in:
 *    v - value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_next(uint16_t v)
{
	uint8_t q[3];
	int ret;

	q[0] = HC12BDM_CMD_FW_WRITE_NEXT;
	uint16_host2be_to_buf(q + 1, v);

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_REG> value <0x%04x>\n",
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  write target CPU register
 *
 *  in:
 *    reg - register selection
 *    v - register value to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_reg(int reg, uint16_t v)
{
	uint8_t q[3];
	int ret;

	switch (reg)
	{
		case HC12BDM_REG_PC:
			q[0] = HC12BDM_CMD_FW_WRITE_PC;
			break;
		case HC12BDM_REG_D:
			q[0] = HC12BDM_CMD_FW_WRITE_D;
			break;
		case HC12BDM_REG_X:
			q[0] = HC12BDM_CMD_FW_WRITE_X;
			break;
		case HC12BDM_REG_Y:
			q[0] = HC12BDM_CMD_FW_WRITE_Y;
			break;
		case HC12BDM_REG_SP:
			q[0] = HC12BDM_CMD_FW_WRITE_SP;
			break;
		case HC12BDM_REG_CCR:
			/* not supported - skip to default: */
		default:
			return EINVAL;
	}

	uint16_host2be_to_buf(q + 1, v);

	ret = bdm12pod_dialog(q, sizeof(q), NULL, 0);
	if (ret != 0)
		return ret;

	if (options.debug)
	{
		printf("BDM12POD cmd <WRITE_NEXT> value <0x%04x>\n",
		       (unsigned int)v);
	}

	return 0;
}


/*
 *  execute GO/GO_UNTIL/TRACE1/TAGGO command
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_go(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_FW_GO, "GO");
}


static int bdm12pod_go_until(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_FW_GO_UNTIL, "GO_UNTIL");
}


static int bdm12pod_trace1(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_FW_TRACE1, "TRACE1");
}


static int bdm12pod_taggo(void)
{
	return bdm12pod_cmd(HC12BDM_CMD_FW_TAGGO, "TAGGO");
}


/*
 *  read target memory (can be unaligned)
 *
 *  in:
 *    addr - target memory address
 *    buf - buffer for data to read
 *    len - size of data to read
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_read_mem(uint16_t addr, void *buf, size_t len)
{
	uint8_t *ptr;
	size_t n;
	size_t i;
	uint16_t v;
	int ret;

	if (len == 0)
		return 0;

	ptr = (uint8_t *)buf;

	if (addr & 1)
	{
		ret = bdm12pod_read_byte(addr++, ptr++);
		if (ret != 0)
			return ret;

		if (--len == 0)
			return 0;
	}

	if (len >= 2)
	{
		n = len / 2; /* loose odd remainder */

		if (options.podex_mem_bug)
		{
			for (i = 0; i < n; ++ i)
			{
				ret = bdm12pod_read_word((uint16_t)(addr + i * 2), &v);
				if (ret != 0)
					return ret;
				uint16_host2be_to_buf(ptr + i * 2, v);
			}
		}
		else
		{
			ret = bdm12pod_mem_dump(addr, (uint16_t *)ptr, (uint16_t)n);
			if (ret != 0)
				return ret;
		}

		n *= 2;
		len -= n;
		addr += (uint16_t)n;
		ptr += n;
	}

	if (len != 0) /* must be 1 */
	{
		ret = bdm12pod_read_byte(addr, ptr);
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  write target memory (can be unaligned)
 *
 *  in:
 *    addr - target memory address
 *    buf - buffer with data to write
 *    len - size of data to write
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_write_mem(uint16_t addr, const void *buf, size_t len)
{
	const uint8_t *ptr;
	size_t n;
	size_t i;
	uint16_t v;
	int ret;

	if (len == 0)
		return 0;

	ptr = (const uint8_t *)buf;

	if (addr & 1)
	{
		ret = bdm12pod_write_byte(addr++, *ptr++);
		if (ret != 0)
			return ret;

		if (--len == 0)
			return 0;
	}

	if (len >= 2)
	{
		n = len / 2; /* loose odd remainder */

		if (options.podex_mem_bug || bdm12pod_version < 0x46)
		{
			for (i = 0; i < n; ++ i)
			{
				v = uint16_be2host_from_buf(ptr + i * 2);
				ret = bdm12pod_write_word((uint16_t)(addr + i * 2), v);
				if (ret != 0)
					return ret;
			}
		}
		else
		{
			ret = bdm12pod_mem_put(addr, (uint16_t *)ptr, (uint16_t)n);
			if (ret != 0)
				return ret;
		}

		n *= 2;
		len -= n;
		addr += (uint16_t)n;
		ptr += n;
	}

	if (len != 0) /* must be 1 */
	{
		ret = bdm12pod_write_byte(addr, *ptr);
		if (ret != 0)
			return ret;
	}

	return 0;
}


/*
 *  open POD connection
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_open(void)
{
	serial_cfg_t cfg;
	int ret;

	if (options.osc == 0)
	{
		error("oscillator frequency not specified (-o option required)\n");
		return EINVAL;
	}

	if (options.port == NULL)
	{
		error("serial port not specified (-p option required)\n");
		return EINVAL;
	}

	ret = serial_open(&bdm12pod_serial, options.port);
	if (ret != 0)
		return ret;

	if (options.baud == 0)
		options.baud = BDM12POD_DEFAULT_BAUD_RATE;

	cfg.baud_rate = options.baud;
	cfg.char_size = SERIAL_CFG_CHAR_SIZE_8;
	cfg.parity = SERIAL_CFG_PARITY_NONE;
	cfg.stop_bits = SERIAL_CFG_STOP_BITS_1;
	cfg.handshake = SERIAL_CFG_HANDSHAKE_NONE;

	ret = serial_set_cfg(&bdm12pod_serial, &cfg);
	if (ret != 0)
		goto error;

	if (options.verbose)
	{
		printf("BDM12POD serial port <%s> baud rate <%lu bps>\n",
		       (const char *)options.port,
		       (unsigned long)options.baud);
	}

	ret = bdm12pod_sync();
	if (ret != 0)
		goto error;

	ret = bdm12pod_get_version();
	if (ret != 0)
		goto error;

	ret = bdm12pod_set_param(options.osc,
		BDM12POD_DEFAULT_TRACE_DELAY,
		BDM12POD_DEFAULT_RESET_DELAY);
	if (ret != 0)
		goto error;

	return 0;

error:
	serial_close(&bdm12pod_serial);
	return ret;
}


/*
 *  close POD connection
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int bdm12pod_close(void)
{
	return serial_close(&bdm12pod_serial);
}


/* POD handler */

hc12bdm_handler_t bdm12pod_bdm_handler =
{
	bdm12pod_open,
	bdm12pod_close,
	bdm12pod_reset_normal,
	bdm12pod_reset_special,
	bdm12pod_background,
	bdm12pod_ack_enable,
	bdm12pod_ack_disable,
	bdm12pod_read_bd_byte,
	bdm12pod_read_bd_word,
	bdm12pod_read_byte,
	bdm12pod_read_word,
	bdm12pod_write_bd_byte,
	bdm12pod_write_bd_word,
	bdm12pod_write_byte,
	bdm12pod_write_word,
	bdm12pod_read_mem,
	bdm12pod_write_mem,
	bdm12pod_read_next,
	bdm12pod_read_reg,
	bdm12pod_write_next,
	bdm12pod_write_reg,
	bdm12pod_go,
	bdm12pod_go_until,
	bdm12pod_trace1,
	bdm12pod_taggo
};
