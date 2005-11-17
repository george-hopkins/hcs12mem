/*
    hc12mem - HC12 memory reader & writer
    srecrand.c: random data S-record generator
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PAGE_SIZE 0x4000
#define PAGE_BASE 0x8000

int main(void)
{
	FILE *f;
	unsigned int i, j;
	unsigned int size;
	unsigned int rec_size;
	unsigned int addr_size;
	unsigned int ppage;
	unsigned int addr;
	unsigned char sum;
	unsigned char b;
	int keep_lrae;
	unsigned int lrae_size;

	srand(time(NULL));

	f = fopen("out.s19", "wt");
	if (f == NULL)
	{
		fprintf(stderr, "creating file failed\n");
		exit(EXIT_FAILURE);
	}

	size = 64 * 1024;
	rec_size = 16;
	ppage = 0x3c;
	addr_size = 3;
	keep_lrae = 1;
	lrae_size = 0x0400;

	for (i = 0; i < size; i += rec_size)
	{
		addr = (ppage + i / PAGE_SIZE) * 0x00010000 + PAGE_BASE + (i % PAGE_SIZE);
		if ((ppage + i / PAGE_SIZE) == 0x3e && (i % PAGE_SIZE) < lrae_size)
			continue;
		if ((ppage + i / PAGE_SIZE) == 0x3f && (i % PAGE_SIZE) == 0)
			continue;
		if ((ppage + i / PAGE_SIZE) == 0x3f && (i % PAGE_SIZE) >= 0x3f00)/*(0xff00 % PAGE_SIZE))*/
			continue;

		fprintf(f, "S2%02X%06X",
			(unsigned int)(rec_size + addr_size + 1),
			(unsigned int)addr);

		sum = (unsigned char)(rec_size + addr_size + 1);
		for (j = 0; j < addr_size; ++j)
		{
			sum += (unsigned char)(addr & 0x000000ff);
			addr >>= 8;
		}

		for (j = 0; j < rec_size; ++j)
		{
			b = (unsigned char)rand();
			fprintf(f, "%02X", (unsigned int)b);
			sum += b;
		}

		fprintf(f, "%02X\n", (unsigned int)(0xff - sum));
	}

	fclose(f);

	return EXIT_SUCCESS;
}
