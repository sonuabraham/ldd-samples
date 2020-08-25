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
#include <linux/init.h>
//#include <base-module.h>

extern void basefun(void);

static int __init derived_init(void)
{
	pr_info("Hello world from derived \n");
	basefun();
	return 0;
}

static void __exit derived_exit(void)
{
	pr_info("Goodbye world from derived \n");
}

module_init(derived_init);
module_exit(derived_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("Module #2 to demostrate module dependencies");
MODULE_LICENSE("GPL v2");
