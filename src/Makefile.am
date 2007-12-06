# hcs12mem - HC12/S12 memory reader & writer
# Makefile.am: automake Makefile template
#
# Copyright (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

srcdir = @srcdir@
VPATH = @srcdir@

AM_CFLAGS = -DHCS12MEM_DATA_DIR=\"${pkgdatadir}\"

bin_PROGRAMS = hcs12mem

hcs12mem_SOURCES = \
	sys.c \
	sys.h \
	serial.c \
	serial.h \
	sys_usb.c \
	sys_usb.h \
	srec.c \
	srec.h \
	tbdml.c \
	tbdml.h \
	tbdml_comm.h \
	bdm12pod.c \
	bdm12pod.h \
	hcs12bdm.c \
	hcs12bdm.h \
	hcs12lrae.c \
	hcs12lrae.h \
	hcs12sm.c \
	hcs12sm.h \
	hcs12mcu.c \
	hcs12mcu.h \
	hcs12mem.c \
	hcs12mem.h

EXTRA_DIST = \
	getopt_own.c \
	getopt_own.h \
	libusb/libusbwin32.h \
	libusb/libusbunix.h \
	src.dsp

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = *~
