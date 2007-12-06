/*
    hcs12mem - HCS12/S12 memory reader & writer
    Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>

    tbdml.h: Turbo BDM Light interface
    based on
    Turbo BDM Light - interface DLL
    Copyright (C) 2005  Daniel Malik

    $Id$

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

#include "hcs12mem.h"
#include "tbdml.h"
#include "tbdml_comm.h"
#include "sys_usb.h"

/* usb device connection handle */
static sys_usb_dev_t tbdml_device;


/*
 *  send command to TBDML
 *
 *  in:
 *    cmd - command type
 *    btx - buffer with query data (tx)
 *    ntx - query data size
 *    brx - buffer for answer data (rx)
 *    ntx - answer data size 
 *          -1 when anwer with ack only is required
 *          0 when no answer is expected
 *  out:
 *    status code (errno-like)
 */

static int tbdml_cmd(uint8_t cmd, const void *btx, int ntx, void *brx, int nrx)
{
	uint8_t buf[TBDML_MAX_DATA_SIZE + 2];
	size_t n;
	int ret;

	buf[1] = cmd;
	if (ntx != 0)
		memcpy(buf + 2, btx, ntx);

	if (nrx == 0)
	{
		buf[0] = 0;
		n = (size_t)(ntx > 4 ? ntx - 4 : 0);
		ret = sys_usb_control_msg(&tbdml_device,
			USB_ENDPOINT_OUT | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			cmd,
			uint16_host2le_from_buf(buf + 2),
			uint16_host2le_from_buf(buf + 4),
			buf + 6, &n, TBDML_TIMEOUT);
		if (ret != 0)
			return ret;
	}
	else
	{
		if (nrx == -1)
			nrx = 0;
		n = (size_t)(nrx + 1);
		buf[0] = (uint8_t)n;
		ret = sys_usb_control_msg(&tbdml_device,
			USB_ENDPOINT_IN | USB_TYPE_VENDOR | USB_RECIP_DEVICE,
			cmd,
			uint16_host2le_from_buf(buf + 2),
			uint16_host2le_from_buf(buf + 4),
			buf, &n, TBDML_TIMEOUT);
		if (ret != 0)
			return ret;
		if (buf[0] == TBDML_CMD_UNKNOWN)
		{
			error("device communication failed: unknown command\n");
			return EIO;
		}
		if (buf[0] == TBDML_CMD_FAILED)
			return -1;
		if (buf[0] != cmd || n != (size_t)(nrx + 1))
		{
			error("device communication failed: unknown response\n");
			return EIO;
		}
		if (nrx != 0)
			memcpy(brx, buf + 1, nrx);
	}

	return 0;
}


/*
 *  send command to TBDML via bulk transfer over EP2
 *
 *  in:
 *    cmd - command type
 *    btx - buffer with query data (tx)
 *    ntx - query data size
 *    brx - buffer for answer data (rx)
 *    ntx - answer data size 
 *  out:
 *    status code (errno-like)
 */

static int tbdml_cmd_bulk(uint8_t cmd, const void *btx, size_t ntx, void *brx, size_t nrx)
{
	uint8_t buf[TBDML_MAX_DATA_SIZE + 3];
	size_t size;
	int ret;

	/* here we have a bug in the firmware of TBDML: for bulk transfers,
	   it requires one more byte at the beginning, with data size for
	   all the remaining data (starting with data size counter)
	   - so first byte must be one more than real byte counter in the
	   second byte */

	buf[0] = (uint8_t)(ntx + 2); /* one more for cmd, one more for size */
	buf[1] = (uint8_t)(ntx + 1); /* one more for cmd byte */
	buf[2] = cmd;
	if (ntx != 0)
		memcpy(buf + 3, btx, ntx);

	size = ntx + 3; /* 3 more bytes: size, size again and cmd */
	ret = sys_usb_bulk_write(&tbdml_device, 2, buf, &size, TBDML_TIMEOUT);
	if (ret != 0)
		return ret;

	size = nrx + 1; /* first returned byte before real data - status */
	ret = sys_usb_bulk_read(&tbdml_device, 2, buf, &size, TBDML_TIMEOUT);
	if (ret != 0)
		return ret;

	if (buf[0] == TBDML_CMD_UNKNOWN)
	{
		error("device communication failed: unknown command\n");
		return EIO;
	}
	if (buf[0] == TBDML_CMD_FAILED)
		return -1;
	if (buf[0] != cmd)
	{
		error("device communication failed: unknown response\n");
		return EIO;
	}
	if (nrx != 0)
		memcpy(brx, buf + 1, nrx);

	return 0;
}


/*
 *  get TBDML version
 *
 *  in:
 *    hw - hardware version (on return)
 *    sw - firmware version (on return)
 *  out:
 *    status code (errno-like)
 */

static int tbdml_get_version(uint8_t *hw, uint8_t *sw)
{
	tbdml_raw_version_t r;
	int ret;

	ret = tbdml_cmd(TBDML_CMD_GET_VER, NULL, 0, &r, sizeof(r));
	if (ret != 0)
		return ret;
	*hw = r.hw;
	*sw = r.sw;
	return 0;
}


/*
 *  get status of the last command
 *
 *  in:
 *    ok - TRYE/FALSE on return
 *  out:
 *    status code (errno-like)
 */

static int tbdml_get_last_status(int *ok)
{
	int ret;

	ret = tbdml_cmd(TBDML_CMD_GET_LAST_STATUS, NULL, 0, NULL, -1);
	if (ret == -1)
	{
		*ok = FALSE;
		return 0;
	}
	if (ret == 0)
	{
		*ok = TRUE;
		return 0;
	}
	return ret;
}


/*
 *  set target type
 *
 *  in:
 *    tt - target type
 *  out:
 *    status code (errno-like)
 */

static int tbdml_set_target_type(uint8_t tt)
{
	int ret;

	ret = tbdml_cmd(TBDML_CMD_SET_TARGET, &tt, sizeof(tt), NULL, -1);
	if (ret == -1)
	{
		error("unable to set target type: invalid type\n");
		return EINVAL;
	}
	return ret;
}


/*
 *  connect to target and measure target frequency
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int tbdml_target_sync(void)
{
	int ret;

	ret = tbdml_cmd(TBDML_CMD_CONNECT, NULL, 0, NULL, -1);
	if (ret == -1)
	{
		error("unable to connect to target device\n");
		return EIO;
	}
	return ret;
}


/*
 *  get BDM status
 *
 *  in:
 *    s - BDM status (on return)
 *  out:
 *    status code (errno-like)
 */

static int tbdml_bdm_status(tbdml_bdm_status_t *s)
{
	tbdml_raw_status_t r;
	int ret;

	ret = tbdml_cmd(TBDML_CMD_GET_STATUS, NULL, 0, &r, sizeof(r));
	if (ret != 0)
		return ret;
	s->ackn_support = (r.status & 0x01 ? TRUE : FALSE);
	s->reset_active = (r.status & 0x02 ? TRUE : FALSE);
	s->connection_state = (r.status & 0x18) >> 3;
	return 0;
}


/*
 *  get current target frequency
 *
 *  in:
 *    f - frequency (on return)
 *  out:
 *    status code (errno-like)
 */

static int tbdml_get_speed(unsigned long *f)
{
	tbdml_raw_speed_t speed;
	unsigned long sync;
	int ret;

	ret = tbdml_cmd(TBDML_CMD_READ_SPEED1, NULL, 0, &speed, sizeof(speed));
	if (ret != 0)
		return ret;
	sync = (unsigned long)uint16_le2host_from_buf(speed.sync_length);
	if (sync == 0)
		*f = 0;
	else
		*f = (((TBDML_SYNC_FREQ_MUL * 10000UL / sync) + 5UL) / 10UL) * 1000UL;
	return 0;
}


/*
 *  set target frequency
 *
 *  in:
 *    f - frequency
 *  out:
 *    status code (errno-like)
 */

static int tbdml_set_speed(unsigned long f)
{
	unsigned long sync;
	tbdml_raw_speed_t speed;
	int ret;

	if (f == 0)
		goto invalid;
	sync = ((TBDML_SYNC_FREQ_MUL * 10000UL / ((f + 500UL) / 1000UL)) + 5UL) / 10UL;
	if (sync == 0 || sync > 65535)
		goto invalid;
	uint16_host2le_to_buf(speed.sync_length, (uint16_t)sync);
	ret = tbdml_cmd(TBDML_CMD_SET_SPEED1, &speed, sizeof(speed), NULL, -1);
	if (ret == -1)
		goto invalid;
	return ret;
invalid:
	error("invalid target frequency\n");
	return EINVAL;
}


/*
 *  start target code execution
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int tbdml_go(void)
{
	return tbdml_cmd(TBDML_CMD_GO1, NULL, 0, NULL, -1);
}


/*
 *  step single target instruction
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int tbdml_trace1(void)
{
	return tbdml_cmd(TBDML_CMD_STEP1, NULL, 0, NULL, -1);
}


/*
 *  enter target background mode
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int tbdml_background(void)
{
	return tbdml_cmd(TBDML_CMD_HALT, NULL, 0, NULL, -1);
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

static int tbdml_read_bd_byte(uint16_t addr, uint8_t *v)
{
	uint8_t a[2];

	uint16_host2le_to_buf(a, addr);
	return tbdml_cmd(TBDML_CMD_READ_BD, a, sizeof(a), v, 1);
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

static int tbdml_read_byte(uint16_t addr, uint8_t *v)
{
	uint8_t a[2];

	uint16_host2le_to_buf(a, addr);
	return tbdml_cmd(TBDML_CMD_READ_8, a, sizeof(a), v, 1);
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

static int tbdml_read_word(uint16_t addr, uint16_t *v)
{
	uint8_t a[2];
	uint8_t buf[2];
	int ret;

	uint16_host2le_to_buf(a, addr);
	ret = tbdml_cmd(TBDML_CMD_READ_16, a, sizeof(a), buf, sizeof(buf));
	if (ret != 0)
		return ret;
	*v = uint16_le2host_from_buf(buf);
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

static int tbdml_write_bd_byte(uint16_t addr, uint8_t v)
{
	uint8_t buf[3];

	uint16_host2le_to_buf(buf + 0, addr);
	buf[2] = v;
	return tbdml_cmd(TBDML_CMD_WRITE_BD, buf, sizeof(buf), NULL, -1);
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

static int tbdml_write_byte(uint16_t addr, uint8_t v)
{
	uint8_t buf[3];

	uint16_host2le_to_buf(buf + 0, addr);
	buf[2] = v;
	return tbdml_cmd(TBDML_CMD_WRITE_8, buf, sizeof(buf), NULL, 0);
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

static int tbdml_write_word(uint16_t addr, uint16_t v)
{
	uint8_t buf[4];

	uint16_host2le_to_buf(buf + 0, addr);
	uint16_host2le_to_buf(buf + 2, v);
	return tbdml_cmd(TBDML_CMD_WRITE_16, buf, sizeof(buf), NULL, 0);
}


/*
 *  read target memory block
 *
 *  in:
 *    addr - target memory address (updated on return)
 *    buf - buffer for data to read (updated on return)
 *    len - size of data to read
 *  out:
 *    status code (errno-like)
 */

static int tbdml_read_block(uint16_t *addr, uint8_t **buf, uint16_t len)
{
	uint8_t q[3];
	int ret;

	uint16_host2le_to_buf(q + 0, *addr);
	q[2] = (uint8_t)len;
	if (options.tbdml_bulk)
		ret = tbdml_cmd_bulk(TBDML_CMD_READ_BLOCK1, q, sizeof(q), *buf, (int)len);
	else
		ret = tbdml_cmd(TBDML_CMD_READ_BLOCK1, q, sizeof(q), *buf, (int)len);
	if (ret != 0)
		return ret;
	*addr += len;
	*buf += len;
	return 0;
}


/*
 *  write target memory block
 *
 *  in:
 *    addr - target memory address (updated on return)
 *    buf - buffer with data to write (updated on return)
 *    len - size of data to write
 *  out:
 *    status code (errno-like)
 */

static int tbdml_write_block(uint16_t *addr, const uint8_t **buf, uint16_t len)
{
	uint8_t q[3 + TBDML_MAX_DATA_SIZE];
	int ret;

	uint16_host2le_to_buf(q + 0, *addr);
	q[2] = (uint8_t)len;
	memcpy(q + 3, *buf, len);

	if (options.tbdml_bulk)
		ret = tbdml_cmd_bulk(TBDML_CMD_WRITE_BLOCK1, q, 3 + len, NULL, 0);
	else
		ret = tbdml_cmd(TBDML_CMD_WRITE_BLOCK1, q, 3 + len, NULL, 0);
	if (ret != 0)
		return ret;
	*addr += len;
	*buf += len;
	return 0;
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

static int tbdml_read_mem(uint16_t addr, void *buf, size_t len)
{
	uint8_t *ptr;
	size_t n;
	int ret;

	if (len == 0)
		return 0;

	ptr = (uint8_t *)buf;

	if (len > TBDML_MAX_DATA_SIZE && (addr & 1) != 0)
	{
		/* if address is odd and we are going to read more than one block make the first packet 1 byte short to align the address */

		n = (TBDML_MAX_DATA_SIZE & 0xfe) - 1;
		ret = tbdml_read_block(&addr, &ptr, (uint16_t)n);
		if (ret != 0)
			return ret;
		len -= n;
	}

	while (len > TBDML_MAX_DATA_SIZE)
	{
		n = TBDML_MAX_DATA_SIZE & 0xfe;
		ret = tbdml_read_block(&addr, &ptr, (uint16_t)n);
		if (ret != 0)
			return ret;
		len -= n;
	}

	if (len != 0)
	{
		ret = tbdml_read_block(&addr, &ptr, (uint16_t)len);
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

static int tbdml_write_mem(uint16_t addr, const void *buf, size_t len)
{
	const uint8_t *ptr;
	size_t n;
	int ret;

	if (len == 0)
		return 0;

	ptr = (const uint8_t *)buf;

	if (len > TBDML_MAX_DATA_SIZE && (addr & 1) != 0)
	{
		/* if address is odd and we are going to write more than one block make the first packet 1 byte short to align the address */

		n = (TBDML_MAX_DATA_SIZE & 0xfe) - 1;
		ret = tbdml_write_block(&addr, &ptr, (uint16_t)n);
		if (ret != 0)
			return ret;
		len -= n;
	}

	while (len > TBDML_MAX_DATA_SIZE)
	{
		n = TBDML_MAX_DATA_SIZE & 0xfe;
		ret = tbdml_write_block(&addr, &ptr, (uint16_t)n);
		if (ret != 0)
			return ret;
		len -= n;
	}

	if (len != 0)
	{
		ret = tbdml_write_block(&addr, &ptr, (uint16_t)len);
		if (ret != 0)
			return ret;
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

static int tbdml_read_reg(int reg, uint16_t *v)
{
	tbdml_raw_regs_t r;
	int ret;

	ret = tbdml_cmd(TBDML_CMD_READ_REGS, NULL, 0, &r, sizeof(r));
	if (ret != 0)
		return ret;

	switch (reg)
	{
		case HCS12BDM_REG_PC:
			*v = uint16_le2host_from_buf(r.pc);
			break;
		case HCS12BDM_REG_D:
			*v = uint16_le2host_from_buf(r.d);
			break;
		case HCS12BDM_REG_X:
			*v = uint16_le2host_from_buf(r.ix);
			break;
		case HCS12BDM_REG_Y:
			*v = uint16_le2host_from_buf(r.iy);
			break;
		case HCS12BDM_REG_SP:
			*v = uint16_le2host_from_buf(r.sp);
			break;
		case HCS12BDM_REG_CCR:
			*v = uint16_le2host_from_buf(r.ccr);
			break;
		default:
			return EINVAL;
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

static int tbdml_write_reg(int reg, uint16_t v)
{
	uint8_t cmd;
	uint8_t buf[2];

	switch (reg)
	{
		case HCS12BDM_REG_PC:
			cmd = TBDML_CMD_WRITE_REG_PC;
			break;
		case HCS12BDM_REG_D:
			cmd = TBDML_CMD_WRITE_REG_D;
			break;
		case HCS12BDM_REG_X:
			cmd = TBDML_CMD_WRITE_REG_X;
			break;
		case HCS12BDM_REG_Y:
			cmd = TBDML_CMD_WRITE_REG_Y;
			break;
		case HCS12BDM_REG_SP:
			cmd = TBDML_CMD_WRITE_REG_SP;
			break;
		case HCS12BDM_REG_CCR:
			cmd = TBDML_CMD_WRITE_REG_CCR;
			break;
		default:
			return EINVAL;
	}

	uint16_host2le_to_buf(buf, v);
	return tbdml_cmd(cmd, buf, sizeof(buf), NULL, 0);

}


/*
 *  reset target into special mode
 *
 *  in:
 *    mode - reset mode
 *  out:
 *    status code (errno-like)
 */

static int tbdml_target_reset(uint8_t mode)
{
	int ret;
	uint8_t b;

	ret = tbdml_cmd(TBDML_CMD_RESET, &mode, sizeof(mode), NULL, -1);
	if (ret == -1)
	{
		error("unable to reset target: reset line stuck\n");
		return EIO;
	}
	if (ret == 0)
	{
		sys_delay(TBDML_RESET_DELAY);
		tbdml_read_bd_byte(0xffff, &b);
	}
	return ret;
}


/*
 *  reset target into normal or special mode
 *
 *  in:
 *    mode - reset mode
 *  out:
 *    status code (errno-like)
 */

static int tbdml_reset_normal(void)
{
	return tbdml_target_reset(TBDML_RESET_NORMAL_MODE);
}


static int tbdml_reset_special(void)
{
	return tbdml_target_reset(TBDML_RESET_SPECIAL_MODE);
}


/*
 *  open POD connection
 *
 *  in:
 *    void
 *  out:
 *    status code (errno-like)
 */

static int tbdml_open(void)
{
	int ret;
	uint8_t v_hw;
	uint8_t v_sw;

	ret = sys_usb_open();
	if (ret != 0)
		return ret;

	ret = sys_usb_device_open(&tbdml_device, TBDML_USB_VID, TBDML_USB_PID);
	if (ret != 0)
		goto error;

	ret = tbdml_get_version(&v_hw, &v_sw);
	if (ret != 0)
		goto error;

	if (options.verbose)
	{
		printf("TBDML hardware version <%u.%u> firmware version <%u.%u>\n",
		       (unsigned int)(v_hw >> 4),
		       (unsigned int)(v_hw & 0x0f),
		       (unsigned int)(v_sw >> 4),
		       (unsigned int)(v_sw & 0x0f));
	}

	ret = tbdml_set_target_type(TBDML_TARGET_HC12);
	if (ret != 0)
		goto error;

	if (options.osc == 0)
	{
		ret = tbdml_target_sync();
		if (ret != 0)
			goto error;

		ret = tbdml_get_speed(&options.osc);
		if (ret != 0)
			goto error;

		if (options.verbose)
		{
			printf("TBDML detected target frequency <%u.%03u MHz>\n",
			       (unsigned int)(options.osc / 1000000UL),
			       (unsigned int)((options.osc % 1000000UL) / 1000UL));
		}
	}
	else
	{
		ret = tbdml_set_speed(options.osc);
		if (ret != 0)
			goto error;
	}

	return 0;
error:
	sys_usb_close();
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

static int tbdml_close(void)
{
	sys_usb_device_close(&tbdml_device);
	sys_usb_close();
	return 0;
}


/* POD handler */

hcs12bdm_handler_t tbdml_bdm_handler =
{
	tbdml_open,
	tbdml_close,
	tbdml_reset_normal,
	tbdml_reset_special,
	tbdml_background,
	NULL,
	NULL,
	tbdml_read_bd_byte,
	NULL,
	tbdml_read_byte,
	tbdml_read_word,
	tbdml_write_bd_byte,
	NULL,
	tbdml_write_byte,
	tbdml_write_word,
	tbdml_read_mem,
	tbdml_write_mem,
	NULL,
	tbdml_read_reg,
	NULL,
	tbdml_write_reg,
	tbdml_go,
	NULL,
	tbdml_trace1,
	NULL
};
