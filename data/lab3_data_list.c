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
#include <asm/atomic.h>
#include <linux/errno.h>
#include <linux/slab.h>
#include <linux/init.h>

/* initialize the link list */
static LIST_HEAD(data_list);

struct data_entry {
	struct list_head list;	
	int intvar;		
	char strvar[16];		
};

static int __init datadrv_init(void)
{
	struct data_entry *de;
	int k;

	for (k = 0; k < 5; k++) {

		if (!(de = kmalloc(sizeof(struct data_entry), GFP_KERNEL))) {
			pr_info
			    ("DataDrv: failed to allocate memory for data entry %d\n",
			     k);
			return -ENOMEM;
		}

		de->intvar = k;	
		sprintf(de->strvar, "Data_%d", k + 1);
		pr_info("DataDrv: adding %s at index %d to data_list\n",
			de->strvar, de->intvar);
		list_add(&de->list, &data_list);
	}
	return 0;
}

static void __exit datadrv_exit(void)
{
	struct list_head *list;	/* pointer to list head object */
	struct list_head *tmp;	/* temporary list head for safe deletion */

	if (list_empty(&data_list)) {
		pr_info("DataDrv: (exit): data list is empty!\n");
		return;
	}
	pr_info("DataDrv: (exit): data list is not empty!\n");

	list_for_each_safe(list, tmp, &data_list) {
		struct data_entry *de =
		    list_entry(list, struct data_entry, list);
		list_del(&de->list);
		pr_info("DataDrv (exit): %s at index %d removed from list\n",
			de->strvar, de->intvar);
		kfree(de);
	}

	/* confirm that the list is empty*/
	if (list_empty(&data_list))
		pr_info("DataDrv (done): data list is empty!\n");
	else
		pr_info("Camels: (done): data list is not empty!\n");
}

module_init(datadrv_init);
module_exit(datadrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD Module to illustrate link list usage in linux device drivers");
MODULE_LICENSE("GPL v2");
