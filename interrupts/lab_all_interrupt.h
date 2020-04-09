/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

#ifndef _LAB_ALL_INTERRUPT_H
#define _LAB_ALL_INTERRUPT_H

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>
#include <linux/kthread.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/miscdevice.h>

#define MYDEV_NAME "mycdrv"
#define ramdisk_size (size_t)(PAGE_SIZE)
//#define MAXIRQS 256
#define MAXIRQS nr_irqs
#define NCOPY (MAXIRQS * sizeof(atomic_t))

static char *ramdisk;
static atomic_t *interrupts, *bhs, *nevents;

/* default delay time in top half -- try 10 to get results */
static int delay = 0;
module_param(delay, int, S_IRUGO);

static irqreturn_t my_interrupt(int irq, void *dev_id);
#ifdef THREADED_IRQ
static irqreturn_t thread_fun(int irq, void *thr_arg);
#endif

static void freeup_irqs(void)
{
	int irq;
	for (irq = 0; irq < MAXIRQS; irq++) {
		pr_info("irq=%d, trying to free!\n", irq);
		/* if greater than 0, was able to share */
		if (atomic_read(&interrupts[irq]) >= 0) {
			pr_info
			    ("Freeing IRQ= %4d, which had %10d, %10d events\n",
			     irq, atomic_read(&interrupts[irq]),
			     atomic_read(&bhs[irq]));
			synchronize_irq(irq);
			free_irq(irq, interrupts);
		}
	}
}

static void get_irqs(void)
{
	int irq;
	interrupts = (atomic_t *) ramdisk;
	bhs = (atomic_t *) (ramdisk + NCOPY);

	for (irq = 0; irq < MAXIRQS; irq++) {
		atomic_set(&interrupts[irq], -1);	/* set to -1 as a flag */
		atomic_set(&bhs[irq], 0);
		atomic_set(&nevents[irq], 0);
#ifdef THREADED_IRQ
		if (!request_threaded_irq(irq, my_interrupt, thread_fun,
					  IRQF_SHARED, "my_int", interrupts))
#else
		if (!request_irq(irq, my_interrupt,
				 IRQF_SHARED, "my_int", interrupts))
#endif
		{
			atomic_set(&interrupts[irq], 0);
			pr_info("Succeded in registering IRQ=%d\n", irq);
		}
	}
}

static ssize_t
mycdrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int nbytes, maxbytes, bytes_to_do;
	maxbytes = ramdisk_size - *ppos;
	bytes_to_do = maxbytes > lbuf ? lbuf : maxbytes;
	if (bytes_to_do == 0)
		pr_warning("Reached end of the device on a read");
	nbytes = bytes_to_do - copy_to_user(buf, ramdisk + *ppos, bytes_to_do);
	*ppos += nbytes;
	pr_info("\n Leaving the   READ function, nbytes=%d, pos=%d\n",
		nbytes, (int)*ppos);
	return nbytes;
}

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.read = mycdrv_read,
};

static struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = MYDEV_NAME,
	.fops = &mycdrv_fops,
};

static int __init my_generic_init(void)
{
	ramdisk = kmalloc(ramdisk_size, GFP_KERNEL);
	if (misc_register(&my_misc_device)) {
		pr_err("Culdn't register device misc, "
		       "%d.\n", my_misc_device.minor);
		return -EBUSY;
	}

	pr_info("\nSucceeded in registering character device %s\n", MYDEV_NAME);

	nevents = kmalloc(MAXIRQS * sizeof(atomic_t), GFP_KERNEL);
	get_irqs();

	return 0;
}

static void __exit my_generic_exit(void)
{
	freeup_irqs();
	misc_deregister(&my_misc_device);
	kfree(ramdisk);
	kfree(nevents);
	pr_info("\ndevice unregistered\n");
}

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap10/lab_all_interrupt.h");
MODULE_LICENSE("GPL v2");

#endif
