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
 * Using the misc API (Using a header)
 *
 * Implement your fullest driver from above with the misc API.
 *    
 * Once again a second solution is given which includes the same
 * header file previously used.
 *
*/

#include <linux/module.h>
#include "lab_miscdev.h"

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_generic_read,
	.write = mycdrv_generic_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
	.llseek = mycdrv_generic_lseek,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab3_chrdrv.c");
MODULE_LICENSE("GPL v2");
