dnl hc12mem - HC12 memory reader & writer
dnl configure.in: configure script template
dnl $Id$
dnl
dnl Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>
dnl
dnl   This program is free software; you can redistribute it and/or modify
dnl   it under the terms of the GNU General Public License as published by
dnl   the Free Software Foundation; either version 2 of the License, or
dnl   (at your option) any later version.
dnl
dnl   This program is distributed in the hope that it will be useful,
dnl   but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl   GNU General Public License for more details.
dnl
dnl   You should have received a copy of the GNU General Public License
dnl   along with this program; if not, write to the Free Software
dnl   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

AC_INIT(src/hc12mem.c)
AM_INIT_AUTOMAKE([hc12mem], [1.2])
AM_CONFIG_HEADER(config.h)
AC_CANONICAL_HOST
AC_PROG_MAKE_SET

dnl checks for programs

AC_PROG_INSTALL
AC_PROG_CC
AC_PROG_LN_S

dnl additional flags for compiler

SYSTEM=`uname -s`
if test "$SYSTEM" = "SunOS"; then
  AC_DEFINE(SYSTEM_SUNOS, 1, [System name])
  CFLAGS="$CFLAGS -D__EXTENSIONS__"
fi
if test "$SYSTEM" = "FreeBSD"; then
  AC_DEFINE(SYSTEM_FREEBSD, 1, [System name])
fi
if test "$SYSTEM" = "Linux"; then
  AC_DEFINE(SYSTEM_LINUX, 1, [System name])
  CFLAGS="$CFLAGS -D_GNU_SOURCE"
fi
if test "$SYSTEM" != "${SYSTEM#CYGWIN}"; then
  AC_DEFINE(SYSTEM_CYGWIN, 1, [System name])
  CYGWIN="1"
  AC_SUBST(CYGWIN)
fi
if test "$GCC" = "yes"; then
  if test "$CYGWIN" = "1"; then
    CFLAGS="$CFLAGS -pedantic -Wall -fsigned-char"
  else
    CFLAGS="$CFLAGS -ansi -pedantic -Wall -fsigned-char"
  fi
fi

dnl enable debugging code

AC_ARG_ENABLE([debug],
  ACX_HELP_STRING([--enable-debug],[enable use of debugging code (default=no)]),
  ,enable_debug=no)
if test "$enable_debug" = "yes" ; then
  AC_DEFINE(DEBUG, 1, [Enable debugging code])
  CFLAGS="$CFLAGS -g"
else
  AC_DEFINE(NDEBUG, 1, [Disable debugging code])
fi

dnl checks headers

AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h stdio.h stdlib.h stdarg.h stdint.h time.h errno.h)
AC_CHECK_HEADERS(limits.h string.h strings.h memory.h ctype.h inttypes.h)
AC_CHECK_HEADERS(sys/types.h sys/time.h sys/ioctl.h sys/sysctl.h)
AC_CHECK_HEADERS(sys/file.h sys/stat.h fcntl.h)
AC_CHECK_HEADERS(termios.h)
AC_CHECK_HEADERS(getopt.h)

dnl Checks compiler characteristics

AC_C_CONST
AC_C_INLINE
AC_C_BIGENDIAN
AC_CHECK_SIZEOF(char)
AC_CHECK_SIZEOF(short)
AC_CHECK_SIZEOF(int)
AC_CHECK_SIZEOF(long)

dnl check defines and functions

AC_TYPE_SIZE_T
AC_CHECK_FUNCS(cfmakeraw)
AC_CHECK_FUNCS(strlcpy)
AC_CHECK_FUNCS(getopt)
AC_CHECK_FUNCS(getopt_long)
ACX_OPTRESET

dnl export configuration to Makefiles

AC_SUBST(CFLAGS)

dnl Makefiles to generate

AC_OUTPUT([
	Makefile
	src/Makefile
	test/Makefile
	doc/Makefile
	doc/hc12mem.1
	target/Makefile
	target/bdm/Makefile
	target/lrae/Makefile
	])

dnl feature synopsis

dnl echo 'lbxd features:'
dnl ACX_FEATURE([enable],[debug])
