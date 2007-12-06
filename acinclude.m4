dnl hcs12mem - HC12/S12 memory reader & writer
dnl acinclude.m4: additional autoconf tests
dnl
dnl Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>
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

dnl @synopsis ACX_OPTRESET
dnl check whether optreset is defined

AC_DEFUN([ACX_OPTRESET],
[AC_CACHE_CHECK([for optreset in getopt.h],
  acx_cv_optreset,
[AC_TRY_COMPILE([#include <getopt.h>],
[optreset = 1;],
  acx_cv_optreset=yes, acx_cv_optreset=no)])
if test $acx_cv_optreset = yes; then
  AC_DEFINE(HAVE_OPTRESET,1,[Define if you have optreset in <getopt.h>])
fi
])

dnl @synopsis ACX_HELP_STRING(OPTION,DESCRIPTION)
AC_DEFUN([ACX_HELP_STRING],
	 [  $1 builtin([substr],[                       ],len($1))[$2]])

dnl @synopsis ACX_FEATURE(ENABLE_OR_WITH,NAME[,VALUE])
AC_DEFUN([ACX_FEATURE],
	 [echo "builtin([substr],[],len(--$1-$2))--$1-$2: ifelse($3,,[$]translit($1-$2,-,_),$3)"])
