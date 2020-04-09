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
 * Using ioctl's to pass data of variable length.
 *
 * Extend the previous exercise to send a buffer whose length is
 * determined at run time.  You will probably need to use the _IOC
 * macro directly in the user-space program.  (See linux/ioctl.h.)
 */

#include <linux/module.h>

/* either of these (but not both) will work */
//#include "lab_char.h"
#include "lab_miscdev.h"

#define MYIOC_TYPE 'k'

static inline long
mycdrv_unlocked_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
	int i, rc, direction;
	int size;
	char *buffer;
	void __user *ioargp = (void __user *)arg;

	/* make sure it is a valid command */

	if (_IOC_TYPE(cmd) != MYIOC_TYPE) {
		pr_warning(" got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}

	/* get the size of the buffer and kmalloc it */

	size = _IOC_SIZE(cmd);
	buffer = kmalloc((size_t) size, GFP_KERNEL);
	if (!buffer) {
		pr_err("Kmalloc failed for buffer\n");
		return -ENOMEM;
	}

	/* fill it with X */

	memset(buffer, 'X', size);

	direction = _IOC_DIR(cmd);

	switch (direction) {

	case _IOC_WRITE:
		pr_info(
		     " reading = %d bytes from user-space and writing to device\n",
		     size);
		rc = copy_from_user(buffer, ioargp, size);
		pr_info("rc from copy_from_user = %d\n", rc);
		break;

	case _IOC_READ:
		pr_info(
		       " reading device and writing = %d bytes to user-space\n",
		       size);
		rc = copy_to_user(ioargp, buffer, size);
		pr_info("rc from copy_to_user = %d\n", rc);
		break;

	default:
		pr_warning(" got invalid case, CMD=%d\n", cmd);
		return -EINVAL;
	}
	for (i = 0; i < size; i++)
		pr_info("%c", buffer[i]);
	pr_info("\n");

	if (buffer)
		kfree(buffer);
	return rc;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = mycdrv_unlocked_ioctl,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap6/lab2_ioctl_vardata.c");
MODULE_LICENSE("GPL v2");
