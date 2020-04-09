/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/*
 * Using Relay Channels. (mmap testing program)
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define N_SB 16

int main(int argc, char **argv)
{
	int fd, size;
	char *area, *fname = "/sys/kernel/debug/my_rc_file0";

	if (argc > 1)
		fname = argv[1];

	size = N_SB * getpagesize();	/* make sure this agrees with module! */

	if (argc > 2)
		size = atoi(argv[2]);

	printf(" Memory Mapping %s, size %d bytes\n", fname, size);

	if ((fd = open(fname, O_RDONLY)) < 0) {
		perror("problems opening the file ");
		exit(errno);
	}
	area = mmap(NULL, size, PROT_READ, MAP_SHARED, fd, 0);
	if (area == MAP_FAILED) {
		perror("error mmaping");
		exit(errno);
	}
	write(STDOUT_FILENO, area, 64 * 20);	/* may want to write out more */
	close(fd);
	exit(0);
}
