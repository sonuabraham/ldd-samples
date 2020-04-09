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
 * Basic page-aligned write program USAGE: foo [filename(def=/dev/mycdrv)]
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <malloc.h>
#include <sys/stat.h>

#define STRING "TESTING DRIVER WITH A WRITE"

int main(int argc, char *argv[])
{
	int fd, rc = 0, size, pagesize;
	void *buffer;
	char *filename = "/dev/mycdrv";
	size = strlen(STRING) + 1;	/* leave room for \0 */
	pagesize = getpagesize();
	rc = posix_memalign(&buffer, pagesize, size);
	/*    buffer = (char *)memalign (pagesize, size); */
	printf("rc=%d, buffer=%p, mod pagesize = %ld\n", rc, buffer,
	       (unsigned long)buffer % pagesize);
	strcpy(buffer, STRING);

	if (argc > 1)
		filename = argv[1];

	fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
	printf("opened file: %s,  with file descriptor = %d\n", filename, fd);
	rc = write(fd, buffer, strlen(buffer) + 1);
	printf("wrote %d bytes which were:\n%s\n", rc, (char *)buffer);
	close(fd);
	exit(EXIT_SUCCESS);
}
