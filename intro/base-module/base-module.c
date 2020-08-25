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

static int __init base_init(void)
{
	pr_info("Hello world from base \n");
	return 0;
}

static void __exit base_exit(void)
{
	pr_info("Goodbye world from base \n");
}

void basefun(void)
{
	pr_info("Base function invoked \n");
}

EXPORT_SYMBOL(basefun);

module_init(base_init);
module_exit(base_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("Module #1 to demostrate module dependencies");
MODULE_LICENSE("GPL v2");
