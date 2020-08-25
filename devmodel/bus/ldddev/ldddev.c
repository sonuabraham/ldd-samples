
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>	/* printk() */
#include <linux/slab.h>		/* kmalloc() */
#include <linux/fs.h>		/* everything... */
#include <linux/errno.h>	/* error codes */
#include <linux/types.h>	/* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h>	/* O_ACCMODE */
#include <linux/aio.h>
#include <asm/uaccess.h>
#include "ldddev.h"		/* local definitions */
#include <lddbus.h>

extern int register_ldd_device(struct ldd_device *);
//extern void unregister_ldd_device(struct ldd_device *);
//extern int register_ldd_driver(struct ldd_driver *);
//extern void unregister_ldd_driver(struct ldd_driver *);

int sculld_major =   SCULLD_MAJOR;
int sculld_devs =    SCULLD_DEVS;	/* number of bare sculld devices */
int sculld_qset =    SCULLD_QSET;
int sculld_order =   SCULLD_ORDER;

module_param(sculld_major, int, 0);
module_param(sculld_devs, int, 0);
module_param(sculld_qset, int, 0);
module_param(sculld_order, int, 0);
MODULE_AUTHOR("Alessandro Rubini");
MODULE_LICENSE("GPL v2");

struct sculld_dev *sculld_devices; /* allocated in sculld_init */

int sculld_trim(struct sculld_dev *dev);
void sculld_cleanup(void);



/* Device model stuff */

static struct ldd_driver sculld_driver = {
	.version = "$Revision: 1.21 $",
	.module = THIS_MODULE,
	.driver = {
		.name = "sculld",
	},
};



/*
 * Open and close
 */

int sculld_open (struct inode *inode, struct file *filp)
{

	return 0;          /* success */
}

int sculld_release (struct inode *inode, struct file *filp)
{
	return 0;
}

struct file_operations sculld_fops = {
	.owner =     THIS_MODULE,
	.open =	     sculld_open,
	.release =   sculld_release,
};

int sculld_trim(struct sculld_dev *dev)
{
	struct sculld_dev *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

	for (dptr = dev; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			/* This code frees a whole quantum-set */
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					free_pages((unsigned long)(dptr->data[i]),
							dptr->order);

			kfree(dptr->data);
			dptr->data=NULL;
		}
		next=dptr->next;
		if (dptr != dev) kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = sculld_qset;
	dev->order = sculld_order;
	dev->next = NULL;
	return 0;
}


static void sculld_setup_cdev(struct sculld_dev *dev, int index)
{
	int err, devno = MKDEV(sculld_major, index);
    
	cdev_init(&dev->cdev, &sculld_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &sculld_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}

static ssize_t sculld_show_dev(struct device *ddev, struct device_attribute *attr, char *buf)
{
	struct sculld_dev *dev = dev_get_drvdata(ddev);

	return print_dev_t(buf, dev->cdev.dev);
}

static DEVICE_ATTR(dev, S_IRUGO, sculld_show_dev, NULL);

static void sculld_register_dev(struct sculld_dev *dev, int index)
{
	sprintf(dev->devname, "sculld%d", index);
	dev->ldev.name = dev->devname;
	dev->ldev.driver = &sculld_driver;
	dev_set_drvdata(&dev->ldev.dev, dev);
	register_ldd_device(&dev->ldev);
	device_create_file(&dev->ldev.dev, &dev_attr_dev);
}


/*
 * Finally, the module stuff
 */

int sculld_init(void)
{
	int result, i;
	dev_t dev = MKDEV(sculld_major, 0);
	
	/*
	 * Register your major, and accept a dynamic number.
	 */
	if (sculld_major)
		result = register_chrdev_region(dev, sculld_devs, "sculld");
	else {
		result = alloc_chrdev_region(&dev, 0, sculld_devs, "sculld");
		sculld_major = MAJOR(dev);
	}
	if (result < 0)
		return result;

	/*
	 * Register with the driver core.
	 */
	register_ldd_driver(&sculld_driver);
	
	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	sculld_devices = kmalloc(sculld_devs*sizeof (struct sculld_dev), GFP_KERNEL);
	if (!sculld_devices) {
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(sculld_devices, 0, sculld_devs*sizeof (struct sculld_dev));
	for (i = 0; i < sculld_devs; i++) {
		sculld_devices[i].order = sculld_order;
		sculld_devices[i].qset = sculld_qset;
			mutex_init(&sculld_devices[i].mutex);
		sculld_setup_cdev(sculld_devices + i, i);
		sculld_register_dev(sculld_devices + i, i);
	}


	return 0; /* succeed */

  fail_malloc:
	unregister_chrdev_region(dev, sculld_devs);
	return result;
}



void sculld_cleanup(void)
{
	int i;

	for (i = 0; i < sculld_devs; i++) {
		unregister_ldd_device(&sculld_devices[i].ldev);
		cdev_del(&sculld_devices[i].cdev);
		sculld_trim(sculld_devices + i);
	}
	kfree(sculld_devices);
	unregister_ldd_driver(&sculld_driver);
	unregister_chrdev_region(MKDEV (sculld_major, 0), sculld_devs);
}


module_init(sculld_init);
module_exit(sculld_cleanup);
