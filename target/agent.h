/*
    hcs12mem - HCS12/S12 memory reader & writer
    agent.h: target RAM agent interface
    $Id$

    Copyright (C) 2005,2006,2007 Michal Konieczny <mk@cml.mfk.net.pl>

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

#ifndef __AGENT_H
#define __AGENT_H

#define HCS12_AGENT_CMD      0x0000
#define HCS12_AGENT_STATUS   0x0001
#define HCS12_AGENT_PARAM    0x0002

#define HCS12_AGENT_CMD_INIT                0x00
#define HCS12_AGENT_CMD_EEPROM_MASS_ERASE   0x01
#define HCS12_AGENT_CMD_EEPROM_ERASE_VERIFY 0x02
#define HCS12_AGENT_CMD_EEPROM_ERASE_AREA   0x03
#define HCS12_AGENT_CMD_EEPROM_READ         0x04
#define HCS12_AGENT_CMD_EEPROM_WRITE        0x05
#define HCS12_AGENT_CMD_EEPROM_PROTECT      0x06
#define HCS12_AGENT_CMD_FLASH_MASS_ERASE    0x07
#define HCS12_AGENT_CMD_FLASH_ERASE_VERIFY  0x08
#define HCS12_AGENT_CMD_FLASH_ERASE_SECTOR  0x09
#define HCS12_AGENT_CMD_FLASH_READ          0x0a
#define HCS12_AGENT_CMD_FLASH_WRITE         0x0b
#define HCS12_AGENT_CMD_FLASH_PROTECT       0x0c

#define HCS12_AGENT_ERROR_NONE        0x00
#define HCS12_AGENT_ERROR_XTAL        0x01
#define HCS12_AGENT_ERROR_CMD         0x02
#define HCS12_AGENT_ERROR_VERIFY      0x03
#define HCS12_AGENT_ERROR_PGM         0x04
#define HCS12_AGENT_ERROR_SUM         0x55

#endif
