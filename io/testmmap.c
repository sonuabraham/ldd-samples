/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/mman.h>

#define DEATH(mess) { perror(mess); exit(errno); }

int main(int argc, char **argv)
{
	int fd, size, rc, j;
	char *area, *tmp, *nodename = "/dev/mycdrv";
	char c[2] = "CX";

	if (argc > 1)
		nodename = argv[1];

	size = getpagesize();	/* use one page by default */
	if (argc > 2)
		size = atoi(argv[2]);

	printf(" Memory Mapping Node: %s, of size %d bytes\n", nodename, size);

	if ((fd = open(nodename, O_RDWR)) < 0)
		DEATH("problems opening the node ");

	area = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (area == MAP_FAILED)
		DEATH("error mmaping");

	/* can close the file now */

	close(fd);

	/* put the string repeatedly in the file */

	tmp = area;
	for (j = 0; j < size - 1; j += 2, tmp += 2)
		memcpy(tmp, &c, 2);

	/* just cat out the file to see if it worked */

	rc = write(STDOUT_FILENO, area, size);

	if (rc != size)
		DEATH("problems writing");

	exit(EXIT_SUCCESS);
}
