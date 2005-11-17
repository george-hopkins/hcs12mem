/*
    hc12mem - HC12 memory reader & writer
    hc12mem.h: main module - startup and auxiliary routines
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

#ifndef __HC12MEM_H
#define __HC12MEM_H

#include "sys.h"

#ifndef VERSION
#define VERSION "1.1"
#endif

/* default S-record size */

#define HC12MEM_DEFAULT_SREC_SIZE 16

/* supported FLASH addressing variants */

#define HC12MEM_FLASH_ADDR_NON_BANKED    0
#define HC12MEM_FLASH_ADDR_BANKED_LINEAR 1
#define HC12MEM_FLASH_ADDR_BANKED_PPAGE  2

/* program options */

typedef struct
{
	int verbose;
	int debug;
	int force;
	int verify;
	const char *iface;
	const char *port;
	unsigned long baud;
	const char *chip;
	unsigned long start;
	int start_valid;
	const char *target;
	unsigned long osc;
	int flash_addr;
	int include_erased;
	size_t srec_size;
	int podex_25;
	int podex_mem_bug;
	int keep_lrae;
}
hc12mem_options_t;

/* target info list record */

typedef struct hc12mem_target_info_t
{
	char *key;
	char *value;
	struct hc12mem_target_info_t *next;
}
hc12mem_target_info_t;

/* target connection handler */

typedef struct
{
	const char *name;
	int (*open)(void);
	int (*close)(void);
	int (*ram_run)(const char *file);
	int (*unsecure)(void);
	int (*secure)(void);
	int (*eeprom_read)(const char *file);
	int (*eeprom_erase)(void);
	int (*eeprom_write)(const char *file);
	int (*eeprom_protect)(const char *opt);
	int (*flash_read)(const char *file);
	int (*flash_erase)(int unsecure);
	int (*flash_write)(const char *file);
	int (*flash_protect)(const char *opt);
	int (*reset)(void);
}
hc12mem_target_handler_t;

/* globals */

extern hc12mem_options_t options;
extern char hc12mem_data_dir[];

void error(const char *fmt, ...);
unsigned long progress_start(const char *title);
void progress_stop(unsigned long t, const char *title, uint32_t bytes);
void progress_report(uint32_t n, uint32_t total);
const char *hc12mem_target_info(const char *key, int first);
int hc12mem_target_param(const char *key, uint32_t *value, uint32_t def);

#endif /* __HC12MEM_H */
