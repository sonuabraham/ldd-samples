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
 * Using Relay Channels. (read()reading program)
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int fd, j, rc;
	char buf[64];
	char *fname = "/sys/kernel/debug/my_rc_file0";
	if (argc > 1)
		fname = argv[1];
	fd = open(fname, O_RDONLY);
	printf("opening %s, fd=%d\n", fname, fd);

	for (j = 1; j < 20; j++) {
		rc = read(fd, buf, 64);
		printf("rc=%d    %s::", rc, buf);
	}
	exit(0);
}
