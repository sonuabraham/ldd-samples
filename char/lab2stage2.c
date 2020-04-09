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
#include <linux/fs.h>		
#include <linux/uaccess.h>	
#include <linux/slab.h>		
#include <linux/cdev.h>		

static char *membuff;
#define membuff_size (size_t) (32*PAGE_SIZE)
#define CHARDEV_NAME "chardev"
static dev_t first;
static unsigned int count = 1;
static int chardev_major = 400, chardev_minor = 0;
static struct cdev *chardev_cdev;

static int chardrv_open(struct inode *inode, struct file *file)
{
	pr_info("Opening char device: %s\n", CHARDEV_NAME);
	return 0;
}

static int chardrv_release(struct inode *inode, struct file *file)
{
	pr_info("Closing char device: %s\n", CHARDEV_NAME);
	return 0;
}

static ssize_t
chardrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int rbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("READ: Cannot read beyond end of device..\n");
		return 0;
	}
	rbytes = lbuf - copy_to_user(buf, membuff + *ppos, lbuf);
	*ppos += rbytes;
	pr_info("\n READ: nbytes=%d, pos=%d\n", rbytes, (int)*ppos);
	return rbytes;
}

static ssize_t
chardrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int wbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("WRITE: Cannot write beyond end of device..\n");
		return 0;
	}
	wbytes = lbuf - copy_from_user(membuff + *ppos, buf, lbuf);
	*ppos += wbytes;
	pr_info("\n WRITE: nbytes=%d, pos=%d\n", wbytes, (int)*ppos);
	return wbytes;
}

static const struct file_operations chardrv_fops = {
	.owner   = THIS_MODULE,
	.read    = chardrv_read,
	.write   = chardrv_write,
	.open    = chardrv_open,
	.release = chardrv_release,
};

static int __init chardrv_init(void)
{
	membuff = kmalloc(membuff_size, GFP_KERNEL);
	first = MKDEV(chardev_major, chardev_minor);
	register_chrdev_region(first, count, CHARDEV_NAME);
	chardev_cdev = cdev_alloc();
	cdev_init(chardev_cdev, &chardrv_fops);
	cdev_add(chardev_cdev, first, count);
	pr_info("Sucessfully registered character device: %s\n", CHARDEV_NAME);
	return 0;
}

static void __exit chardrv_exit(void)
{
	cdev_del(chardev_cdev);
	unregister_chrdev_region(first, count);
	pr_info("Successfully unregistered character device:%s\n", CHARDEV_NAME);
	kfree(membuff);
}

module_init(chardrv_init);
module_exit(chardrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Stage 2 character Driver");
MODULE_LICENSE("GPL v2");
