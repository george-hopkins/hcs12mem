/*
    hc12mem - HC12 memory reader & writer
    bdm12pod.h: Kevin Ross' BDM12POD driver
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

#ifndef __BDM12POD_H
#define __BDM12POD_H

#include "hc12bdm.h"

/* timeouts and delays */

#define BDM12POD_DEFAULT_BAUD_RATE 115200 /* bps */
#define BDM12POD_CTS_TIMEOUT 2000 /* ms */
#define BDM12POD_RX_TIMEOUT  2000 /* ms */
#define BDM12POD_TX_TIMEOUT  2000 /* ms */
#define BDM12POD_DEFAULT_TRACE_DELAY 0 /* ms */
#define BDM12POD_DEFAULT_RESET_DELAY 1 /* ms */

/* BDM12POD commands */

#define BDM12POD_CMD_SYNC         0x00
#define BDM12POD_CMD_RESET_CPU    0x01
#define BDM12POD_CMD_RESET_LOW    0x02
#define BDM12POD_CMD_RESET_HIGH   0x03
#define BDM12POD_CMD_EXT          0x04
#define BDM12POD_CMD_EEPROM_WRITE 0x05
#define BDM12POD_CMD_SET_REG_BASE 0x06
#define BDM12POD_CMD_EEPROM_ERASE 0x07

#define BDM12POD_CMD_EXT_GET_VERSION 0x00
#define BDM12POD_CMD_EXT_REG_DUMP    0x01
#define BDM12POD_CMD_EXT_TRACE_TO    0x02
#define BDM12POD_CMD_EXT_MEM_DUMP    0x03
#define BDM12POD_CMD_EXT_SET_PARAM   0x04
#define BDM12POD_CMD_EXT_IOCTL       0x05
#define BDM12POD_CMD_EXT_MEM_PUT     0x06
#define BDM12POD_CMD_EXT_SPEED       0x07

/* target clock selection */

#define BDM12POD_PARAM_ECLK_1M  0x00
#define BDM12POD_PARAM_ECLK_2M  0x01
#define BDM12POD_PARAM_ECLK_4M  0x02
#define BDM12POD_PARAM_ECLK_8M  0x03
#define BDM12POD_PARAM_ECLK_EXT 0x04

/* BDM12POD handler */

extern hc12bdm_handler_t bdm12pod_bdm_handler;

#endif /* __BDM12POD_H */
