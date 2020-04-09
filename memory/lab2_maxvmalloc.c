/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */
/*
 * Testing Maximum Memory Allocation (vmalloc)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/vmalloc.h>

static int mem = 64;
module_param(mem, int, S_IRUGO);

#define MB (1024*1024)

static int __init my_init(void)
{
	static char *vm_buff;
	long size;

	/* try vmalloc */

	for (size = 4 * MB; size <= mem * MB; size += 4 * MB) {
		pr_info(" pages=%6ld, size=%8ld ", size / PAGE_SIZE, size / MB);
		if (!(vm_buff = (char *)vmalloc(size))) {
			pr_err("... vmalloc failed\n");
			break;
		}
		pr_info("... vmalloc OK\n");
		vfree(vm_buff);
	}

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Module Unloading\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap8/lab2_maxvmalloc.c");
MODULE_LICENSE("GPL v2");
