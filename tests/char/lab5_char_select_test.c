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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
	int fd,ret,nbytes = 32;
	fd_set read_fds;
	char *buffer,*filename = "/dev/ldddev";

	buffer = malloc(nbytes);
	
	ret = open(filename, O_RDWR);
	if (ret < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	printf("opened %s successfully\n", filename);
	fd = ret;

	while (1) {
		/* Set up reading file descriptors */
		FD_ZERO(&read_fds);
		FD_SET(fd, &read_fds);

		/* Wait for data from device  */
		ret = select(FD_SETSIZE, &read_fds, NULL, NULL, NULL);
		if (ret < 0) {
			perror("select");
			exit(EXIT_FAILURE);
		}

		if (FD_ISSET(fd, &read_fds)) {
			ret = read(fd, buffer, nbytes);
			if (ret < 0) {
				perror("read");
				exit(EXIT_FAILURE);
			}
			printf("read %s from device!\n", buffer);
		}
	}
	
	free(buffer);
	close(fd);

	return 0;
}
