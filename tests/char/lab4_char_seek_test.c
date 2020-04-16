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
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int length = 20, position = 0, fd, rc;
	char *message, *nodename = "/dev/ldddev";

	/* set up the message */
	message = malloc(length);
	memset(message, '$', length);
	message[length - 1] = '\0';	

	/* open the device node */
	fd = open(nodename, O_RDWR);
	printf("Opened device node, file descriptor:%d\n", fd);

	/* seek to position */
	rc = lseek(fd, position, SEEK_SET);
	printf("1st lseek returned: %d\n", rc);

	/* write to the device node twice */
	rc = write(fd, message, length);
	printf("1st write returned:%d\n", rc);
	rc = write(fd, message, length);
	printf("2nd write returned:%d\n", rc);

	/* reset the message to null */
	memset(message, 0, length);

	/* seek to position */
	rc = lseek(fd, position, SEEK_SET);
	printf("2nd lseek returned:%d\n", rc);

	/* read from the device node */
	rc = read(fd, message, length);
	printf("2nd read returned:%d\n", rc);
	printf("message read: %s\n", message);

	/* close the device node */
	close(fd);
	exit(0);

}
