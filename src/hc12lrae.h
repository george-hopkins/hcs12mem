/*
    hc12mem - HC12 memory reader & writer
    hc12lrae.h: load ram and execute boot loader routines
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

#ifndef __HC12LRAE_H
#define __HC12LRAE_H

#define HC12LRAE_SYNC_RETRIES      16 /* 8 settings used by LRAE x 2 bytes */
#define HC12LRAE_SYNC_TIMEOUT     100 /* ms */
#define HC12LRAE_TX_TIMEOUT      1000 /* ms */
#define HC12LRAE_CHECKSUM_TIMEOUT 500 /* ms */
#define HC12LRAE_BAUD_ERROR_LIMIT 390 /* 3.9% */

#define HC12LRAE_SYNC_MSG     0x55
#define HC12LRAE_SYNC_ACK     0xaa
#define HC12LRAE_CHECKSUM_ACK 0x80

#define HC12LRAE_RAM_TOP     0x3fcf
#define HC12LRAE_RAM_END     0x3fff
#define HC12LRAE_FLASH_START 0x4000

#define HC12LRAE_AGENT_TIMEOUT 1000

extern hc12mem_target_handler_t hc12mem_target_handler_lrae;

#endif /* __HC12LRAE_H */
