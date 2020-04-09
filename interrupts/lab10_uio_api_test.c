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
 * The UIO API (test program)
 *
 */
 
#include  <stdlib.h>
#include  <stdio.h>
#include  <unistd.h>
#include  <fcntl.h>

int main()
{
	int fd;
	unsigned long nint;
	if ((fd = open("/dev/uio0", O_RDONLY)) < 0) {
		perror("Failed to open /dev/uio0\n");
		exit(EXIT_FAILURE);
	}
	fprintf(stderr, "Started uio test driver.\n");
	while (read(fd, &nint, sizeof(nint)) >= 0)
		fprintf(stderr, "Interrupts: %ld\n", nint);
	exit(EXIT_SUCCESS);
}
