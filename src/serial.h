/*
    hc12mem - HC12 memory reader & writer
    serial.h: serial port access routines
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

#ifndef __SERIAL_H
#define __SERIAL_H

#include <stdlib.h>

/* char size */

typedef enum
{
	SERIAL_CFG_CHAR_SIZE_5,
	SERIAL_CFG_CHAR_SIZE_6,
	SERIAL_CFG_CHAR_SIZE_7,
	SERIAL_CFG_CHAR_SIZE_8
}
serial_cfg_char_size_t;

/* parity bit */

typedef enum
{
	SERIAL_CFG_PARITY_NONE,
	SERIAL_CFG_PARITY_EVEN,
	SERIAL_CFG_PARITY_ODD
}
serial_cfg_parity_t;

/* stop bits */

typedef enum
{
	SERIAL_CFG_STOP_BITS_1,
	SERIAL_CFG_STOP_BITS_2
}
serial_cfg_stop_bits_t;

/* handshake mode */

typedef enum
{
	SERIAL_CFG_HANDSHAKE_UNKNOWN,
	SERIAL_CFG_HANDSHAKE_NONE,
	SERIAL_CFG_HANDSHAKE_RTSCTS
}
serial_cfg_handshake_t;

/* serial port config */

typedef struct
{
	unsigned long baud_rate;
	serial_cfg_char_size_t char_size;
	serial_cfg_parity_t parity;
	serial_cfg_stop_bits_t stop_bits;
	serial_cfg_handshake_t handshake;
}
serial_cfg_t;

/* control functions selection */

typedef enum
{
	SERIAL_CONTROL_SET_RTS,
	SERIAL_CONTROL_GET_CTS
}
serial_control_t;

typedef struct
{
	char path[SYS_MAX_PATH + 1];
#if SYS_TYPE_UNIX
	int tty;
#endif
#if SYS_TYPE_WIN32
	HANDLE port_handle;
	HANDLE read_event;
	HANDLE write_event;
	DWORD read_timeout;
	DWORD write_timeout;
#endif
}
serial_t;

/* functions */

int serial_open(serial_t *s, const char *path);
int serial_close(serial_t *s);
int serial_get_cfg(serial_t *s, serial_cfg_t *cfg);
int serial_set_cfg(serial_t *s, const serial_cfg_t *cfg);
int serial_read(serial_t *s, void *data, size_t size, unsigned long timeout);
int serial_write(serial_t *s, const void *data, size_t size, unsigned long timeout);
int serial_flush(serial_t *s);
int serial_control(serial_t *s, serial_control_t c, int *state);

#endif /* __SERIAL_H */
