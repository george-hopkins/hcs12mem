# hcs12mem - HC12/S12 memory reader & writer
# Makefile.am: automake Makefile template
#
# Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>
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
SUBDIRS = src test target doc

EXTRA_DIST = \
	autogen \
	hcs12mem.dsw

#AUX_DIST = \
#	install-sh \
#	missing \
#	mkinstalldirs \
#	depcomp

MAINTAINERCLEANFILES = \
	Makefile.in \
	aclocal.m4 \
	configure \
	config-h.in \
	stamp-h.in \
	stamp-h1

CLEANFILES = *~

dist-win32:
	DOSTMP=dos.tmp; \
	mkdir $$DOSTMP; \
	for f in AUTHORS COPYING NEWS README; do \
		unix2dos < @srcdir@/$$f > $$DOSTMP/$$f; \
	done; \
	rm -f $(PACKAGE)-$(VERSION).zip; \
	zip -j $(PACKAGE)-$(VERSION).zip \
		$$DOSTMP/AUTHORS \
		$$DOSTMP/COPYING \
		$$DOSTMP/NEWS \
		$$DOSTMP/README \
		@srcdir@/out.win32/release/$(PACKAGE).exe \
		@srcdir@/target/*.dat \
		@srcdir@/target/bdm/bdm.s19 \
		@srcdir@/target/lrae/lrae.s19 \
		@srcdir@/doc/$(PACKAGE).html; \
	rm -rf $$DOSTMP

#	cp @srcdir@/COPYING $$DOSTMP/COPYING
#	unix2dos $$DOSTMP/COPYING
#	cp @srcdir@/NEWS $$DOSTMP/NEWS
#	unix2dos $$DOSTMP/NEWS
#	cp @srcdir@/README $$DOSTMP/README
#	unix2dos $$DOSTMP/README

dist-deb:
	DEBTMP=deb.tmp; \
	rm -rf $$DEBTMP; \
	mkdir -p \
		$$DEBTMP/DEBIAN \
		$$DEBTMP/usr/local/bin \
		$$DEBTMP/usr/local/share/$(PACKAGE) \
		$$DEBTMP/usr/local/share/man/man1; \
	cp @srcdir@/packages/DEBIAN/control $$DEBTMP/DEBIAN; \
	cp src/$(PACKAGE) $$DEBTMP/usr/local/bin; \
	strip $$DEBTMP/usr/local/bin/$(PACKAGE); \
	cp @srcdir@/target/*.dat $$DEBTMP/usr/local/share/$(PACKAGE); \
	cp @srcdir@/target/bdm/bdm.s19 $$DEBTMP/usr/local/share/$(PACKAGE); \
	cp @srcdir@/target/lrae/lrae.s19 $$DEBTMP/usr/local/share/$(PACKAGE); \
	cp doc/$(PACKAGE).1 $$DEBTMP/usr/local/share/man/man1; \
	dpkg -b $$DEBTMP $(PACKAGE)-$(VERSION).deb; \
	rm -rf $$DEBTMP

