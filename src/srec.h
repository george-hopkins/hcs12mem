/*
    hcs12mem - HC12/S12 memory reader & writer
    Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>

    srec.h: S-record file access routines

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

#ifndef __SREC_H
#define __SREC_H

/* Motorola S-record types */

#define SREC_HEADER       'S'
#define SREC_TYPE_INFO    '0'
#define SREC_TYPE_A16     '1'
#define SREC_TYPE_A24     '2'
#define SREC_TYPE_A32     '3'
#define SREC_TYPE_REC_NUM '5'
#define SREC_TYPE_A32_END '7'
#define SREC_TYPE_A24_END '8'
#define SREC_TYPE_A16_END '9'

/* max S-record line length:
   2 chars: type
   2 chars: record length
   8 chars: address
   255*2 chars: data
   2 chars: checksum
   2 chars: CR/LF
   1 char: NUL */

#define SREC_LINE_LEN_MAX (2 + 2 + 8 + 255 * 2 + 2 + 2 + 1)

int srec_read(
	const char *file,
	char *info,
	size_t info_len,
	void *buf,
	size_t buf_len,
	uint32_t *entry_raw,
	uint32_t *entry,
	uint32_t *addr_min,
	uint32_t *addr_max,
	uint32_t (*atc)(uint32_t addr)
	);

#define SREC_ENTRY_MODE_RAW       0
#define SREC_ENTRY_MODE_TRANSLATE 1

int srec_write(
	const char *file,
	const char *info,
	uint32_t addr,
	size_t len,
	uint8_t *buf,
	uint32_t entry,
	uint32_t (*atc)(uint32_t addr),
	int skip_empty,
	size_t block_size,
	int entry_mode
	);

#endif /* __SREC_H */
