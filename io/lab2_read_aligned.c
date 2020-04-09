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
 * Basic read program USAGE: foo [filename(def=/dev/mycdrv)] [nbytes(def=4096)]
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>

int main(int argc, char *argv[])
{
	int fd, rc = 0, nbytes, pagesize;
	void *buffer;
	char *filename = "/dev/mycdrv";

	pagesize = getpagesize();
	nbytes = pagesize;

	if (argc > 1)
		filename = argv[1];
	if (argc > 2)
		nbytes = atoi(argv[2]) * pagesize;

	rc = posix_memalign(&buffer, pagesize, nbytes);
	/*    buffer = (char *)memalign (pagesize, nbytes);  */

	printf("rc=%d, buffer=%p, mod pagesize = %ld\n", rc, buffer,
	       (unsigned long)buffer % pagesize);

	fd = open(filename, O_RDONLY);
	printf("opened file: %s,  with file descriptor = %d\n", filename, fd);
	rc = read(fd, buffer, nbytes);
	printf("read %d bytes which were:\n%s\n", rc, (char *)buffer);
	close(fd);
	exit(EXIT_SUCCESS);
}
