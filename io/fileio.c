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
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/fs.h>

static char *filename = "/tmp/tempfile";
module_param(filename, charp, S_IRUGO);

int kernel_write(struct file *file, unsigned long offset,
		 char *addr, unsigned long count)
{
	mm_segment_t old_fs;
	loff_t pos = offset;
	int result;

	old_fs = get_fs();
	set_fs(get_ds());
	/* The cast to a user pointer is valid due to the set_fs() */
	result = vfs_write(file, (void __user *)addr, count, &pos);
	set_fs(old_fs);
	return result;
}

#define NBYTES_TO_READ 20

/* adapted from kernel_read() in kernel/exec.c */

static int __init my_init(void)
{
	struct file *f;
	int nbytes, j;
	char *buffer;
	char newstring[] = "NEWSTRING";

	buffer = kmalloc(PAGE_SIZE, GFP_KERNEL);
	pr_info("Trying to open file = %s\n", filename);
	f = filp_open(filename, O_RDWR, S_IWUSR | S_IRUSR);

	if (IS_ERR(f)) {
		pr_info("error opening %s\n", filename);
		kfree(buffer);
		return -EIO;
	}

	nbytes = kernel_read(f, f->f_pos, buffer, NBYTES_TO_READ);

	pr_info("I read nbytes = %d, which were: \n\n", nbytes);
	for (j = 0; j < nbytes; j++)
		pr_info("%c", buffer[j]);

	strcpy(buffer, newstring);
	nbytes = kernel_write(f, f->f_pos, buffer, strlen(newstring) + 1);
	pr_info("\n\n I wrote nbytes = %d, which were %s \n", nbytes,
		newstring);

	filp_close(f, NULL);
	kfree(buffer);

	return 0;
}

static void __exit my_exit(void)
{
	pr_info("\nclosing up\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap15/fileio.c");
MODULE_LICENSE("GPL v2");
