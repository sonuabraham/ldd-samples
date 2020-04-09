/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void)
{
	int *i;
	i = 0;
	pr_info("Hello: init_module loaded at address 0x%p\n", init_module);
	pr_info("i=%d\n", *i);
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Hello: cleanup_module loaded at address 0x%p\n",
		cleanup_module);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab9_oopsit.c");
MODULE_LICENSE("GPL v2");
