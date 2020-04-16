/*
 * Copyright (C) 20120-2021 Sonu Abraham
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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <string.h>

struct ioctl_data {
	int i;
	long l;
	char s[256];
} ioc_data;

#define LDDIOC_TYPE 'k'

int main(int argc, char *argv[])
{
	int fd, rc;
	int LDD_IOCTL;
	char *devname = "/dev/ldddev";

	/* open the device node */
	fd = open(devname, O_RDWR);
	printf(" Opened device node, file descriptor:%d\n", fd);

	/* retrieve the original values; */
	LDD_IOCTL = (int)_IOR(LDDIOC_TYPE, 1, struct ioctl_data);
	rc = ioctl(fd, LDD_IOCTL, &ioc_data);
	printf("ioctl read returned:%d\n", rc);
	printf
	    ("    oc_data.i = %d\n    ioc_data.x = %ld\n    ioc_data.s = %s\n",
	     ioc_data.i, ioc_data.l, ioc_data.s);

	/* update the data structure */
	strcpy(ioc_data.s, "updated string");
	ioc_data.i = 65;
	ioc_data.l = -90;
	printf("Sending updated data to device:\n");
	printf
	    ("    ioc_data.i = %d\n    ioc_data.l = %ld\n    ioc_data.s = %s\n",
	     ioc_data.i, ioc_data.l, ioc_data.s);

	/* send new data to device */
	LDD_IOCTL = (int)_IOW(LDDIOC_TYPE, 1, struct ioctl_data);
	rc = ioctl(fd, LDD_IOCTL, &ioc_data);
	printf("ioctl write returned:%d\n", rc);

	/* read values again from device  */
	LDD_IOCTL = (int)_IOR(LDDIOC_TYPE, 1, struct ioctl_data);
	rc = ioctl(fd, LDD_IOCTL, &ioc_data);
	printf("ioctl read returned:%d\n", rc);
	printf
	    ("    ioc_data.i = %d\n    ioc_data.l = %ld\n    ioc_data.s = %s\n",
	     ioc_data.i, ioc_data.l, ioc_data.s);

	close(fd);
	exit(0);

}
