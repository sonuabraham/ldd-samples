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
#include <linux/moduleparam.h>

static int param_int = 12;
static char *param_str = "Module input string";
static int param_array[4] = {0,1,2,3};

module_param(param_int, int, S_IRUGO | S_IWUSR);
module_param(param_str,charp,S_IRUGO);
module_param_array(param_array,int,NULL,S_IWUSR|S_IRUSR);

MODULE_PARM_DESC(param_int,"Module integer parameter");
MODULE_PARM_DESC(param_str,"Module char pointer parameter");
MODULE_PARM_DESC(param_array,"Module integer array parameter");

static int __init moduleparams_init(void)
{
	pr_info("Loading moduleparams module\n");
	pr_info(" param_int = %d\n", param_int);
	pr_info(" param_str = %s\n", param_str);
	pr_info(" param_array elements: %d\t%d\t%d\t%d\n", param_array[0],param_array[1],param_array[2],param_array[3]);
	return 0;
}

static void __exit moduleparams_exit(void)
{
	pr_info("Unloading moduleparams module\n");
}

module_init(moduleparams_init);
module_exit(moduleparams_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module parameters");
MODULE_LICENSE("GPL v2");
