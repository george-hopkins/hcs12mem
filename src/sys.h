/*
    hc12mem - HC12 memory reader & writer
    sys.h: operating system interface routines
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

#ifndef __SYS_H
#define __SYS_H

/*
 * system type and debug flags defines:
 *   SYS_TYPE_[UNIX|WINDOWS]
 *   DEBUG for debug build
 *   NDEBUG for release build
 */

#ifdef _WIN32
#  define SYS_TYPE_WIN32 1
#  ifdef _DEBUG
#    define DEBUG 1
#    undef NDEBUG
#  else
#    undef DEBUG
#    define NDEBUG 1
#  endif
#else
#  define SYS_TYPE_UNIX 1
#  if HAVE_CONFIG_H
#    include <config.h>
#  else
#    error autoconf config.h required
#  endif
#endif

/* unix specific */

#ifdef SYS_TYPE_UNIX

#if !STDC_HEADERS
#  error stdc headers required
#endif
#if HAVE_STDIO_H
#  include <stdio.h>
#else
#  error stdio.h required
#endif
#if HAVE_STDLIB_H
#  include <stdlib.h>
#else
#  error stdlib.h required
#endif
#if HAVE_UNISTD_H
#  include <unistd.h>
#else
#  error unistd.h required
#endif
#if HAVE_ERRNO_H
#  include <errno.h>
#else
#  error errno.h required
#endif
#if HAVE_STRING_H
#  include <string.h>
#else
#  error string.h required
#endif
#if HAVE_STDARG_H
#  include <stdarg.h>
#else
#  error stdarg.h required
#endif
#if HAVE_CTYPE_H
#  include <ctype.h>
#else
#  error ctype.h required
#endif

#define SYS_PATH_SEPARATOR '/'
#define SYS_MAX_PATH 256

#define sys_get_error() (errno)
#define sys_set_error(n) (errno = (n))

#if WORDS_BIGENDIAN /* supplied by autoconf */
# define SYS_ARCH_BIG_ENDIAN 1
#else
# define SYS_ARCH_LITTLE_ENDIAN 1
#endif

#if HAVE_STDINT_H
# include <stdint.h>
#elif HAVE_INTTYPES_H
# include <inttypes.h>
#else
# if (SIZEOF_CHAR == 1)
    typedef   signed char  int8_t;
    typedef unsigned char uint8_t;
# else
#   error Unexpected sizeof(char) !
# endif
# if (SIZEOF_SHORT == 2)
    typedef   signed short  int16_t;
    typedef unsigned short uint16_t;
# else
#   error Unexpected sizeof(short) !
# endif
# if (SIZEOF_INT == 4)
    typedef   signed int  int32_t;
    typedef unsigned int uint32_t;
# else
#   error Unexpected sizeof(int) !
# endif
# if (SIZEOF_LONG == 4)
    /* this would require checking compiler long long support first */
    typedef   signed long long  int64_t;
    typedef unsigned long long uint64_t;
# elif (SIZEOF_LONG == 8)
    typedef   signed long  int64_t;
    typedef unsigned long uint64_t;
# else
#   error Unexpected sizeof(long) !
# endif
    typedef   signed long  intptr_t;
    typedef unsigned long uintptr_t;
#endif

#endif /* SYS_TYPE_UNIX */

/* windows specific */

#ifdef SYS_TYPE_WIN32

#define HAVE_GETOPT 1
#undef  HAVE_GETOPT_LONG

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#define SYS_PATH_SEPARATOR '\\'
#define SYS_MAX_PATH MAX_PATH

#define sys_get_error() (int)GetLastError()
#define sys_set_error(n) SetLastError((DWORD)(n))

#define ETIMEDOUT ERROR_TIMEOUT
#define ENOTSUP   ERROR_NOT_SUPPORTED
#define EINVAL    ERROR_INVALID_PARAMETER
#define ENOMEM    ERROR_NOT_ENOUGH_MEMORY
#define EIO       ERROR_IO_DEVICE

#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#define strdup _strdup

#define R_OK 0x04
extern int access(const char *path, int mode);

extern int vsnprintf(char *str, size_t n, const char *format, va_list list);
extern int snprintf(char *str, size_t n, const char *format, ...);

#define SYS_ARCH_LITTLE_ENDIAN 1

typedef   signed __int8    int8_t;
typedef unsigned __int8   uint8_t;
typedef   signed __int16  int16_t;
typedef unsigned __int16 uint16_t;
typedef   signed __int32  int32_t;
typedef unsigned __int32 uint32_t;
typedef   signed __int64  int64_t;
typedef unsigned __int64 uint64_t;
typedef   signed long    intptr_t;
typedef unsigned long   uintptr_t;

#endif /* SYS_TYPE_WIN32 */

/* common for all systems */

#ifndef FALSE
#  define FALSE 0
#endif
#ifndef TRUE
#  define TRUE  1
#endif

#if !HAVE_STRLCPY
extern size_t strlcpy(char *dst, const char *src, size_t dst_size);
#endif

extern uint16_t uint16_be2host_buf(const void *buf);
extern uint16_t uint16_le2host_buf(const void *buf);
extern void uint16_host2be_buf(void *buf, uint16_t v);
extern void uint16_host2le_buf(void *buf, uint16_t v);
extern uint16_t uint16_swap(uint16_t x);

#if SYS_ARCH_LITTLE_ENDIAN
#  define uint16_le2host(x) (x)
#  define uint16_host2le(x) (x)
#  define uint16_be2host uint16_swap
#  define uint16_host2be uint16_swap
#endif
#if SYS_ARCH_BIG_ENDIAN
#  define uint16_le2host uint16_swap
#  define uint16_host2le uint16_swap
#  define uint16_be2host(x) (x)
#  define uint16_host2be(x) (x)
#endif

extern void sys_delay(unsigned long ms);
extern unsigned long sys_get_ms(void);

#endif /* __SYSTEM_H */
