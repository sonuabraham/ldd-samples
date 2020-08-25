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
 * Adding Asynchronous Entry Points to a Character Driver
 *
 *   (Linux API test program)
 *
 * Take one of your earlier character drivers and add new entry points
 * for aio_read() and aio_write().
 *
 * To test this you'll need to write a user-space program that uses
 * the native Linux API.  Have it send out a number of write and read
 * requests and synchronize properly.
 *
 * We also present a solution using the Posix API for the user
 * application; note that this will never hit your driver unless you
 * comment out the normal read and write entry points in which case
 * the kernel will fall back on the asynchronous ones.
 *
 * Make sure you compile by linking with the right libraries; use
 * -laio for the Linux API and -lrt for the Posix API.  (You can use
 * both in either case as they don't conflict.)
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <aio.h>
#include <sys/stat.h>


#include <sys/syscall.h>
#include <linux/aio_abi.h>

#define NBYTES 32
#define NBUF   512

inline int io_setup(unsigned nr, aio_context_t *ctxp) {
	return syscall(__NR_io_setup, nr, ctxp);
}

inline int io_destroy(aio_context_t ctx) {
	return syscall(__NR_io_destroy, ctx);
}

inline int io_submit(aio_context_t ctx, long nr, struct iocb **iocbpp) {
	return syscall(__NR_io_submit, ctx, nr, iocbpp);
}

inline int io_getevents(aio_context_t ctx, long min_nr, long max_nr,
		struct io_event *events, struct timespec *timeout) {
	return syscall(__NR_io_getevents, ctx, min_nr, max_nr, events, timeout);
}


void printbufs(char **buf, int nbytes)
{
	int j;
	fflush(stdout);
	for (j = 0; j < NBUF; j++) {
		write(STDOUT_FILENO, buf[j], nbytes);
		printf("\n");
	}
	printf("\n");
}

int main(int argc, char *argv[])
{
	int fd, rc, j, k, nbytes = NBYTES, maxevents = NBUF;
	char *buf[NBUF], *filename = "/dev/scullc";
	struct iocb *iocbray[NBUF], *iocb;
	off_t offset;
	aio_context_t ctx = 0;
	struct io_event events[2 * NBUF];
	struct timespec timeout = { 0, 0 };

	/* open or create the file and fill it with a pattern */

	if (argc > 1)
		filename = argv[1];

	printf("opening %s\n", filename);

	/* notice opening with these flags won't hurt a device node! */

	if ((fd = open(filename, O_RDWR | O_CREAT | O_TRUNC,
		       S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
		printf("couldn't open %s, ABORTING\n", filename);
		exit(-1);
	}

	/* write initial data out, clear buffers, allocate iocb's */

	for (j = 0; j < NBUF; j++) {
		/* no need to zero iocbs; will be done in io_prep_pread */
		iocbray[j] = malloc(sizeof(struct iocb));
		buf[j] = malloc(nbytes);
		sprintf(buf[j], "%4d%4d%4d%4d%4d%4d%4d%4d", j, j, j, j, j, j, j,
			j);
		write(fd, buf[j], nbytes);
		memset(buf[j], 0, nbytes);
	}
	printf("\n");

	/* prepare the context */

	rc = io_setup(maxevents, &ctx);
	printf(" rc from io_setup = %d\n", rc);

	/* (async) read the data from the file */

	printf(" reading initial data from the file:\n");

	for (j = 0; j < NBUF; j++) {
		iocb = iocbray[j];
		offset = j * nbytes;
		//io_prep_pread(iocb, fd, (void *)buf[j], nbytes, offset);

		iocb->aio_buf = buf[j];
		iocb->aio_offset = offset;
		iocb->aio_nbytes = nbytes;
		rc = io_submit(ctx, 1, &iocb);
	}

	/* sync up and print out the readin data */

	while ((rc = io_getevents(ctx, NBUF, NBUF, events, &timeout)) > 0) {
		printf(" rc from io_getevents on the read = %d\n\n", rc);
	}

	printbufs(buf, nbytes);

	/* filling in the buffers before the write */

	for (j = 0; j < NBUF; j++) {
		char *tmp = buf[j];
		for (k = 0; k < nbytes; k++) {
			sprintf((tmp + k), "%1d", j);
		}
	}

	/* write the changed buffers out */

	printf(" writing new data to the file:\n");
	for (j = 0; j < NBUF; j++) {
		iocb = iocbray[j];
		offset = j * nbytes;
		//io_prep_pwrite(iocb, fd, buf[j], nbytes, offset); //TODO 
		rc = io_submit(ctx, 1, &iocb);
	}

	/* sync up again */

	while ((rc = io_getevents(ctx, NBUF, NBUF, events, &timeout)) > 0) {
		printf(" rc from io_getevents on the write = %d\n\n", rc);
	}

	printbufs(buf, nbytes);

	/* clean up */
	rc = io_destroy(ctx);
	close(fd);
	exit(0);
}
