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

static int __init datadrv_init(void)
{
	int j = 0;
	struct list_head *modules;
	struct module *m = THIS_MODULE;
	modules = &m->list;
	modules = modules->prev;
	pr_info("\n");
	list_for_each_entry(m, modules, list) {
		pr_info("DataDrv: %3d MOD:%20s, taints = %ul\n", j++, m->name, m->taints);
	}
	return 0;
}

static void __exit datadrv_exit(void)
{
	pr_info("DataDrv: module unloaded from 0x%p\n", datadrv_exit);
}

module_init(datadrv_init);
module_exit(datadrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to find tainted kernel modules");
MODULE_LICENSE("GPL v2");
