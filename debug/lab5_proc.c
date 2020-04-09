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

* Using the /proc filesystem. (/proc/driver solution)
*
* Write a module that creates a /proc filesystem entry and can read
* and write to it.
*
* When you read from the entry, you should obtain the value of some
* parameter set in your module.
*
* When you write to the entry, you should modify that value, which
* should then be reflected in a subsequent read.
*
* Make sure you remove the entry when you unload your module.  What
* happens if you don't and you try to access the entry after the
* module has been removed?
*
* There are two different solutions given, one which creates the entry
* in the /proc directory, the other in /proc/driver.
*/

#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include<linux/slab.h>

#if 0
#define NODE "my_proc"
#else
#define NODE "driver/my_proc"
#endif

static int param = 100;
static struct proc_dir_entry *my_proc;

static int
my_proc_read(char *page, char **start, off_t off, int count,
	     int *eof, void *data)
{
	*eof = 1;
	return sprintf(page, "%d\n", param);
}

static int
my_proc_write(struct file *file, const char __user * buffer,
	      unsigned long count, void *data)
{
	char *str;
	str = kmalloc((size_t) count, GFP_KERNEL);
	if (copy_from_user(str, buffer, count)) {
		kfree(str);
		return -EFAULT;
	}
	sscanf(str, "%d", &param);
	pr_info("param has been set to %d\n", param);
	kfree(str);
	return count;
}

static int __init my_init(void)
{
	my_proc = create_proc_entry(NODE, S_IRUGO | S_IWUSR, NULL);
	if (!my_proc) {
		pr_err("I failed to make %s\n", NODE);
		return -1;
	}
	pr_info("I created %s\n", NODE);
	my_proc->read_proc = my_proc_read;
	my_proc->write_proc = my_proc_write;
	return 0;
}

static void __exit my_exit(void)
{
	if (my_proc) {
		remove_proc_entry(NODE, NULL);
		pr_info("Removed %s\n", NODE);
	}
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab5_proc.c");
MODULE_LICENSE("GPL v2");
