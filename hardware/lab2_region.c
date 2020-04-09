/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Accessing I/O Ports
 *
 * Look at /proc/ioports to find a free I/O port region.
 *
 * Write a simple module that checks if the region is available, and
 * requests it.
 *
 * Check and see if the region is properly registered in /proc/ioports
 *
 * Make sure you release the region when done.
 *
 * The module should send some data to the region, and read some data
 * from it.  Do the values agree?  If not, why?
 */

/* IOPORT FROM 0x200 to 0x240 is free on my system (64 bytes) */
#define IOSTART  0x200
#define IOEXTEND 0x40

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/init.h>

static unsigned long iostart = IOSTART, ioextend = IOEXTEND, ioend;
module_param(iostart, ulong, S_IRUGO);
module_param(ioextend, ulong, S_IRUGO);

static int __init my_init(void)
{
	unsigned long ultest = (unsigned long)100;
	ioend = iostart + ioextend;

	pr_info(" requesting the IO region from 0x%lx to 0x%lx\n",
		iostart, ioend);

	if (!request_region(iostart, ioextend, "my_ioport")) {
		pr_info("the IO REGION is busy, quitting\n");
		return -EBUSY;
	}

	pr_info(" writing a long=%ld\n", ultest);
	outl(ultest, iostart);

	ultest = inl(iostart);
	pr_info(" reading a long=%ld\n", ultest);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info(" releasing  the IO region from 0x%lx to 0x%lx\n",
		iostart, ioend);
	release_region(iostart, ioextend);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap9/lab2_region.c");
MODULE_LICENSE("GPL v2");
