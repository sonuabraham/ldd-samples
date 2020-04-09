/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Finding Tainted Modules
 *
 * All modules loaded on the system are linked in a list that can be
 * accessed from any module:
 *
 *    struct module {
 *     ....
 *    struct list_head list;
 *    ....
 *    char name[MODULE_NAME_LEN];
 *    ....
 *    unsigned int taints;
 *    ....
 *    }
 *
 * Write a module that walks through this linked list and prints out
 * the value of taints and any other values of interest.  (The module
 * structure is defined in /usr/src/linux/include/linux/module.h.)
 *
 * You can begin from THIS_MODULE.
 */

#include <linux/module.h>
#include <linux/init.h>

static int __init my_init(void)
{
	int j = 0;
	struct list_head *modules;
	struct module *m = THIS_MODULE;
	modules = &m->list;
	modules = modules->prev;
	pr_info("\n");
	list_for_each_entry(m, modules, list) {
		pr_info("%3d MOD:%20s, taints = %u\n", j++, m->name, m->taints);
	}
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap11/lab2_taints.c");
MODULE_LICENSE("GPL v2");
