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
 * wrmsr.c
 *
 * Utility to write to an MSR.
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>

const char *program;

void wrmsr_on_cpu(uint32_t reg, int cpu, char *regvals[]);

/* filter out ".", "..", "microcode" in /dev/cpu */
int dir_filter(const struct dirent *dirp)
{
	if (isdigit(dirp->d_name[0]))
		return 1;
	else
		return 0;
}

void wrmsr_on_all_cpus(uint32_t reg, char *regvals[])
{
	struct dirent **namelist;
	int dir_entries;

	dir_entries = scandir("/dev/cpu", &namelist, dir_filter, 0);
	while (dir_entries--) {
		wrmsr_on_cpu(reg, atoi(namelist[dir_entries]->d_name), regvals);
		free(namelist[dir_entries]);
	}
	free(namelist);
}

int main(int argc, char *argv[])
{
	uint32_t reg = 0x1FC;
	unsigned long arg;
	char *endarg;

	wrmsr_on_all_cpus(reg, &argv[optind]);

	exit(0);
}

void wrmsr_on_cpu(uint32_t reg, int cpu, char *regvals[])
{
	uint64_t data;
	int fd;
	char msr_file_name[64];

	sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
	fd = open(msr_file_name, O_WRONLY);
	if (fd < 0) {
		if (errno == ENXIO) {		
			return;
		} else if (errno == EIO) {
			fprintf(stderr, "wrmsr: CPU %d doesn't support MSRs\n",
				cpu);
			exit(2);
		} else {
			perror("wrmsr: open");
			exit(127);
		}
	}

	data = 7;

	pwrite(fd, &data, sizeof data, reg);

	close(fd);

	return;
}
