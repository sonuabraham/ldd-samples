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
#include "debug.h"

static int __init dbgprint_init(void)
{
	
	printk(KERN_DEBUG"LDDDbgPrint: I'm in : %s:%i\n", __FILE__, __LINE__);
	printk(KERN_INFO"LDDDbgPrint: I'm in %s function\n",__FUNCTION__);
	printk(KERN_NOTICE"LDDDbgPrint: I'm a notice message\n");
	printk(KERN_WARNING"LDDDbgPrint: I'm a warning message.Attention required!!!\n");
	printk(KERN_ERR"LDDDbgPrint: I'm an error message.Error condition detected!!!\n");
	printk(KERN_CRIT"LDDDbgPrint: I'm a critical message.Serious h/w or s/w failure detected!!!\n");
	printk(KERN_ALERT"LDDDbgPrint: I'm an alert message.Immediate action required!!!\n");
	printk(KERN_EMERG"LDDDbgPrint: I'm an emergency message.I'm going to crash!!!\n");


	if (printk_ratelimit())
	{
		printk(KERN_NOTICE"LDDDbgPrint: I'm a rate limited message\n");
	}


	PDEBUG("This is a custom message from  %s ","Sonu Abraham");
	pr_info("LDDDbgPrint: Loaded debugprints module1\n");
    return 0;
}

static void __exit dbgprint_exit(void)
{
	pr_info("LDDDbgPrint: Unloaded debugprints module\n");
}

module_init(dbgprint_init);
module_exit(dbgprint_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate debugging using printing");
MODULE_LICENSE("GPL v2");
