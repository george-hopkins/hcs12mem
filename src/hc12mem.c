/*
    hc12mem - HC12 memory reader & writer
    hc12mem.c: main module - startup and auxiliary routines
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

#include "hc12mem.h"
#include "hc12mcu.h"
#include "hc12lrae.h"
#include "hc12sm.h"
#include "hc12bdm.h"

#if HAVE_GETOPT_H
# include <getopt.h>
# undef HAVE_GETOPT_OWN
#else
# include "getopt_own.h"
# define HAVE_GETOPT_OWN
#endif


/* program info and usage strings */

static const char *PRG_INFO =
"HC12 memory loader V%s (C) 2005 Michal Konieczny <mk@cml.mfk.net.pl>\n\n";
static const char *PRG_USAGE =
	"Usage: hc12mem [options]\n"
	"  -h, --help\n"
	"      show usage info\n"
	"  -q, --quiet\n"
	"      be quiet (no progress/status messages)\n"
	"  -f, --force\n"
	"      force operation, if questionable\n"
	"  -i <interface>, --interface <interface>\n"
	"      use given interface for target connection, supported ones are:\n"
	"      tbdml     - Daniel Malik's Turbo BDM Light\n"
	"      bdm12pod  - original Kevin Ross's BDM12 POD\n"
	"      podex     - PODEX with patched firmware\n"
	"      podex-bug - original Marek Peca's PODEX (with bugs)\n"
	"      podex-25  - special PODEX version dedicated for 25MHz target\n"
	"      lrae      - Freescale's serial LRAE bootloader (AN2546)\n"
	"      sm        - Freescale's serial monitor (AN2548)\n"
	"  -p <port>, --port <port>\n"
	"      use given port for target connection\n"
	"  -b <baud>, --baud <baud>\n"
	"      use given baud rate for serial port\n"
	"  -t <target>, --target <target>\n"
	"      target device definition\n"
	"  -o <freq>, --osc <freq>\n"
	"      target device oscillator frequency\n"
	"  -j <addr>, --start-address <addr>\n"
	"      start address for RAM image, if not supplied within S-record\n"
	"  -a <type>, --flash-address <type>\n"
	"      S-record type when reading/writing file:\n"
	"      non-banked    - FLASH linear area within 64kB, size depends on target\n"
	"      banked-linear - Freescale banked linear format\n"
	"      banked-ppage  - banked format with PPAGE value as MSB\n"
	"      (when not specified, default is non-banked)\n"
	"  -e, --include-erased\n"
	"      include erased areas of memory in written S-record file\n"
	"      (default is to skip 0xff blocks)\n"
	"  -s <size>, --srecord-size <size>\n"
	"      size of single S-record written to file, default: 16\n"
	"  -v, --verify\n"
	"      verify result of all erase/write operations\n"
	"Following options can be specified multiple times, any of them,\n"
	"processing is according to occurence order:\n"
	"  -R, --reset\n"
	"      reset target into normal mode\n"
	"  -U, --unsecure\n"
	"      unsecure secured microcontroller\n"
	"  -S, --secure\n"
	"      secure the microcontroller\n"
	"  -X <file>, --ram-run <file>\n"
	"      load S-record file into RAM and execute\n"
	"  -A, --eeprom-erase\n"
	"      erase EEPROM memory\n"
	"  -B <file>, --eeprom-read <file>\n"
	"      read EEPROM memory contents into S-record file\n"
	"  -C <file>, --eeprom-write <file>\n"
	"      write EEPROM memory contents from S-record file\n"
	"  -D <range>, --eeprom-protect <range>\n"
	"      write EEPROM protection byte, range can be:\n"
	"      <all|64B|128B|192B|256B|320B|384B|448B|512B>\n"
	"      all - protects whole EEPROM array\n"
	"      xxxB - protects last xxx bytes of EEPROM array\n"
	"  -E, --flash-erase\n"
	"      erase FLASH memory, leave security byte in secured state\n"
	"  -F, --flash-erase-unsecure\n"
	"      erase FLASH memory, program security byte to unsecured state\n"
	"  -G <file>, --flash-read <file>\n"
	"      read FLASH memory contents into S-record file\n"
	"  -H <file>, --flash-write <file>\n"
	"      write FLASH memory contents from S-record file\n"
	"Special options for LRAE:\n"
	"  -Z, --keep-lrae\n"
	"      keep LRAE boot loader in FLASH memory when erasing FLASH\n"
	"      memory (default is to erase it)\n"
	"Special options for TBDML:\n"
	"  -Y, --tbdml-bulk\n"
	"      enable bulk USB transfers for TBDML (faster, but non-standard\n"
	"      according to USB specification)\n";

/* target connection handlers */

static const hc12mem_target_handler_t *hc12mem_target_handler_table[] =
{
	&hc12mem_target_handler_lrae,
	&hc12mem_target_handler_sm,
	&hc12mem_target_handler_bdm12pod,
	&hc12mem_target_handler_tbdml,
	NULL
};

/* globals */

hc12mem_options_t options;
char hc12mem_data_dir[SYS_MAX_PATH + 1];
hc12mem_target_info_t *hc12mem_target_info_head = NULL;
static int progress_last;


/*
 *  error reporting
 *
 *  in:
 *    fmt - printf()-style formatting string
 *    ... - arguments for formatting string
 *  out:
 *    void
 */

void error(const char *fmt, ...)
{
	va_list list;

	fprintf(stderr, "\nerror: ");
	va_start(list, fmt);
	vfprintf(stderr, fmt, list);
	va_end(list);
}


/*
 *  start progress reporting
 *
 *  in:
 *    title - progress counter name
 *  out:
 *    start time
 */

unsigned long progress_start(const char *title)
{
	progress_last = 0;
	if (options.verbose)
	{
		printf("%s [                                                  ]\b"
		       "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b"
		       "\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b",
		       (const char *)title);
		fflush(stdout);
	}

	return sys_get_ms();
}


/*
 *  stop progress reporting
 *
 *  in:
 *    t - progress start time (from progress_start())
 *    title - progress status title
 *    bytes - number of bytes transferred
 *  out:
 *    void
 */

void progress_stop(unsigned long t, const char *title, uint32_t bytes)
{
	t = (sys_get_ms() - t + 5) / 10;

	if (options.verbose)
	{
		printf("]\n");
		fflush(stdout);
	}

	if (title != NULL && options.verbose)
	{
		printf("%s size <%u B> time <%u.%02u s> rate <%u B/s>\n",
		       (const char *)title,
		       (unsigned int)bytes,
		       (unsigned int)(t / 100),
		       (unsigned int)(t % 100),
		       (unsigned int)(t == 0 ? 0 : bytes * 100 / t));
		fflush(stdout);
	}
}


/*
 *  progress reporting
 *
 *  in:
 *    n - step number
 *    total - total number of steps
 *  out:
 *    void
 */

void progress_report(uint32_t n, uint32_t total)
{
	int x;

	x = (n * 1000 / total + 5) / 10;
	while (x > progress_last)
	{
		if (options.verbose && !options.debug)
		{
			if (progress_last & 0x01)
				printf("\b#");
			else
				printf("-");
			fflush(stdout);
		}
		++ progress_last;
	}
}


/*
 *  parse oscillator frequency
 *
 *  in:
 *    str - oscillator frequency as string
 *  out:
 *    oscillator frequency as number, 0 on error
 */

static unsigned long hc12mem_parse_osc(const char *str)
{
	unsigned long osc;
	char *end;

	osc = strtoul(str, &end, 0);
	if (*end == '\0')
		return osc;
	if (strcasecmp(end, "k") == 0 || strcasecmp(end, "kHz") == 0)
		return osc * 1000;
	if (strcasecmp(end, "M") == 0 || strcasecmp(end, "MHz") == 0)
		return osc * 1000000;

	error("invalid oscillator frequency: %s\n", (const char *)str);
	return 0;
}


/*
 *  free target info data
 *
 *  in:
 *    void
 *  out:
 *    void
 */

static void hc12mem_target_info_free(void)
{
	hc12mem_target_info_t *next;

	while (hc12mem_target_info_head != NULL)
	{
		free(hc12mem_target_info_head->key);
		free(hc12mem_target_info_head->value);
		next = hc12mem_target_info_head->next;
		free(hc12mem_target_info_head);
		hc12mem_target_info_head = next;
	}
}


/*
 *  read target info data
 *
 *  in:
 *    void
 *  out:
 *    status code (0 - ok, other value - error code)
 */

static int hc12mem_target_info_read(void)
{
	char file[SYS_MAX_PATH + 1];
	char buf[256];
	FILE *f;
	int ret;
	char *ptr;
	char *key;
	hc12mem_target_info_t *rec;
	hc12mem_target_info_t **node;

	if (access(options.target, R_OK) == -1 &&
	    strchr(options.target, SYS_PATH_SEPARATOR) == NULL)
	{
		snprintf(file, sizeof(file), "%s%c%s.dat",
			 (const char *)hc12mem_data_dir,
			 (char)SYS_PATH_SEPARATOR,
			 (const char *)options.target);
	}
	else
		strlcpy(file, options.target, sizeof(file));

	f = fopen(file, "rt");
	if (f == NULL)
	{
		ret = errno;
		error("cannot open target description file %s (%s)\n",
		      (const char *)file,
		      (const char *)strerror(ret));
		return ret;
	}

	ret = 0;
	while (fgets(buf, sizeof(buf), f) != NULL)
	{
		for (ptr = buf; *ptr != '\0'; ++ ptr)
		{
			if (*ptr == '\r' || *ptr == '\n')
			{
				*ptr = '\0';
				break;
			}
		}

		ptr = buf;
		while (isspace(*ptr))
			++ ptr;

		if (*ptr == '\0' || *ptr == '#')
			continue;

		key = ptr;
		while (*ptr != '\0' && !isspace(*ptr))
			++ ptr;
		if (*ptr != '\0')
		{
			*ptr++ = '\0';
			while (isspace(*ptr))
				++ ptr;
		}

		rec = malloc(sizeof(*rec));
		if (rec == NULL)
		{
		  err_mem:
			hc12mem_target_info_free();
			error("not enough memory\n");
			ret = ENOMEM;
			break;
		}

		rec->key = strdup(key);
		rec->value = strdup(ptr);
		rec->next = NULL;
		if (rec->key == NULL || rec->value == NULL)
		{
			if (rec->key != NULL)
				free(rec->key);
			if (rec->value != NULL)
				free(rec->value);
			free(rec);
			goto err_mem;
		}

		node = &hc12mem_target_info_head;
		while (*node != NULL)
			node = &((*node)->next);
		*node = rec;
	}

	if (ferror(f))
	{
		if (ret == 0)
			ret = errno;
		error("cannot read target description file %s (%s)\n",
		      (const char *)file,
		      (const char *)strerror(errno));
	}

	if (fclose(f) == -1)
	{
		if (ret == 0)
			ret = errno;
		error("cannot close target description file %s (%s)\n",
		      (const char *)file,
		      (const char *)strerror(errno));
		return ret;
	}

	return 0;
}


/*
 *  get target info record value as string
 *
 *  in:
 *    key - key name
 *    first - which record to return (1 - first, 0 - next)
 *  out:
 *    info record value
 */

const char *hc12mem_target_info(const char *key, int first)
{
	static hc12mem_target_info_t *rec;
	hc12mem_target_info_t *ptr;

	if (first)
		rec = hc12mem_target_info_head;
	if (key == NULL)
		return NULL;
	while (rec != NULL)
	{
		ptr = rec;
		rec = rec->next;
		if (strcmp(key, ptr->key) == 0)
			return ptr->value;
	}

	return NULL;
}


/*
 *  get target info record value as number
 *
 *  in:
 *    key - key name
 *    value - on return, key value is returned here
 *    def - default value, when target info doesn't specify one
 *  out:
 *    0 - ok, other value - errno status code
 */

int hc12mem_target_param(const char *key, uint32_t *value, uint32_t def)
{
	const char *ptr;
	unsigned long v;
	char *end;

	*value = def;

	ptr = hc12mem_target_info(key, TRUE);
	if (ptr == NULL)
		return 0;

	v = strtoul(ptr, &end, 0);
	if (*end != '\0')
	{
		error("invalid value for target description parameter %s\n",
		      (const char *)key);
		return EINVAL;
	}

	*value = (uint32_t)v;
	return 0;
}


/*
 *  program entry point
 *
 *  in:
 *    argc, argv - command line arguments
 *  out:
 *    EXIT_SUCCESS / EXIT_FAILURE
 */

int main(int argc, char *argv[])
{
	const hc12mem_target_handler_t *h;
	int c;
	char *end;
	int i;
	int ret;

	/* valid options */

	static const char *opt_string = "hqdfi:p:b:c:t:o:j:a:es:vX:USAB:C:D:EFG:H:RZY";
#if HAVE_GETOPT_LONG
	static const struct option opt_long[] =
#else
	static const struct
	{
		char *name;
		int has_arg;
		int *flag;
		int val;
	}
	opt_long[] =
#endif
	{
		{ "help",           0, NULL, 'h' },
		{ "quiet",          0, NULL, 'q' },
		{ "debug",          0, NULL, 'd' },
		{ "force",          0, NULL, 'f' },
		{ "interface",      1, NULL, 'i' },
		{ "port",           1, NULL, 'p' },
		{ "baud",           1, NULL, 'b' },
		{ "chip",           1, NULL, 'c' },
		{ "target",         1, NULL, 't' },
		{ "osc",            1, NULL, 'o' },
		{ "start-address",  1, NULL, 'j' },
		{ "flash-address",  1, NULL, 'a' },
		{ "include-erased", 0, NULL, 'e' },
		{ "srec-size",      1, NULL, 's' },
		{ "verify",         0, NULL, 'V' },
		{ "reset",          0, NULL, 'R' },
		{ "ram-run",        1, NULL, 'X' },
		{ "unsecure",       0, NULL, 'U' },
		{ "secure",         0, NULL, 'S' },
		{ "eeprom-erase",   0, NULL, 'A' },
		{ "eeprom-read",    1, NULL, 'B' },
		{ "eeprom-write",   1, NULL, 'C' },
		{ "eeprom-protect", 1, NULL, 'D' },
		{ "flash-erase",    0, NULL, 'E' },
		{ "flash-erase-unsecure", 0, NULL, 'F' },
		{ "flash-read",     1, NULL, 'G' },
		{ "flash-write",    1, NULL, 'H' },
		{ "keep-lrae",      0, NULL, 'Z' },
		{ "tbdml-bulk",     0, NULL, 'Y' },
		{ NULL, 0, NULL, 0 }
	};

	if (argc == 1)
	{
		fprintf(stderr, PRG_INFO, (const char *)VERSION);
		fprintf(stderr, PRG_USAGE);
		exit(EXIT_FAILURE);
	}

	/* replace long options with short ones, when there's no support
	   for long options on this platform */

#	if !HAVE_GETOPT_LONG
	{
		int i, j;

		for (i = 1; i < argc; ++ i)
		{
			if (argv[i][0] == '-' && argv[i][1] == '-')
			{
				for (j = 0; opt_long[j].name != NULL; ++ j)
				{
					if (strcmp(argv[i] + 2, opt_long[j].name) == 0)
					{
						sprintf(argv[i], "-%c", opt_long[j].val);
						break;
					}
				}
			}
		}
	}
#	endif

	/* get data directory */

#	if SYS_TYPE_WIN32
	{
		strlcpy(hc12mem_data_dir, _pgmptr, sizeof(hc12mem_data_dir));
		*strrchr(hc12mem_data_dir, SYS_PATH_SEPARATOR) = '\0';
	}
#	else
		strlcpy(hc12mem_data_dir, HC12MEM_DATA_DIR, sizeof(hc12mem_data_dir));
#	endif

	/* options initial values */

	options.verbose = TRUE;
	options.debug = FALSE;
	options.force = FALSE;
	options.verify = FALSE;
	options.iface = NULL;
	options.port = NULL;
	options.baud = 0;
	options.chip = NULL;
	options.target = NULL;
	options.osc = 0;
	options.start = 0;
	options.start_valid = FALSE;
	options.flash_addr = HC12MEM_FLASH_ADDR_NON_BANKED;
	options.include_erased = FALSE;
	options.srec_size = HC12MEM_DEFAULT_SREC_SIZE;
	options.podex_25 = FALSE;
	options.podex_mem_bug = FALSE;
	options.keep_lrae = FALSE;
	options.tbdml_bulk = FALSE;

	/* parse options */

#	ifdef HAVE_GETOPT_OWN
	opterr = 0;
#	endif

	for (;;)
	{
#		if HAVE_GETOPT_LONG
		c = getopt_long(argc, argv, opt_string, opt_long, &i);
#		else
		c = getopt(argc, argv, opt_string);
#		endif

		if (c == -1)
			break;

#		ifdef HAVE_GETOPT_OWN
		{
			char *ptr = strchr(opt_string, c);
			if (ptr != NULL && ptr[1] == ':' && optarg == NULL)
			{
				error("missing argument for option %s (use -h option for help on usage)\n",
					(const char *)argv[optind - 1]);
				exit(EXIT_FAILURE);
			}
		}
#		endif

		switch (c)
		{
			case 'h':
				fprintf(stderr, PRG_INFO, (const char *)VERSION);
				fprintf(stderr, PRG_USAGE);
				exit(EXIT_SUCCESS);

			case 'q':
				options.verbose = FALSE;
				break;

			case 'd':
				options.debug = TRUE;
				break;

			case 'f':
				options.force = TRUE;
				break;

			case 'i':
				options.iface = optarg;
				break;

			case 'p':
				options.port = optarg;
				break;

			case 'b':
				options.baud = (unsigned long)
					strtoul(optarg, &end, 10);
				if (*end != '\0')
				{
					error("invalid baud rate: %s\n",
					      (const char *)optarg);
					exit(EXIT_FAILURE);
				}
				break;

			case 'c':
				options.chip = optarg;
				break;

			case 't':
				if (options.target != NULL)
				{
					error("target already specified\n");
					exit(EXIT_FAILURE);
				}
				options.target = optarg;
				break;

			case 'o':
				options.osc = hc12mem_parse_osc(optarg);
				if (options.osc == 0)
					exit(EXIT_FAILURE);
				break;

			case 'j':
				options.start = strtoul(optarg, &end, 0);
				if (*end != '\0')
				{
					error("invalid start address: %s\n",
					      (const char *)optarg);
					exit(EXIT_FAILURE);
				}
				options.start_valid = TRUE;
				break;

			case 'a':
				if (strcmp(optarg, "non-banked") == 0)
					options.flash_addr = HC12MEM_FLASH_ADDR_NON_BANKED;
				else if (strcmp(optarg, "banked-linear") == 0)
					options.flash_addr = HC12MEM_FLASH_ADDR_BANKED_LINEAR;
				else if (strcmp(optarg, "banked-ppage") == 0)
					options.flash_addr = HC12MEM_FLASH_ADDR_BANKED_PPAGE;
				else
				{
					error("invalid address format: %s\n",
					      (const char *)optarg);
					exit(EXIT_FAILURE);
				}
				break;

			case 'e':
				options.include_erased = TRUE;
				break;

			case 's':
				options.srec_size = strtoul(optarg, &end, 0);
				if (*end != '\0')
				{
					error("invalid S-record size: %s\n",
					      (const char *)optarg);
					exit(EXIT_FAILURE);
				}
				break;

			case 'v':
				options.verify = TRUE;
				break;

			case 'R':
			case 'X':
			case 'U':
			case 'S':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
				break;

			case 'Z':
				options.keep_lrae = TRUE;
				break;

			case 'Y':
				options.tbdml_bulk = TRUE;
				break;

			default:
#				ifdef HAVE_GETOPT_OWN
				error("%c unknown option: %s (use -h option for help on usage)\n",
					c, (const char *)argv[optind - 1]);
#				endif
				exit(EXIT_FAILURE);
		}
	}

	if (optind < argc)
	{
		error("extraneous command line item: %s\n",
		      (const char *)argv[optind]);
		exit(EXIT_FAILURE);
	}

	/* validate some options */

	if (options.iface == NULL)
	{
		error("missing target interface selection (-i option)\n");
		exit(EXIT_FAILURE);
	}

	if (options.target == NULL)
	{
		error("target not specified (-t option)\n");
		exit(EXIT_FAILURE);
	}

	if (strcmp(options.iface, "podex") == 0)
		options.iface = "bdm12pod";
	else if (strcmp(options.iface, "podex-bug") == 0)
	{
		options.iface = "bdm12pod";
		options.podex_mem_bug = TRUE;
	}
	else if (strcmp(options.iface, "podex-25") == 0)
	{
		options.iface = "bdm12pod";
		options.podex_25 = TRUE;
		if (options.osc != 0 && options.osc != 25000000)
		{
			error("this interface works with target oscillator = 25MHz only\n");
			exit(EXIT_FAILURE);
		}
		options.osc = 25000000;
	}

	h = NULL;
	for (i = 0; hc12mem_target_handler_table[i] != NULL; ++ i)
	{
		if (strcmp(options.iface, hc12mem_target_handler_table[i]->name) == 0)
		{
			h = hc12mem_target_handler_table[i];
			break;
		}
	}
	if (h == NULL)
	{
		error("invalid target interface: %s\n",
		      (const char *)options.iface);
		return EXIT_FAILURE;
	}

	if (options.verbose)
		printf(PRG_INFO, (const char *)VERSION);

	/* read target info data */

	ret = hc12mem_target_info_read();
	if (ret != 0)
		exit(EXIT_FAILURE);

	ret = hc12mcu_target_parse();
	if (ret != 0)
	{
		hc12mem_target_info_free();
		exit(EXIT_FAILURE);
	}

	/* open target connection */

	if ((*h->open)() != 0)
	{
		hc12mem_target_info_free();
		exit(EXIT_FAILURE);
	}

	/* process requested target operations */

	optind = 1;
#	if HAVE_OPTRESET
	optreset = 1;
#	endif

	for (;;)
	{
#		if HAVE_GETOPT_LONG
		c = getopt_long(argc, argv, opt_string, opt_long, &i);
#		else
		c = getopt(argc, argv, opt_string);
#		endif

		if (c == -1)
			break;

		switch (c)
		{
			case 'R':
				ret = (*h->reset)();
				break;
			case 'X':
				ret = (*h->ram_run)(optarg);
				break;
			case 'U':
				ret = (*h->unsecure)();
				break;
			case 'S':
				ret = (*h->secure)();
				break;
			case 'A':
				ret = (*h->eeprom_erase)();
				break;
			case 'B':
				ret = (*h->eeprom_read)(optarg);
				break;
			case 'C':
				ret = (*h->eeprom_write)(optarg);
				break;
			case 'D':
				ret = (*h->eeprom_protect)(optarg);
				break;
			case 'E':
				ret = (*h->flash_erase)(FALSE);
				break;
			case 'F':
				ret = (*h->flash_erase)(TRUE);
				break;
			case 'G':
				ret = (*h->flash_read)(optarg);
				break;
			case 'H':
				ret = (*h->flash_write)(optarg);
				break;
			default:
				ret = 0;
				break;
		}

		if (ret != 0)
		{
			(*h->close)();
			hc12mem_target_info_free();
			exit(EXIT_FAILURE);
		}
	}

	/* close target connection */

	if ((*h->close)() == -1)
	{
		hc12mem_target_info_free();
		exit(EXIT_FAILURE);
	}

	hc12mem_target_info_free();

	return EXIT_SUCCESS;
}
