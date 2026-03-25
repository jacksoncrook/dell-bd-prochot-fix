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
 * disable-bd-prochot.c
 *
 * Utility to disable BD PROCHOT through an MSR.
 */

#include <errno.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>


const char *program;

void modify_msr_on_cpu(uint32_t reg, int cpu);

/* filter out ".", "..", "microcode" in /dev/cpu */
int dir_filter(const struct dirent *dirp) {
	if (isdigit(dirp->d_name[0]))
		return 1;
	else
		return 0;
}

void modify_msr_on_all_cpus(uint32_t reg)
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
	uint32_t reg = 0x1FC;

	program = argv[0];

	modify_msr_on_all_cpus(reg);
	exit(0);
}

void modify_msr_on_cpu(uint32_t reg, int cpu)
{
	uint64_t data;
	int fd;
	char *pat;
	char msr_file_name[64];

	sprintf(msr_file_name, "/dev/cpu/%d/msr", cpu);
	fd = open(msr_file_name, O_RDWR);
	if (fd < 0) {
		if (errno == ENXIO) {
			return;
		} else if (errno == EIO) {
			fprintf(stderr, "rdmsr: CPU %d doesn't support MSRs\n",
				cpu);
			exit(2);
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

	data &= 0xFFFFFFFFFFFFFFFEULL;

	if (pwrite(fd, &data, sizeof data, reg) != sizeof data) {
		if (errno == EIO) {
			fprintf(stderr,
				"wrmsr: CPU %d cannot set MSR "
					"0x%08x to 0x%016llx\n",
					cpu, reg, data);
			exit(3);
		} else {
			perror("wrmsr: pwrite");
			exit(127);
		}
	}

	close(fd);

	printf("CPU %d: ", cpu);
	printf("%1llx\n", data);
	return;
}