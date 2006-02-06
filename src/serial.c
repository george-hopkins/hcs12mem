/*
    hc12mem - HC12 memory reader & writer
    serial.c: serial port access routines
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
#include "serial.h"

#if SYS_TYPE_UNIX
#  include <fcntl.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <sys/ioctl.h>
#  include <sys/time.h>
#  include <termios.h>
#endif


#if SYS_TYPE_UNIX

/*
 *  open serial port
 *
 *  in:
 *    s - serial port handle
 *    path - serial port path
 *  out:
 *    status code (errno-like)
 */

int serial_open(serial_t *s, const char *path)
{
	int ret;

	strlcpy(s->path, path, sizeof(s->path));
	s->tty = open(path, O_RDWR | O_NOCTTY | O_NDELAY);
	if (s->tty == -1)
	{
		ret = errno;
		error("unable to open %s (%s)\n",
		      (const char *)path,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  close serial port
 *
 *  in:
 *    s - serial port handle
 *  out:
 *    status code (errno-like)
 */

int serial_close(serial_t *s)
{
	int ret;

	if (s->tty == -1)
		return EINVAL;

	ret = 0;
	if (close(s->tty) == -1)
	{
		ret = errno;
		error("unable to close %s (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
	}

	s->tty = -1;
	s->path[0] = '\0';

	return ret;
}


/* baud rates table */

static const struct
{
	unsigned long br;
	speed_t speed;
}
serial_speed_table[] =
{
	{     50,    B50 },
	{     75,    B75 },
	{    110,   B110 },
	{    134,   B134 },
	{    150,   B150 },
	{    200,   B200 },
	{    300,   B300 },
	{    600,   B600 },
	{   1200,  B1200 },
	{   1800,  B1800 },
	{   2400,  B2400 },
	{   4800,  B4800 },
	{   9600,  B9600 },
	{  19200, B19200 },
	{  38400, B38400 },
#	ifdef B57600
	{  57600, B57600 },
#	endif
#	ifdef B115200
	{ 115200, B115200 },
#	endif
	{ 0, 0 }
};

#define SERIAL_SPEED_TABLE_SIZE \
	(sizeof(serial_speed_table) / sizeof(serial_speed_table[0]))


/*
 *  get serial port configuration
 *
 *  in:
 *    s - serial port handle
 *    cfg - on return, serial port configuration
 *  out:
 *    status code (errno-like)
 */

int serial_get_cfg(serial_t *s, serial_cfg_t *cfg)
{
	speed_t speed;
	struct termios params;
	int i;
	int ret;

	if (tcgetattr(s->tty, &params) == -1)
	{
		ret = errno;
		error("unable to get %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	speed = cfgetospeed(&params);
	for (i = 0; i < SERIAL_SPEED_TABLE_SIZE; ++ i)
	{
		if (speed == serial_speed_table[i].speed)
		{
			cfg->baud_rate = serial_speed_table[i].br;
			break;
		}
	}
	if (i == SERIAL_SPEED_TABLE_SIZE)
		cfg->baud_rate = 0;

	switch (params.c_cflag & CSIZE)
	{
		case CS5:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_5;
			break;

		case CS6:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_6;
			break;

		case CS7:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_7;
			break;

		case CS8:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_8;
			break;
	}

	if (!(params.c_cflag & PARENB))
		cfg->parity = SERIAL_CFG_PARITY_NONE;
	else
	{
		if (params.c_cflag & PARODD)
			cfg->parity = SERIAL_CFG_PARITY_ODD;
		else
			cfg->parity = SERIAL_CFG_PARITY_EVEN;
	}

	if (params.c_cflag & CSTOPB)
		cfg->stop_bits = SERIAL_CFG_STOP_BITS_2;
	else
		cfg->stop_bits = SERIAL_CFG_STOP_BITS_1;

	if (params.c_cflag &= CRTSCTS)
		cfg->handshake = SERIAL_CFG_HANDSHAKE_RTSCTS;
	else
		cfg->handshake = SERIAL_CFG_HANDSHAKE_NONE;

	return 0;
}


/*
 *  set serial port configuration
 *
 *  in:
 *    s - serial port handle
 *    cfg - serial port configuration
 *  out:
 *    status code (errno-like)
 */

int serial_set_cfg(serial_t *s, const serial_cfg_t *cfg)
{
	struct termios attr;
	int i;
	int ret;

	if (tcgetattr(s->tty, &attr) == -1)
	{
		ret = errno;
		error("unable to get %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	for (i = 0; i < SERIAL_SPEED_TABLE_SIZE; ++ i)
	{
		if (cfg->baud_rate == serial_speed_table[i].br)
			break;
	}
	if (i == SERIAL_SPEED_TABLE_SIZE)
	{
		error("invalid baud rate: %lu\n",
		      (unsigned long)cfg->baud_rate);
		return EINVAL;
	}

	cfsetispeed(&attr, serial_speed_table[i].speed);
	cfsetospeed(&attr, serial_speed_table[i].speed);

#if HAVE_CFMAKERAW
	cfmakeraw(&attr);
#else
	attr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
	attr.c_oflag &= ~OPOST;
	attr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
	attr.c_cflag &= ~(CSIZE | PARENB);
	attr.c_cflag |= CS8;
#endif

	attr.c_iflag &= ~(IGNBRK | BRKINT | IGNPAR | PARMRK | INPCK | ISTRIP | INLCR | IGNCR | ICRNL | IXON | IXANY | IXOFF | IMAXBEL);
	attr.c_oflag &= ~(OPOST | ONLCR | OCRNL | ONOCR | ONLRET);
	attr.c_lflag &= ~(ISIG | ICANON | ECHO | ECHOE | ECHOK | ECHONL | ECHOCTL | ECHOKE | FLUSHO | NOFLSH | TOSTOP | IEXTEN);
#ifdef ECHOPRT
	attr.c_lflag &= ~ECHOPRT;
#endif
#ifdef PENDIN
	attr.c_lflag &= ~PENDIN;
#endif
	attr.c_cflag &= ~(CSIZE | CSTOPB | PARENB | PARODD | CRTSCTS);
	attr.c_cflag |= (CREAD | CLOCAL | HUPCL);

	switch (cfg->char_size)
	{
		case SERIAL_CFG_CHAR_SIZE_5:
			attr.c_cflag |= CS5;
			break;
		case SERIAL_CFG_CHAR_SIZE_6:
			attr.c_cflag |= CS6;
			break;
		case SERIAL_CFG_CHAR_SIZE_7:
			attr.c_cflag |= CS7;
			break;
		case SERIAL_CFG_CHAR_SIZE_8:
			attr.c_cflag |= CS8;
			break;
	}

	switch (cfg->parity)
	{
		case SERIAL_CFG_PARITY_NONE:
			break;
		case SERIAL_CFG_PARITY_EVEN:
			attr.c_cflag |= PARENB;
			break;
		case SERIAL_CFG_PARITY_ODD:
			attr.c_cflag |= PARENB;
			attr.c_cflag |= PARODD;
			break;
	}

	switch (cfg->stop_bits)
	{
		case SERIAL_CFG_STOP_BITS_1:
			break;
		case SERIAL_CFG_STOP_BITS_2:
			attr.c_cflag |= CSTOPB;
			break;
	}

	switch (cfg->handshake)
	{
		case SERIAL_CFG_HANDSHAKE_NONE:
		case SERIAL_CFG_HANDSHAKE_UNKNOWN:
			break;
		case SERIAL_CFG_HANDSHAKE_RTSCTS:
			attr.c_cflag |= CRTSCTS;
			break;
	}

	if (tcsetattr(s->tty, TCSANOW, &attr) == -1)
	{
		ret = errno;
		error("unable to set %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  update timeout
 */

static int serial_timeout(struct timeval *start, unsigned long *to, struct timeval *sto)
{
	struct timeval t;
	long diff;

	if (start->tv_sec == 0)
	{
		/* get operation start time */

		gettimeofday(start, NULL);
	}
	else
	{
		/* get current time in the middle of the operation */

	  	back:
		gettimeofday(&t, NULL);

		/* if time went back, restart timeout counting */

		if (t.tv_sec < start->tv_sec || (t.tv_sec == start->tv_sec && t.tv_usec < start->tv_usec))
		{
			gettimeofday(start, NULL);
			goto back;
		}

		/* calculate time difference (now - start) */

		if (t.tv_sec == start->tv_sec)
			diff = (t.tv_usec - start->tv_usec) / (long)1000;
		else
		{
			if (t.tv_usec >= start->tv_usec)
			{
				diff = (t.tv_usec - start->tv_usec) / (long)1000;
				diff += (t.tv_sec - start->tv_sec) * (long)1000;
			}
			else
			{
				diff = ((long)1000000 + t.tv_usec - start->tv_usec) / (long)1000;
				diff += (t.tv_sec - start->tv_sec - (long)1) * (long)1000;
			}
		}

		/* if time has elapsed, this is timeout */

		if ((unsigned long)diff >= *to)
			return TRUE;

		/* update timeout with already passed time */

		*to -= (unsigned long)diff;
	}

	/* calculate timeout for select function */

	sto->tv_sec = (long)(*to / 1000);
	sto->tv_usec = (long)(*to % 1000) * 1000UL;

	return FALSE;
}


/*
 *  read from serial port
 *
 *  in:
 *    s - serial port handle
 *    data - buffer for read data
 *    size - data size to read
 *    timeout - read timeout, milliseconds
 *  out:
 *    status code (errno-like)
 */

int serial_read(serial_t *s, void *data, size_t *size, unsigned long timeout)
{
	size_t n;
	unsigned char *ptr;
	struct timeval start;
	struct timeval sto;
	fd_set fdset;
	int ret;

	n = *size;
	*size = 0;
	ptr = (unsigned char *)data;

	timerclear(&start);
	while (*size < n)
	{
		if (timeout != 0xffffffff)
		{
			if (serial_timeout(&start, &timeout, &sto))
				return ETIMEDOUT;

			/* check file descriptor */

			FD_ZERO(&fdset);
			FD_SET(s->tty, &fdset);
			ret = select(s->tty + 1, &fdset, (fd_set *)NULL, (fd_set *)NULL, &sto);
			if (ret == 0)
				return ETIMEDOUT;
			if (ret == -1)
			{
				if (errno == EINTR || errno == EAGAIN)
					continue;
				goto error;
			}
		}

		ret = (int)read(s->tty, ptr, n - *size);
		if (ret == -1)
		{
			if (errno == EINTR || errno == EAGAIN)
				continue;
		  error:
			ret = errno;
			error("unable to read %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
			return ret;
		}

		*size += (size_t)ret;
		ptr += ret;
	}

	return 0;
}


/*
 *  write to serial port
 *
 *  TODO: use timeout, currently it's ignored
 *
 *  in:
 *    s - serial port handle
 *    data - data buffer
 *    size - data size to write
 *    timeout - write timeout, milliseconds
 *  out:
 *    status code (errno-like)
 */

int serial_write(serial_t *s, const void *data, size_t *size, unsigned long timeout)
{
	size_t n;
	const unsigned char *ptr;
	struct timeval start;
	struct timeval sto;
	fd_set fdset;
	int ret;

	n = *size;
	*size = 0;
	ptr = (const unsigned char *)data;

	if (timeout == 0)
		return EINVAL;

	timerclear(&start);
	while (*size < n)
	{
		if (timeout != 0xffffffff)
		{
			if (serial_timeout(&start, &timeout, &sto))
			{
				errno = ETIMEDOUT;
				goto error;
			}

 			/* check file descriptor */

			FD_ZERO(&fdset);
			FD_SET(s->tty, &fdset);
			ret = select(s->tty + 1, (fd_set *)NULL, &fdset, (fd_set *)NULL, &sto);
			if (ret == 0)
			{
				errno = ETIMEDOUT;
				goto error;
			}
			if (ret == -1)
			{
				if (errno == EINTR || errno == EAGAIN)
					continue;
				goto error;
			}
		}

		ret = (int)write(s->tty, ptr, n - *size);
		if (ret == -1)
		{
			if (errno == EINTR || errno == EAGAIN)
				continue;
		  error:
			ret = errno;
			error("unable to write %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
			return ret;
		}

		*size += (size_t)ret;
		ptr += ret;
	}

	return 0;
}


/*
 *  flush serial port
 *
 *  in:
 *    s - serial port handle
 *  out:
 *    status code (errno-like)
 */

int serial_flush(serial_t *s)
{
	int ret;

	if (tcflush(s->tty, TCIOFLUSH) == -1)
	{
		ret = errno;
		error("unable to flush %s (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  serial port control functions
 *
 *  in:
 *    s - serial port handle
 *    c - control function selection
 *    state - state for control function
 *  out:
 *    status code (errno-like)
 */

int serial_control(serial_t *s, serial_control_t c, int *state)
{
	int tiocm;
	unsigned int v;
	int ret;

	switch (c)
	{
		case SERIAL_CONTROL_SET_RTS:
			tiocm = TIOCM_RTS;
			if (ioctl(s->tty, *state ? TIOCMBIS : TIOCMBIC, &tiocm) == -1)
			{
				ret = errno;
				error("unable to set RTS state for %s (%s)\n",
				      (const char *)s->path,
				      (const char *)strerror(ret));
				return ret;
			}
			return 0;

		case SERIAL_CONTROL_GET_CTS:
			if (ioctl(s->tty, TIOCMGET, &v) == -1)
			{
				ret = errno;
				error("unable to get CTS state for %s (%s)\n",
				      (const char *)s->path,
				      (const char *)strerror(ret));
				return ret;
			}
			*state = (v & TIOCM_CTS) ? TRUE : FALSE;
			return 0;
	}

	return EINVAL;
}

#endif


#ifdef SYS_TYPE_WIN32

/*
 *  init serial port data structures
 */

static void serial_init(serial_t *s)
{
	s->port_handle = INVALID_HANDLE_VALUE;
	s->read_event = INVALID_HANDLE_VALUE;
	s->write_event = INVALID_HANDLE_VALUE;
	s->read_timeout = INFINITE;
	s->write_timeout = INFINITE;
}

/*
 *  open serial port
 *
 *  in:
 *    s - serial port handle
 *    path - serial port path
 *  out:
 *    status code (errno-like)
 */

int serial_open(serial_t *s, const char *path)
{
	COMMTIMEOUTS to;
	int ret;

	serial_init(s);

	/* create read event */

	s->read_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (s->read_event == NULL)
	{
		ret = sys_get_error();
		goto ret_error;
	}

	/* create write event */

	s->write_event = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (s->write_event == NULL)
	{
		ret = sys_get_error();
		goto ret_error;
	}

	/* open port device */

	snprintf(s->path, sizeof(s->path), "\\\\.\\%s", (const char *)path);
	s->port_handle = CreateFile(s->path, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (s->port_handle == INVALID_HANDLE_VALUE)
	{
		/* when given serial port doesn't exist, WIN9x CreateFile returns
		ERROR_GEN_FAILURE, WinNT returns ERROR_FILE_NOT_FOUND,
		so we unify the answer here */

		ret = sys_get_error();
		if (ret == ERROR_GEN_FAILURE)
			ret = ERROR_FILE_NOT_FOUND;
		error("unable to open %s (%s)\n",
		      (const char *)path,
		      (const char *)strerror(ret));
		goto ret_error;
	}

	/* set initial timeouts to infinity */

	to.ReadIntervalTimeout = 0;
	to.ReadTotalTimeoutMultiplier = 0;
	to.ReadTotalTimeoutConstant = 0;
	to.WriteTotalTimeoutMultiplier = 0;
	to.WriteTotalTimeoutConstant = 0;

	if (SetCommTimeouts(s->port_handle, &to) == FALSE)
	{
		ret = sys_get_error();
		goto ret_error;
	}

	return 0;

	/* error return */

ret_error:
	serial_close(s);
	return ret;
}


/*
 *  close serial port
 *
 *  in:
 *    s - serial port handle
 *  out:
 *    status code (errno-like)
 */

int serial_close(serial_t *s)
{
	int ret = 0;

	if (s->port_handle != INVALID_HANDLE_VALUE)
	{
		EscapeCommFunction(s->port_handle, CLRDTR);
		EscapeCommFunction(s->port_handle, CLRRTS);
		serial_flush(s);
		if (!CloseHandle(s->port_handle))
		{
			ret = sys_get_error();
			error("unable to close %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
		}
	}

	if (s->read_event != INVALID_HANDLE_VALUE)
		CloseHandle(s->read_event);

	if (s->write_event != INVALID_HANDLE_VALUE)
		CloseHandle(s->write_event);

	serial_init(s);

	return ret;
}


/*
 *  get serial port configuration
 *
 *  in:
 *    s - serial port handle
 *    cfg - on return, serial port configuration
 *  out:
 *    status code (errno-like)
 */

int serial_get_cfg(serial_t *s, serial_cfg_t *cfg)
{
	DCB dcb;
	int ret;

	/* get current config */

	if (!GetCommState(s->port_handle, &dcb))
	{
		ret = sys_get_error();
		error("unable to get %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	/* baud rate is straightforward */

	cfg->baud_rate = (unsigned long)dcb.BaudRate;

	/* byte size */

	switch (dcb.ByteSize)
	{
		case 5:
			cfg->char_size= SERIAL_CFG_CHAR_SIZE_5;
			break;
		case 6:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_6;
			break;
		case 7:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_7;
			break;
		case 8:
			cfg->char_size = SERIAL_CFG_CHAR_SIZE_8;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* parity bit */

	switch (dcb.Parity)
	{
		case NOPARITY:
			cfg->parity = SERIAL_CFG_PARITY_NONE;
			break;
		case EVENPARITY:
			cfg->parity = SERIAL_CFG_PARITY_EVEN;
			break;
		case ODDPARITY:
			cfg->parity = SERIAL_CFG_PARITY_ODD;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* stop bits */

	switch (dcb.StopBits)
	{
		case ONESTOPBIT:
			cfg->stop_bits = SERIAL_CFG_STOP_BITS_1;
			break;
		case TWOSTOPBITS:
			cfg->stop_bits = SERIAL_CFG_STOP_BITS_2;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* handshake mode */

	if (dcb.fDtrControl == DTR_CONTROL_ENABLE && dcb.fRtsControl == RTS_CONTROL_ENABLE)
		cfg->handshake = SERIAL_CFG_HANDSHAKE_NONE;
	else if (dcb.fDtrControl == DTR_CONTROL_ENABLE && dcb.fRtsControl == RTS_CONTROL_HANDSHAKE)
		cfg->handshake = SERIAL_CFG_HANDSHAKE_RTSCTS;
	else
		cfg->handshake = SERIAL_CFG_HANDSHAKE_UNKNOWN;

	return 0;
}


/*
 *  set serial port configuration
 *
 *  in:
 *    s - serial port handle
 *    cfg - serial port configuration
 *  out:
 *    status code (errno-like)
 */

int serial_set_cfg(serial_t *s, const serial_cfg_t *cfg)
{
	DCB dcb;
	int ret;

	/* get current config */

	if (!GetCommState(s->port_handle, &dcb))
	{
		ret = sys_get_error();
		error("unable to get %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	/* set some constant settings */

	dcb.fBinary = TRUE;
	dcb.fParity = FALSE;
	dcb.fOutX = FALSE;
	dcb.fInX = FALSE;
	dcb.fErrorChar = FALSE;
	dcb.fNull = FALSE;
	dcb.fAbortOnError = FALSE;

	/* baud rate */

	dcb.BaudRate = (DWORD)cfg->baud_rate;

	/* char size */

	switch (cfg->char_size)
	{
		case SERIAL_CFG_CHAR_SIZE_5:
			dcb.ByteSize = 5;
			break;
		case SERIAL_CFG_CHAR_SIZE_6:
			dcb.ByteSize = 6;
			break;
		case SERIAL_CFG_CHAR_SIZE_7:
			dcb.ByteSize = 7;
			break;
		case SERIAL_CFG_CHAR_SIZE_8:
			dcb.ByteSize = 8;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* parity bit */

	switch (cfg->parity)
	{
		case SERIAL_CFG_PARITY_NONE:
			dcb.Parity = NOPARITY;
			break;
		case SERIAL_CFG_PARITY_EVEN:
			dcb.Parity = EVENPARITY;
			break;
		case SERIAL_CFG_PARITY_ODD:
			dcb.Parity = ODDPARITY;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* stop bits */

	switch (cfg->stop_bits)
	{
		case SERIAL_CFG_STOP_BITS_1:
			dcb.StopBits = ONESTOPBIT;
			break;
		case SERIAL_CFG_STOP_BITS_2:
			dcb.StopBits = TWOSTOPBITS;
			break;
		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* handshake mode */

	switch (cfg->handshake)
	{
		case SERIAL_CFG_HANDSHAKE_UNKNOWN:
			break;

		case SERIAL_CFG_HANDSHAKE_NONE:
			dcb.fOutxCtsFlow = FALSE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDsrSensitivity = FALSE;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
			dcb.fRtsControl = RTS_CONTROL_ENABLE;
			break;

		case SERIAL_CFG_HANDSHAKE_RTSCTS:
			dcb.fOutxCtsFlow = TRUE;
			dcb.fOutxDsrFlow = FALSE;
			dcb.fDsrSensitivity = FALSE;
			dcb.fDtrControl = DTR_CONTROL_ENABLE;
			dcb.fRtsControl = RTS_CONTROL_HANDSHAKE;
			break;

		default:
			return ERROR_INVALID_PARAMETER;
	}

	/* set new config */

	if (!SetCommState(s->port_handle, &dcb))
	{
		ret = sys_get_error();
		error("unable to change %s settings (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  set serial port timeouts
 */

static int serial_set_timeout(serial_t *s, DWORD rd, DWORD wr)
{
	COMMTIMEOUTS to;
	int ret;

	if (rd != s->read_timeout || wr != s->write_timeout)
	{
		/* read timeout */

		if (rd == 0)
		{
			/* timeout = 0: no timeout, return immediately */

			to.ReadIntervalTimeout = MAXDWORD;
			to.ReadTotalTimeoutMultiplier = 0;
			to.ReadTotalTimeoutConstant = 0;
		}
		else if (rd == INFINITE)
		{
			/* no timeout: wait for data till it arrives */

			to.ReadIntervalTimeout = 0;
			to.ReadTotalTimeoutMultiplier = 0;
			to.ReadTotalTimeoutConstant = 0;
		}
		else
		{
			/* otherwise we have specific timeout */

			/* timeout between any two consecutive bytes (0 - not used) */

			to.ReadIntervalTimeout = 0;

			/* total timeout = ReadTotalTimeoutConstant + ReadTotalTimeoutMultiplier * size */

			to.ReadTotalTimeoutMultiplier = 0;
			to.ReadTotalTimeoutConstant = rd;
		}

		/* write timeout */

		if (wr == 0)
		{
			/* timeout = 0 is invalid */

			return ERROR_INVALID_PARAMETER;
		}
		else if (wr == INFINITE)
		{
			/* no timeout: try till data is sent */

			to.WriteTotalTimeoutMultiplier = 0;
			to.WriteTotalTimeoutConstant = 0;
		}
		else
		{
			/* otherwise we have specific timeout */

			to.WriteTotalTimeoutMultiplier = 0;
			to.WriteTotalTimeoutConstant = wr;
		}

		/* remember new values */

		s->read_timeout = rd;
		s->write_timeout = wr;

		/* set new timeouts */

		if (!SetCommTimeouts(s->port_handle, &to))
		{
			ret = sys_get_error();
			error("unable to set timeouts for %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
			return ret;
		}
	}

	return 0;
}


/*
 *  read from serial port
 *
 *  in:
 *    s - serial port handle
 *    data - buffer for read data
 *    size - data size to read
 *    timeout - read timeout, milliseconds
 *  out:
 *    status code (errno-like)
 */

int serial_read(serial_t *s, void *data, size_t *size, unsigned long timeout)
{
	OVERLAPPED o;
	DWORD n;
	int ret;

	ret = serial_set_timeout(s, (DWORD)timeout, s->write_timeout);
	if (ret != 0)
		return ret;

	o.hEvent = s->read_event;
	o.Offset = 0;
	o.OffsetHigh = 0;

	if (!ReadFile(s->port_handle, data, (DWORD)(*size), &n, &o))
	{
		n = GetLastError();
		if (n == ERROR_IO_PENDING)
		{
			if (!GetOverlappedResult(s->port_handle, &o, &n, TRUE))
				goto ret_error;
			if ((size_t)n != *size)
				return ERROR_TIMEOUT;
		}
		else
		{
			ret_error:
			ret = sys_get_error();
			error("unable to read %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
			return ret;
		}
	}

	return 0;
}


/*
 *  write to serial port
 *
 *  in:
 *    s - serial port handle
 *    data - data buffer
 *    size - data size to write
 *    timeout - write timeout, milliseconds
 *  out:
 *    status code (errno-like)
 */

int serial_write(serial_t *s, const void *data, size_t *size, unsigned long timeout)
{
	OVERLAPPED o;
	DWORD n;
	int ret;

	ret = serial_set_timeout(s, s->read_timeout, (DWORD)timeout);
	if (ret != 0)
		return ret;

	o.hEvent = s->write_event;
	o.Offset = 0;
	o.OffsetHigh = 0;

	if (!WriteFile(s->port_handle, data, (DWORD)(*size), &n, &o))
	{
		n = GetLastError();
		if (n == ERROR_IO_PENDING)
		{
			if (!GetOverlappedResult(s->port_handle, &o, &n, TRUE))
				goto ret_error;
			if ((size_t)n != *size)
				return ERROR_TIMEOUT;
		}
		else
		{
			ret_error:
			ret = sys_get_error();
			error("unable to read %s (%s)\n",
			      (const char *)s->path,
			      (const char *)strerror(ret));
			return ret;
		}
	}

	return 0;
}


/*
 *  flush serial port
 *
 *  in:
 *    s - serial port handle
 *  out:
 *    status code (errno-like)
 */

int serial_flush(serial_t *s)
{
	int ret;

	if (!PurgeComm(s->port_handle, PURGE_RXABORT | PURGE_RXCLEAR | PURGE_TXABORT | PURGE_TXCLEAR))
	{
		ret = sys_get_error();
		error("unable to flush %s (%s)\n",
		      (const char *)s->path,
		      (const char *)strerror(ret));
		return ret;
	}

	return 0;
}


/*
 *  serial port control functions
 *
 *  in:
 *    s - serial port handle
 *    c - control function selection
 *    state - state for control function
 *  out:
 *    status code (errno-like)
 */

int serial_control(serial_t *s, serial_control_t c, int *state)
{
	DWORD status;
	int ret;

	switch (c)
	{
		case SERIAL_CONTROL_SET_RTS:
			if (!EscapeCommFunction(s->port_handle, *state ? SETRTS : CLRRTS))
			{
				ret = sys_get_error();
				error("unable to set RTS state for %s (%s)\n",
				      (const char *)s->path,
				      (const char *)strerror(ret));
				return ret;
			}
			return 0;

		case SERIAL_CONTROL_GET_CTS:
			if (!GetCommModemStatus(s->port_handle, &status))
			{
				ret = sys_get_error();
				error("unable to get CTS state for %s (%s)\n",
				      (const char *)s->path,
				      (const char *)strerror(ret));
				return ret;
			}
			*state = (status & MS_CTS_ON) ? TRUE : FALSE;
			return 0;
	}

	return ERROR_INVALID_PARAMETER;
}

#endif
