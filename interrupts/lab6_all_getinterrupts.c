/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Sharing All Interrupts, Bottom Halves (get results application)
 *
 * Extend the solution to share all possible interrupts, and evaluate
 * the consumer/producer problem.
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
	int *bhs = malloc(NB);

	if (argc > 1)
		nodename = argv[1];

	if ((fd = open(nodename, O_RDONLY)) < 0)
		DEATH("opening device node");
	if (read(fd, interrupts, NB) != NB)
		DEATH("reading interrupts");
	if (read(fd, bhs, NB) != NB)
		DEATH("reading bhs");

	for (j = 0; j < MAXIRQS; j++)
		if (interrupts[j] > 0)
			printf(" %4d %10d%10d\n", j, interrupts[j], bhs[j]);
	exit(0);
}
