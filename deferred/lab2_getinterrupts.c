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
 * Sharing All Interrupts  (Helper Application)
 *
 * Extend the previous solution to construct a character driver that
 * shares every possible interrupt with already installed handlers.
 *
 * The highest interrupt number you have to consider will depend on
 * your kernel and platform; look at /proc/interrupts to ascertan what
 * is necessary.
 *
 * Take particular care when you call free_irq() as it is very easy to
 * freeze your system if you are not careful.
 *
 * The character driver can be very simple; for instance if no open()
 * and release() methods are specified, success is the default.
 *
 * A read() on the device should return a brief report on the total
 * number of interrupts handled for each IRQ.
 *
 * To do this you'll also have to write a short application to
 * retrieve and print out the data.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define DEATH(mess) { perror(mess); exit(errno); };

#define MAXIRQS 256
#define NB (MAXIRQS * sizeof(int))

int main(int argc, char *argv[])
{
	int fd, j;
	char *nodename = "/dev/mycdrv";
	int *interrupts = malloc(NB);
	if (argc > 1)
		nodename = argv[1];

	if ((fd = open(nodename, O_RDONLY)) < 0)
		DEATH("opening device node");
	if (read(fd, interrupts, NB) != NB)
		DEATH("reading interrupts");

	for (j = 0; j < MAXIRQS; j++)
		if (interrupts[j] > 0)
			printf(" %4d %10d\n", j, interrupts[j]);
	exit(0);
}
