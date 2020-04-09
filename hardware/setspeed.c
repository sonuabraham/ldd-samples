/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char **argv)
{
	int fd, rc;
	char *c;
	struct termios tty;
	unsigned short flags = (CREAD | CLOCAL | HUPCL);

	if (argc < 2) {
		printf(" give /dev/ttyS0 or /dev/ttyS1 as an argument!\n");
		exit(-1);
	}

	fd = open(argv[1], O_RDWR | O_NONBLOCK);
	printf(" fd = %d from opening %s\n", fd, argv[1]);
	rc = fcntl(fd, F_GETFL);
	rc = fcntl(fd, F_SETFL, O_RDWR);

	rc = tcgetattr(fd, &tty);
	printf("rc = %d from tcsetattr\n", rc);

	tty.c_iflag = IGNBRK | IGNPAR;
	tty.c_oflag = 0;
	tty.c_lflag = 0;
	tty.c_line = 0;
	tty.c_cc[VTIME] = 0;
	tty.c_cc[VMIN] = 1;
	tty.c_cflag = flags | B1200;
	c = "*n";

	rc = write(fd, c, 2);
	printf("rc = %d from write\n", rc);
	usleep(100000);

	rc = tcsetattr(fd, TCSAFLUSH, &tty);
	printf("rc = %d from tcsetattr\n", rc);

	close(fd);
	exit(0);
}
