/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Using poll() (character driver)
 *
 * Take the wait_event() solution and extend it to have a poll() entry
 * point.
 *
 * You'll need an application that opens the device node and then
 * calls poll() and waits for data to be available.
 *
 */

#include <linux/module.h>
#include <linux/poll.h>

/* either of these (but not both) will work */
//#include "lab_char.h"
#include "lab_miscdev.h"

static DECLARE_WAIT_QUEUE_HEAD(wq);

static atomic_t data_ready;

static ssize_t
mycdrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	pr_info("process %i (%s) going to sleep\n", current->pid,
		current->comm);
	wait_event_interruptible(wq, (atomic_read(&data_ready)));
	pr_info("process %i (%s) awakening\n", current->pid, current->comm);
	atomic_set(&data_ready, 0);
	return mycdrv_generic_read(file, buf, lbuf, ppos);
}

//if (file->f_flags & O_NDELAY)
//     return -EAGAIN;
static ssize_t
mycdrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int nbytes = mycdrv_generic_write(file, buf, lbuf, ppos);
	pr_info("process %i (%s) awakening the readers...\n",
		current->pid, current->comm);
	wake_up_interruptible(&wq);
	atomic_set(&data_ready, 1);
	return nbytes;
}

static unsigned int mycdrv_poll(struct file *file, poll_table * wait)
{
	poll_wait(file, &wq, wait);
	pr_info("In poll at jiffies=%ld\n", jiffies);
	if (atomic_read(&data_ready))
		return POLLIN | POLLRDNORM;
	return 0;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
	.write = mycdrv_write,
	.open = mycdrv_generic_open,
	.release = mycdrv_generic_release,
	.poll = mycdrv_poll,
};

static int __init my_init(void)
{
	atomic_set(&data_ready, 0);
	return my_generic_init();
}

module_init(my_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap6/lab6_poll.c");
MODULE_LICENSE("GPL v2");
