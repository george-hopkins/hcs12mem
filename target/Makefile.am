# hc12mem - HC12 memory reader & writer
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
SUBDIRS = bdm lrae

pkgdata_DATA = \
	mc9s12a32.dat \
	mc9s12a64.dat \
	mc9s12a128.dat \
	mc9s12a256.dat \
	mc9s12a512.dat \
	mc9s12d32.dat \
	mc9s12d64.dat \
	mc9s12d128.dat \
	mc9s12d256.dat \
	mc9s12d512.dat \
	mc9s12c32.dat \
	mc9s12c64.dat \
	mc9s12c96.dat \
	mc9s12c128.dat \
	mc9s12gc16.dat \
	mc9s12gc32.dat \
	mc9s12gc64.dat \
	mc9s12gc96.dat \
	mc9s12gc128.dat \
	mc9s12e32.dat \
	mc9s12e64.dat \
	mc9s12e128.dat \
	mc9s12e256.dat \
	mc9s12h128.dat \
	mc9s12h256.dat \
	mc9s12ne64.dat \
	mc9s12uf32.dat \
	mc9s12xd256.dat \
	mc9s12xd512.dat

EXTRA_DIST = $(pkgdata_DATA) \
	agent.h \
	io.h \
	io_mc9s12.h \
	io_template.h

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = *~
