/* ----------------------------------------------------------------------- *
 *
 *   Copyright 2000 Transmeta Corporation - All Rights Reserved
 *   Copyright 2004-2008 H. Peter Anvin - All Rights Reserved
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 *   Boston MA 02110-1301, USA; either version 2 of the License, or
 *   (at your option) any later version; incorporated herein by reference.
 *
 * ----------------------------------------------------------------------- */

/*
 * rdmsr.c
 *
 * Utility to read an MSR.
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>

#include "version.h"

static const struct option long_options[] = {
	{"help",		0, 0, 'h'},
	{"version",		0, 0, 'V'},
	{"bitfield",		1, 0, 'f'},
	{0, 0, 0, 0}
};
static const char short_options[] = "hV:f:";

const char *program;

void usage(void)
{
	fprintf(stderr,
		"Usage: %s [options] regno\n"
		"  --help         -h  Print this help\n"
		"  --version      -V  Print current version\n"
		"  --bitfield h:l -f  Output bits [h:l] only\n", program);
}

void rdmsr_on_cpu(uint32_t reg, int cpu);

/* filter out ".", "..", "microcode" in /dev/cpu */
int dir_filter(const struct dirent *dirp) {
	if (isdigit(dirp->d_name[0]))
		return 1;
	else
		return 0;
}

void rdmsr_on_all_cpus(uint32_t reg)
{
	struct dirent **namelist;
	int dir_entries;

	dir_entries = scandir("/dev/cpu", &namelist, dir_filter, 0);
	while (dir_entries--) {
		rdmsr_on_cpu(reg, atoi(namelist[dir_entries]->d_name));
		free(namelist[dir_entries]);
	}
	free(namelist);
}

unsigned int highbit = 63, lowbit = 0;

int main(int argc, char *argv[])
{
	uint32_t reg;
	int c;
	int cpu = 0;
	unsigned long arg;
	char *endarg;

	program = argv[0];

	while ((c =
		getopt_long(argc, argv, short_options, long_options,
			    NULL)) != -1) {
		switch (c) {
		case 'h':
			usage();
			exit(0);
		case 'V':
			fprintf(stderr, "%s: version %s\n", program,
				VERSION_STRING);
			exit(0);
		case 'f':
			if (sscanf(optarg, "%u:%u", &highbit, &lowbit) != 2 ||
			    highbit > 63 || lowbit > highbit) {
				usage();
				exit(127);
			}
			break;
		default:
			usage();
			exit(127);
		}
	}

	if (optind != argc - 1) {
		/* Should have exactly one argument */
		usage();
		exit(127);
	}

	reg = strtoul(argv[optind], NULL, 0);

	rdmsr_on_all_cpus(reg);
	exit(0);
}

void rdmsr_on_cpu(uint32_t reg, int cpu)
{
	uint64_t data;
	int fd;
	char *pat;
	int width;
	char msr_file_name[64];
	unsigned int bits;

	sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
	fd = open(msr_file_name, O_RDONLY);
	if (fd < 0) {
		if (errno == ENXIO) {
			return;
		} else if (errno == EIO) {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
				cpu);
			exit(3);
		} else {
			perror("rdmsr: open");
			exit(127);
		}
	}

	if (pread(fd, &data, sizeof data, reg) != sizeof data) {
		if (errno == EIO) {
			fprintf(stderr, "rdmsr: CPU %d cannot read "
				"MSR 0x%08x\n",
				cpu, reg);
			exit(4);
		} else {
			perror("rdmsr: pread");
			exit(127);
		}
	}

	close(fd);

	bits = highbit - lowbit + 1;
	if (bits < 64) {
		/* Show only part of register */
		data >>= lowbit;
		data &= (1ULL << bits) - 1;
	}


	printf("CPU %d: ", cpu);
	printf("%1llx\n", data);
	return;
}
