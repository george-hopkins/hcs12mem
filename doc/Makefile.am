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

man_MANS = hc12mem.1
EXTRA_DIST = hc12mem.1.in

MAINTAINERCLEANFILES = Makefile.in
CLEANFILES = *~

previewman:
	nroff -man hc12mem.1 | less
