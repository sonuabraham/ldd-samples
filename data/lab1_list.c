/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Linked lists
 *
 * Write a module that sets up a doubly-linked circular list of data
 * structures.  The data structure can be as simple as an integer
 * variable.
 *
 * Test inserting and deleting elements in the list.
 *
 * Walk through the list (using list_entry()) and print out values to
 * make sure the insertion and deletion processes are working.
 */

#include <linux/module.h>
#include <asm/atomic.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/init.h>

/* we're going to do a camel race! */
static LIST_HEAD(camel_list);

struct camel_entry {
	struct list_head clist;	/* link in camel list */
	int gate;		/* assigned gate at Camel Derby */
	char name[20];		/* camel's name */
};

static int __init my_init(void)
{
	struct camel_entry *ce;
	int k;

	for (k = 0; k < 5; k++) {

		if (!(ce = kmalloc(sizeof(struct camel_entry), GFP_KERNEL))) {
			pr_info
			    (" Camels: failed to allocate memory for camel %d \n",
			     k);
			return -ENOMEM;
		}

		ce->gate = 11 + k;	/* gate number */
		sprintf(ce->name, "Camel_%d", k + 1);
		pr_info(" Camels: adding %s at gate %d to camel_list \n",
			ce->name, ce->gate);
		list_add(&ce->clist, &camel_list);
	}
	return 0;
}

static void __exit my_exit(void)
{
	struct list_head *list;	/* pointer to list head object */
	struct list_head *tmp;	/* temporary list head for safe deletion */

	if (list_empty(&camel_list)) {
		pr_info("Camels (exit): camel list is empty! \n");
		return;
	}
	pr_info("Camels: (exit): camel list is NOT empty! \n");

	list_for_each_safe(list, tmp, &camel_list) {
		struct camel_entry *ce =
		    list_entry(list, struct camel_entry, clist);
		list_del(&ce->clist);
		pr_info("Camels (exit): %s at gate %d removed from list \n",
			ce->name, ce->gate);
		kfree(ce);
	}

	/* Now, did we remove the camel manure? */

	if (list_empty(&camel_list))
		pr_info("Camels (done): camel list is empty! \n");
	else
		pr_info("Camels: (done): camel list is NOT empty! \n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap11/lab1_list.c");
MODULE_LICENSE("GPL v2");
