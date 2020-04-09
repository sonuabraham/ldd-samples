/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/jiffies.h>

static int x_delay = 1;		/* the default delay */

static int
x_read_busy(char *buf, char **start, off_t offset, int len,
	    int *eof, void *unused)
{
	unsigned long j = jiffies + x_delay * HZ;

	while (time_before(jiffies, j))
		/* nothing */ ;
	*eof = 1;
	return sprintf(buf, "jiffies = %d\n", (int)jiffies);
}

static struct proc_dir_entry *x_proc_busy;

static int __init my_init(void)
{
	x_proc_busy = create_proc_entry("x_busy", 0, NULL);
	x_proc_busy->read_proc = x_read_busy;
	return 0;
}

static void __exit my_exit(void)
{
	if (x_proc_busy)
		remove_proc_entry("x_busy", NULL);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab13_busy.c");
MODULE_LICENSE("GPL v2");
