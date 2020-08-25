/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#define IOSTART  0x3F4
#define IOEXTEND 0x1

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/io.h>
#include <linux/init.h>

static unsigned long iostart = IOSTART, ioextend = IOEXTEND, ioend;
module_param(iostart, ulong, S_IRUGO);
module_param(ioextend, ulong, S_IRUGO);

#define SHORT_NR_PORTS	3	/* use 8 ports by default */
static unsigned long base = 0x378; // 0x378;
unsigned long short_base = 0;

static int __init my_init(void)
{
	unsigned char ultest = (unsigned char)45;
	#if 0
	ioend = iostart + ioextend;

	release_region(iostart,ioextend);

	pr_info(" requesting the IO region from 0x%lx to 0x%lx\n",
		iostart, ioend);

	if (!request_region(iostart, ioextend, "my_ioport")) {
		pr_info("the IO REGION is busy, quitting\n");
		return -EBUSY;
	}
#endif
		short_base = base;


	release_region(short_base,SHORT_NR_PORTS);


		if (!request_region(short_base, SHORT_NR_PORTS, "shortest2")) {
			printk(KERN_INFO "short: can't get I/O port address 0x%lx\n",
					short_base);
			return -ENODEV;
		}

	pr_info(" writing a char=%x\n", ultest);
	outb(ultest, short_base);

	ultest = inb(short_base);
	pr_info(" reading a char=%x\n", ultest);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info(" releasing  the IO region from 0x%lx to 0x%lx\n",
		short_base, ioend);
	release_region(short_base, SHORT_NR_PORTS);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Accessing I/O ports");
MODULE_LICENSE("GPL v2");
