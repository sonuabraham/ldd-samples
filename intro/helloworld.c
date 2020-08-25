/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init helloworld_init(void)
{
	pr_info("Hello World!!\n");
	return 0;
}

static void __exit helloworld_exit(void)
{
	pr_info("Good bye\n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("Simple Helloworld Module");
MODULE_LICENSE("GPL v2");

