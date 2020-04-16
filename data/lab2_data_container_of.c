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
#include <linux/slab.h>		


struct test1 {
    int a;
};

struct test2 {
    int b;
    struct test1 z;
    int c;
};

static int __init datadrv_init(void)
{
	struct test2 *obj = NULL,*obj2 = NULL;
    struct test1 *obj1= NULL;

    pr_info("DataDrv: init \n");
    obj = (struct test2* )kmalloc(sizeof(struct test2), GFP_KERNEL);
    if(obj == NULL){
        pr_info("DataDrv: Cannot allocate memory...!\n");
		return -1;
    }
    obj->z.a = 25;
    obj->b   = 35;
    obj->c   = 45;

    /* pointer to existing entry */
    obj1 = &obj->z;

    obj2 = container_of(obj1, struct test2, z);

    pr_info("DataDrv: obj2->b = %d\n", obj2->b);

	kfree(obj);

	return 0;
}

static void __exit datadrv_exit(void)
{
	pr_info("DataDrv: exit \n");
}

module_init(datadrv_init);
module_exit(datadrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate container_of macro");
MODULE_LICENSE("GPL v2");

