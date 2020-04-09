/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Remapping I/O Ports
 *
 * Alter your solution to use ioport_map() and the proper reading and
 * writing functions.
 */

/* IOPORT FROM 0x200 to 0x240 is free on my system (64 bytes) */
#define IOSTART  0x200
#define IOEXTEND 0x40

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/init.h>

static char __iomem *mapped;
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

	mapped = ioport_map(iostart, ioextend);

	pr_info("ioport mapped at %p\n", mapped);
	pr_info(" writing a long=%ld\n", ultest);
	iowrite32(ultest, mapped);

	ultest = ioread32(mapped);
	pr_info(" reading a long=%ld\n", ultest);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info(" releasing  the IO region from 0x%lx to 0x%lx\n",
		iostart, ioend);
	release_region(iostart, ioextend);
	ioport_unmap(mapped);

}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap9/lab3_map.c");
MODULE_LICENSE("GPL v2");
