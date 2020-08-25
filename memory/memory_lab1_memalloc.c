/*
 * Copyright (C) 2020-2021 Sonu Abraham
 * Copyright (C) 2020-2021 Embedded Tech Labs Pty Ltd
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <linux/moduleparam.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>

static long size = 4;
module_param(size, long, S_IRUSR | S_IWUSR);
MODULE_PARM_DESC(size, "memory size in Kbytes");

static int __init memalloc_init(void)
{
	void *ptr;

	pr_info("size=%ldkbytes\n", size);

	ptr = kmalloc(size << 10, GFP_KERNEL);
	pr_info("kmalloc(..., GFP_KERNEL) =%px\n", ptr);
	kfree(ptr);

	ptr = kmalloc(size << 10, GFP_ATOMIC);
	pr_info("kmalloc(..., GFP_ATOMIC) =%px\n", ptr);
	kfree(ptr);

	ptr = vmalloc(size << 10);
	pr_info("vmalloc(...)             =%px\n", ptr);
	vfree(ptr);

	ptr = kvmalloc(size << 10, GFP_KERNEL);
	pr_info("kvmalloc(..., GFP_KERNEL)=%px\n", ptr);
	kvfree(ptr);

	ptr = kvmalloc(size << 10, GFP_ATOMIC);
	pr_info("kvmalloc(..., GFP_ATOMIC)=%px\n", ptr);
	kvfree(ptr);

	pr_info("LDDMemAlloc: Module Loaded\n");

	return -EINVAL;
}

static void __exit memalloc_exit(void)
{
	pr_info("LDDMemAlloc: Module Unloaded\n");
}

module_init(memalloc_init);
module_exit(memalloc_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Memory allocation in device drivers");
MODULE_LICENSE("GPL v2");
