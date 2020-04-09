/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	int fd = -1, size = 4096, status;
	char *area;
	pid_t pid;

	area =
	    mmap(NULL, size, PROT_READ | PROT_WRITE,
		 MAP_SHARED | MAP_ANONYMOUS, fd, 0);

	pid = fork();
	if (pid == 0) {		/* child */
		strcpy(area, "This is a message from the child");
		printf("Child has written: %s\n", area);
		exit(EXIT_SUCCESS);
	}
	if (pid > 0) {		/* parent */
		wait(&status);
		printf("Parent has read:   %s\n", area);
		exit(EXIT_SUCCESS);
	}
	exit(EXIT_FAILURE);
}
