/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/utsname.h>
#include <linux/errno.h>

static void datadrv_cleanup(void)
{
	/* never called */
}

int datadrv_init(void)
{
	/* print information and return an error */
	printk("arch   Size:  char  short  int  long   ptr long-long "
		" u8 u16 u32 u64\n");
	printk("%-12s  %3i   %3i   %3i   %3i   %3i   %3i      "
		"%3i %3i %3i %3i\n",
	        init_uts_ns.name.machine,
		(int)sizeof(char), (int)sizeof(short), (int)sizeof(int),
		(int)sizeof(long),
		(int)sizeof(void *), (int)sizeof(long long), (int)sizeof(__u8),
		(int)sizeof(__u16), (int)sizeof(__u32), (int)sizeof(__u64));
	return -ENODEV;
}

module_init(datadrv_init);
module_exit(datadrv_cleanup);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to list size of standard data types");
MODULE_LICENSE("GPL v2");
