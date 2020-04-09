/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* IOPORT FROM 0x200 to 0x240 is free on my system (64 bytes) */
#define IOSTART  0x200
#define IOEXTEND 0x40

#include <linux/module.h>
#include <linux/ioport.h>
#include <linux/jiffies.h>
#include <linux/io.h>
#include <linux/init.h>

#define NLOOP 1000000		/* should be a multiple of millions */
#define BILL 1000000000		/* make the time in nanoseconds) */

static int __init my_init(void)
{
	int j;
	unsigned long ultest = (unsigned long)1000;
	unsigned long jifa, jifb, jifc, jifd;

	if (!request_region(IOSTART, IOEXTEND, "my_ioport")) {
		pr_info("the IO REGION is busy, quitting\n");
		return -EBUSY;
	}
	pr_info(" requesting the IO region from 0x%x to 0x%x\n",
		IOSTART, IOSTART + IOEXTEND);

	/* get output delays */

	jifa = jiffies;
	for (j = 0; j < NLOOP; j++)
		outl(ultest, IOSTART);
	jifb = jiffies;
	jifc = jiffies;
	for (j = 0; j < NLOOP; j++)
		outl_p(ultest, IOSTART);
	jifd = jiffies;
	pr_info("outl: nsec/op=%ld  outl_p: nsec/op=%ld   nsec delay/op=%ld\n",
		(jifb - jifa) * (BILL / NLOOP) / HZ,
		(jifd - jifc) * (BILL / NLOOP) / HZ,
		((jifd - jifc) - (jifb - jifa)) * (BILL / NLOOP) / HZ);

	/* get input delays */

	jifa = jiffies;
	for (j = 0; j < NLOOP; j++)
		ultest = inl(IOSTART);
	jifb = jiffies;
	jifc = jiffies;
	for (j = 0; j < NLOOP; j++)
		ultest = inl_p(IOSTART);
	jifd = jiffies;
	pr_info(" inl: nsec/op=%ld   inl_p: nsec/op=%ld   nsec delay/op=%ld\n",
		(jifb - jifa) * (BILL / NLOOP) / HZ,
		(jifd - jifc) * (BILL / NLOOP) / HZ,
		((jifd - jifc) - (jifb - jifa)) * (BILL / NLOOP) / HZ);
	return 0;
}

static void __exit my_exit(void)
{
	pr_info(" releasing  the IO region from 0x%x to 0x%x\n",
		IOSTART, IOSTART + IOEXTEND);
	release_region(IOSTART, IOEXTEND);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap9/delay_test.c");
MODULE_LICENSE("GPL v2");
