/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>

int main(int argc, char *argv[])
{
	struct pollfd ufds[1];
	int timeout = 10000;	/* time out for poll */
	int fd, rc, nbytes = 32;
	char *buffer, *filename = "/dev/ldddev";

	buffer = malloc(nbytes);

	fd = open(filename, O_RDONLY);
	printf("opened file: %s,  with file descriptor = %d\n", filename, fd);

	ufds[0].fd = fd;
	ufds[0].events = POLLIN;

	for (;;) {

		if ((rc = poll(ufds, 1, timeout)) < 0) {
			perror("Failure in poll\n");
			exit(EXIT_FAILURE);
		}

		if (rc > 0) {

			printf(" poll returns %d, revents = 0x%03x", rc,
			       ufds[0].revents);

			if (ufds[0].revents & POLLIN) {
				rc = read(fd, buffer, nbytes);
				printf("reading %d bytes:%s\n", rc, buffer);
			} else {
				printf("POLLIN not set!\n");
			}

		} else {
			printf("poll timed out in %d milliseconds on %s.\n",
			       timeout, filename);
		}
	}

	free(buffer);
	close(fd);
	printf("Shutting down %s\n", argv[0]);
	exit(EXIT_SUCCESS);
}
