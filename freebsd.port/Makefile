# New ports collection makefile for:	hc12mem
# Date created:				18 November 2005
# Whom:					mk
#
# $FreeBSD: ports/devel/hc12mem/Makefile,v 1.3 2005/12/16 19:11:36 ehaupt Exp $
#

PORTNAME=	hc12mem
PORTVERSION=	1.3.2
CATEGORIES=	devel

MASTER_SITES=	${MASTER_SITE_SOURCEFORGE:S/$/:sf/} \
		http://cml.mfk.net.pl/hc12mem/
MASTER_SITE_SUBDIR=	hc12mem/:sf
DISTFILES=	${DISTNAME}${EXTRACT_SUFX}:sf,DEFAULT

MAINTAINER=	mk@capri.pl
COMMENT=	HCS12 MCU tool: erase,read,write,protect,unprotect FLASH/EEPROM

GNU_CONFIGURE=	yes
USE_GMAKE=	yes
USE_GETOPT_LONG=yes
MANCOMPRESSED=	no
USE_GCC=	3.4+

MAN1=		hc12mem.1

.include <bsd.port.mk>
