/*
    hc12mem - HC12 memory reader & writer
    srec.c: S-record file access routines
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
#include "srec.h"

/*
 *  convert two chars from string into hex number
 *
 *  in:
 *    str - string to convert (2 chars are taken into conversion)
 *    b - hex number, on return
 *  out:
 *    status code (errno-like)
 */

static int srec_str2hex(const char *str, uint8_t *b)
{
	if (!isxdigit(str[0]) || !isxdigit(str[1]))
		return EINVAL;

	if (isupper(str[0]))
		*b = str[0] - 'A' + 0x0a;
	else if (islower(str[0]))
		*b = str[0] - 'a' + 0x0a;
	else
		*b = str[0] - '0';

	*b *= 0x10;

	if (isupper(str[1]))
		*b += str[1] - 'A' + 0x0a;
	else if (islower(str[1]))
		*b += str[1] - 'a' + 0x0a;
	else
		*b += str[1] - '0';

	return 0;
}


/*
 *  parse S-record line
 *
 *  in:
 *    buf - S-record text
 *    type - record type (on return)
 *    cnt - bytes count (on return)
 *    addr - block address (on return)
 *    data - data buffer (written on return)
 *  out:
 *    status code (errno-like)
 */

static int srec_parse(const char *buf,
	char *type, uint8_t *cnt, uint32_t *addr, uint8_t *data)
{
	uint8_t b;
	uint8_t sum;
	int addr_len;
	int i;

	if (*buf++ != SREC_HEADER)
		return EINVAL;

	/* record type */

	*type = *buf++;
	switch (*type)
	{
		case SREC_TYPE_INFO:
		case SREC_TYPE_REC_NUM:
		case SREC_TYPE_A16:
		case SREC_TYPE_A16_END:
			addr_len = 2;
			break;
		case SREC_TYPE_A24:
		case SREC_TYPE_A24_END:
			addr_len = 3;
			break;
		case SREC_TYPE_A32:
		case SREC_TYPE_A32_END:
			addr_len = 4;
			break;
		default:
			return EINVAL;
	}

	/* byte counter */

	if (srec_str2hex(buf, &b) != 0)
		return EINVAL;
	buf += 2;

	switch (*type)
	{
		case SREC_TYPE_REC_NUM:
		case SREC_TYPE_A16_END:
		case SREC_TYPE_A24_END:
		case SREC_TYPE_A32_END:
			if ((int)b != addr_len + 1)
				return EINVAL;
			break;

		default:
			if ((int)b < addr_len + 1)
				return EINVAL;
			break;
	}

	*cnt = (int)b - (addr_len + 1);
	sum = b;

	/* address */

	*addr = 0;
	for (i = 0; i < addr_len; ++ i)
	{
		if (srec_str2hex(buf, &b) != 0)
			return EINVAL;
		buf += 2;
		*addr = (*addr << 8) + (uint32_t)b;
		sum += b;
	}

	/* data bytes */

	for (i = 0; i < (int)(*cnt); ++ i)
	{
		if (srec_str2hex(buf, &data[i]) != 0)
			return EINVAL;
		buf += 2;
		sum += data[i];
	}

	/* control sum */

	if (srec_str2hex(buf, &b) != 0)
		return EINVAL;
	buf += 2;
	sum += b;
	if (sum != 0xff)
		return EINVAL;

	/* end of line */

	if (*buf != '\0' && *buf != '\r' && *buf != '\n')
		return EINVAL;

	return 0;
}


/*
 *  read S-record file
 *
 *  in:
 *    file - file name to read
 *    info - buffer for info record data (on return)
 *    info_len - length of buffer for info record data
 *    buf - buffer for data
 *    buf_len - data buffer length
 *    entry - entry address (on return)
 *    addr_min - minimum address encountered (on return)
 *    addr_max - maximum address encountered (on return)
 *    atc - address translation callback
 *  out:
 *    status code (errno-like)
 */

int srec_read(const char *file, char *info, size_t info_len,
	void *buf, size_t buf_len, uint32_t *entry,
	uint32_t *addr_min, uint32_t *addr_max, uint32_t (*atc)(uint32_t addr))
{
	FILE *f;
	char str[SREC_LINE_LEN_MAX + 1];
	int line;
	int ret;
	char type;
	uint8_t cnt;
	uint32_t addr;
	uint32_t addr_low;
	uint32_t addr_high;
	uint8_t data[256]; /* S-record line: max 252 bytes of data */

	if (file == NULL)
		f = stdin;
	else
	{
		f = fopen(file, "rt");
		if (f == NULL)
		{
			ret = errno;
			error("unable to open %s (%s)\n",
			      (const char *)file,
			      (const char *)strerror(ret));
			return ret;
		}
	}

	*info = '\0';
	*addr_min = (uint32_t)buf_len;
	*addr_max = 0;

	ret = 0;
	line = 0;
	while (ret == 0 && line != -1 && fgets(str, sizeof(str), f) != NULL)
	{
		++ line;
		if (strlen(str) == sizeof(str) - 1)
		{
			error("S-record line too long (line #%u)\n",
			      (unsigned int)line);
			ret = EINVAL;
			break;
		}

		if (srec_parse(str, &type, &cnt, &addr, data) != 0)
		{
			error("invalid S-record (line #%u)\n",
			      (unsigned int)line);
			ret = EINVAL;
			break;
		}

		switch (type)
		{
			case SREC_TYPE_INFO:
				data[cnt] = '\0';
				strlcpy(info, (const char *)data, info_len);
				break;

			case SREC_TYPE_REC_NUM:
				break;

			case SREC_TYPE_A16:
			case SREC_TYPE_A24:
			case SREC_TYPE_A32:
				addr_low = (*atc)(addr);
				addr_high = (*atc)(addr + (uint32_t)cnt - 1);
				if (addr_low >= (uint32_t)buf_len ||
				    addr_high >= (uint32_t)buf_len)
				{
					error("data block address out of range (line #%u)\n",
					      (unsigned int)line);
					ret = EINVAL;
				}
				else
				{
					memcpy((uint8_t *)buf + addr_low, data, (size_t)cnt);
					if (addr_low < *addr_min)
						*addr_min = addr_low;
					if (addr_high > *addr_max)
						*addr_max = addr_high;
				}
				break;

			case SREC_TYPE_A16_END:
			case SREC_TYPE_A24_END:
			case SREC_TYPE_A32_END:
				addr_low = (*atc)(addr);
				if (addr != 0 && addr_low >= (uint32_t)buf_len)
				{
					error("entry address out of range (line #%u)\n",
					      (unsigned int)line);
					ret = EINVAL;
				}
				else if (entry != NULL)
					*entry = addr_low;
				line = -1;
				break;
		}
	}

	if (fclose(f) == -1)
	{
		ret = errno;
		error("cannot close file %s (%s)\n",
		      (const char *)(file == NULL ? "<stdin>" : file),
		      (const char *)strerror(ret));
	}

	return ret;
}


/*
 *  write single S-record line
 *
 *  in:
 *    f - file to write
 *    type - record type
 *    addr - record address
 *    len - data size
 *    data - data buffer
 *  out:
 *    status code (errno-like)
 */

static int srec_write_line(FILE *f, char type,
	uint32_t addr, size_t len, const uint8_t *data)
{
	char str[SREC_LINE_LEN_MAX + 1];
	const char *fmt;
	size_t alen;
	size_t n;
	size_t i;
	uint8_t sum;
	int ret;

	switch (type)
	{
		case SREC_TYPE_INFO:
		case SREC_TYPE_REC_NUM:
		case SREC_TYPE_A16:
		case SREC_TYPE_A16_END:
			fmt = "%c%c%02X%04X";
			alen = 2;
			break;
		case SREC_TYPE_A24:
		case SREC_TYPE_A24_END:
			fmt = "%c%c%02X%06X";
			alen = 3;
			break;
		case SREC_TYPE_A32:
		case SREC_TYPE_A32_END:
			fmt = "%c%c%02X%08X";
			alen = 4;
			break;
		default:
			errno = EINVAL;
			return -1;
	}

	n = snprintf(str, sizeof(str), fmt,
		(char)SREC_HEADER,
		(char)type,
		(unsigned int)(len + alen + 1),
		(unsigned int)addr);

	sum = (uint8_t)(len + alen + 1);
	for (i = 0; i < alen; ++ i)
	{
		sum += (uint8_t)(addr & 0x000000ff);
		addr >>= 8;
	}

	for (i = 0; i < len; ++ i)
	{
		n += snprintf(str + n, sizeof(str) - n, "%02X",
			(unsigned int)data[i]);
		sum += data[i];
	}

	n += snprintf(str + n, sizeof(str) - n, "%02X\n",
		(unsigned int)(0xff - sum));

	if (fwrite(str, 1, n, f) != n)
	{
		ret = errno;
		error("cannot write file (%s)\n",
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  default address translation
 */

static uint32_t srec_addr_straight(uint32_t addr)
{
	return addr;
}


/*
 *  write S-record file
 *
 *  in:
 *    file - file name to write
 *    info - info record data
 *    addr - data starting address
 *    len - data length
 *    buf - data buffer
 *    entry - entry address
 *    atc - address translation callback
 *    skip_empty - skip empty (0xff) areas when this flag is set
 *    block_size - single S-record size
 *  out:
 *    status code (errno-like)
 */

int srec_write(const char *file, const char *info,
	uint32_t addr, size_t len, uint8_t *buf, uint32_t entry,
	uint32_t (*atc)(uint32_t addr), int skip_empty, size_t block_size)
{
	FILE *f;
	size_t n;
	size_t i;
	char type_a;
	char type_end;
	int ret;

	if (atc == NULL)
		atc = srec_addr_straight;

	f = fopen(file, "wt");
	if (f == NULL)
	{
		ret = errno;
		error("cannot open %s for writing (%s)\n",
		      (const char *)file,
		      (const char *)strerror(ret));
		return ret;
	}

	if (info != NULL)
	{
		n = strlen(info);
		if (n > 252) /* 255 less 3 bytes */
			n = 252;
		ret = srec_write_line(f, SREC_TYPE_INFO, 0, n, (const uint8_t *)info);
		if (ret != 0)
		{
			fclose(f);
			return ret;
		}
	}

	if ((*atc)(addr) + (uint32_t)len <= (uint32_t)0x00010000)
	{
		type_a = SREC_TYPE_A16;
		type_end = SREC_TYPE_A16_END;
	}
	else if ((*atc)(addr) + (uint32_t)len <= (uint32_t)0x01000000)
	{
		type_a = SREC_TYPE_A24;
		type_end = SREC_TYPE_A24_END;
	}
	else
	{
		type_a = SREC_TYPE_A32;
		type_end = SREC_TYPE_A32_END;
	}

	while (len > 0)
	{
		n = (len > block_size ? block_size : len);

		if (skip_empty)
		{
			for (i = 0; i < n; ++i)
			{
				if (buf[i] != 0xff)
					break;
			}
		}
		else
			i = 0;

		if (i != n)
		{
			ret = srec_write_line(f, type_a,
				(*atc)(addr), n, buf);
			if (ret != 0)
			{
				fclose(f);
				return ret;
			}
		}

		addr += n;
		len -= n;
		buf += n;
	}

	ret = srec_write_line(f, type_end, (*atc)(entry), 0, NULL);
	if (ret != 0)
	{
		fclose(f);
		return -1;
	}

	if (fclose(f) == -1)
	{
		ret = errno;
		error("cannot close file %s (%s)\n",
		      (const char *)file,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}
