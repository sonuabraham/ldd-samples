

/* -*- C -*-
 * main.c -- the bare scullc char module
 *
 * Copyright (C) 2001 Alessandro Rubini and Jonathan Corbet
 * Copyright (C) 2001 O'Reilly & Associates
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files.  The citation
 * should list that the code comes from the book "Linux Device
 * Drivers" by Alessandro Rubini and Jonathan Corbet, published
 * by O'Reilly & Associates.   No warranty is attached;
 * we cannot take responsibility for errors or fitness for use.
 *
 * $Id: _main.c.in,v 1.21 2004/10/14 20:11:39 corbet Exp $
 */

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
#include <linux/uio.h>
#include <linux/seq_file.h>
#include <asm/uaccess.h>
#include "scullc.h"		/* local definitions */
 #include <linux/uaccess.h>

int scullc_major =   SCULLC_MAJOR;
int scullc_devs =    SCULLC_DEVS;	/* number of bare scullc devices */
int scullc_qset =    SCULLC_QSET;
int scullc_quantum = SCULLC_QUANTUM;

module_param(scullc_major, int, 0);
module_param(scullc_devs, int, 0);
module_param(scullc_qset, int, 0);
module_param(scullc_quantum, int, 0);
MODULE_AUTHOR("Alessandro Rubini");
MODULE_LICENSE("Dual BSD/GPL");

struct scullc_dev *scullc_devices; /* allocated in scullc_init */

int scullc_trim(struct scullc_dev *dev);
void scullc_cleanup(void);

/* declare one cache pointer: use it for all devices */
struct kmem_cache *scullc_cache;





#ifdef SCULLC_USE_PROC /* don't waste space if unused */
/*
 * The proc filesystem: function to read and entry
 */

static int scullc_read_mem_proc_show(struct seq_file *m, void *v)
{
	int i, j, quantum, qset;
	int limit = m->size - 80; /* Don't print more than this */
	struct scullc_dev *d;

	for(i = 0; i < scullc_devs; i++) {
		d = &scullc_devices[i];
		if (mutex_lock_interruptible(&d->mutex))
			return -ERESTARTSYS;
		qset = d->qset;  /* retrieve the features of each device */
		quantum=d->quantum;
		seq_printf(m,"\nDevice %i: qset %i, quantum %i, sz %li\n",
				i, qset, quantum, (long)(d->size));
		for (; d; d = d->next) { /* scan the list */
			seq_printf(m,"  item at %p, qset at %p\n",d,d->data);
			if (m->count > limit)
				goto out;
			if (d->data && !d->next) /* dump only the last item - save space */
				for (j = 0; j < qset; j++) {
					if (d->data[j])
						seq_printf(m,"    % 4i:%8p\n",j,d->data[j]);
					if (m->count > limit)
						goto out;
				}
		}
	  out:
		mutex_unlock(&scullc_devices[i].mutex);
		if (m->count > limit)
			break;
	}
	return 0;
}

#define DEFINE_PROC_SEQ_FILE(_name) \
	static int _name##_proc_open(struct inode *inode, struct file *file)\
	{\
		return single_open(file, _name##_proc_show, NULL);\
	}\
	\
	static const struct file_operations _name##_proc_fops = {\
		.open		= _name##_proc_open,\
		.read		= seq_read,\
		.llseek		= seq_lseek,\
		.release	= single_release,\
	};

DEFINE_PROC_SEQ_FILE(scullc_read_mem)
#endif /* SCULLC_USE_PROC */

/*
 * Open and close
 */

int scullc_open (struct inode *inode, struct file *filp)
{
	struct scullc_dev *dev; /* device information */

	/*  Find the device */
	dev = container_of(inode->i_cdev, struct scullc_dev, cdev);

    	/* now trim to 0 the length of the device if open was write-only */
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY) {
		if (mutex_lock_interruptible(&dev->mutex))
			return -ERESTARTSYS;
		scullc_trim(dev); /* ignore errors */
		mutex_unlock(&dev->mutex);
	}

	/* and use filp->private_data to point to the device data */
	filp->private_data = dev;

	return 0;          /* success */
}

int scullc_release (struct inode *inode, struct file *filp)
{
	return 0;
}

/*
 * Follow the list 
 */
struct scullc_dev *scullc_follow(struct scullc_dev *dev, int n)
{
	while (n--) {
		if (!dev->next) {
			dev->next = kmalloc(sizeof(struct scullc_dev), GFP_KERNEL);
			memset(dev->next, 0, sizeof(struct scullc_dev));
		}
		dev = dev->next;
		continue;
	}
	return dev;
}

/*
 * Data management: read and write
 */

ssize_t scullc_do_read (struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos, int aio)
{
	struct scullc_dev *dev = filp->private_data; /* the first listitem */
	struct scullc_dev *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset; /* how many bytes in the listitem */
	int item, s_pos, q_pos, rest;
	ssize_t retval = 0;

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;
	if (*f_pos > dev->size) 
		goto nothing;
	if (*f_pos + count > dev->size)
		count = dev->size - *f_pos;
	/* find listitem, qset index, and offset in the quantum */
	item = ((long) *f_pos) / itemsize;
	rest = ((long) *f_pos) % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

    	/* follow the list up to the right position (defined elsewhere) */
	dptr = scullc_follow(dev, item);

	if (!dptr->data)
		goto nothing; /* don't fill holes */
	if (!dptr->data[s_pos])
		goto nothing;
	if (count > quantum - q_pos)
		count = quantum - q_pos; /* read only up to the end of this quantum */

	if (aio) {
		if (memcpy (buf, dptr->data[s_pos]+q_pos, count)) {
			retval = -EFAULT;
			goto nothing;
		}
	} else {
		if (copy_to_user (buf, dptr->data[s_pos]+q_pos, count)) {
			retval = -EFAULT;
			goto nothing;
		}
	}
	mutex_unlock(&dev->mutex);

	*f_pos += count;
	return count;

  nothing:
	mutex_unlock(&dev->mutex);
	return retval;
}

ssize_t scullc_read (struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
	return scullc_do_read(filp, buf, count, f_pos, 0);
}


ssize_t scullc_do_write (struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos, int aio)
{
	struct scullc_dev *dev = filp->private_data;
	struct scullc_dev *dptr;
	int quantum = dev->quantum;
	int qset = dev->qset;
	int itemsize = quantum * qset;
	int item, s_pos, q_pos, rest;
	ssize_t retval = -ENOMEM; /* our most likely error */

	if (mutex_lock_interruptible(&dev->mutex))
		return -ERESTARTSYS;

	/* find listitem, qset index and offset in the quantum */
	item = ((long) *f_pos) / itemsize;
	rest = ((long) *f_pos) % itemsize;
	s_pos = rest / quantum; q_pos = rest % quantum;

	/* follow the list up to the right position */
	dptr = scullc_follow(dev, item);
	if (!dptr->data) {
		dptr->data = kmalloc(qset * sizeof(void *), GFP_KERNEL);
		if (!dptr->data)
			goto nomem;
		memset(dptr->data, 0, qset * sizeof(char *));
	}
	/* Allocate a quantum using the memory cache */
	if (!dptr->data[s_pos]) {
		dptr->data[s_pos] = kmem_cache_alloc(scullc_cache, GFP_KERNEL);
		if (!dptr->data[s_pos])
			goto nomem;
		memset(dptr->data[s_pos], 0, scullc_quantum);
	}
	if (count > quantum - q_pos)
		count = quantum - q_pos; /* write only up to the end of this quantum */
	if (aio) {
		if (memcpy (dptr->data[s_pos]+q_pos, buf, count)) {
			retval = -EFAULT;
			goto nomem;
		}
	} else {
		if (copy_from_user (dptr->data[s_pos]+q_pos, buf, count)) {
			retval = -EFAULT;
			goto nomem;
		}
	}
	*f_pos += count;
 
    	/* update the size */
	if (dev->size < *f_pos)
		dev->size = *f_pos;
	mutex_unlock(&dev->mutex);
	return count;

  nomem:
	mutex_unlock(&dev->mutex);
	return retval;
}


ssize_t scullc_write (struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
	return scullc_do_write(filp, buf, count, f_pos, 0);
}

/*
 * The ioctl() implementation
 */

long scullc_ioctl (struct file *filp,
                 unsigned int cmd, unsigned long arg)
{

	int err = 0, ret = 0, tmp;

	/* don't even decode wrong cmds: better returning  ENOTTY than EFAULT */
	if (_IOC_TYPE(cmd) != SCULLC_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULLC_IOC_MAXNR) return -ENOTTY;

	/*
	 * the type is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. Note that the type is user-oriented, while
	 * verify_area is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	#if 0
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err)
		return -EFAULT;
	#endif 
	
	switch(cmd) {

	case SCULLC_IOCRESET:
		scullc_qset = SCULLC_QSET;
		scullc_quantum = SCULLC_QUANTUM;
		break;

	case SCULLC_IOCSQUANTUM: /* Set: arg points to the value */
		ret = __get_user(scullc_quantum, (int __user *) arg);
		break;

	case SCULLC_IOCTQUANTUM: /* Tell: arg is the value */
		scullc_quantum = arg;
		break;

	case SCULLC_IOCGQUANTUM: /* Get: arg is pointer to result */
		ret = __put_user (scullc_quantum, (int __user *) arg);
		break;

	case SCULLC_IOCQQUANTUM: /* Query: return it (it's positive) */
		return scullc_quantum;

	case SCULLC_IOCXQUANTUM: /* eXchange: use arg as pointer */
		tmp = scullc_quantum;
		ret = __get_user(scullc_quantum, (int __user *) arg);
		if (ret == 0)
			ret = __put_user(tmp, (int __user *) arg);
		break;

	case SCULLC_IOCHQUANTUM: /* sHift: like Tell + Query */
		tmp = scullc_quantum;
		scullc_quantum = arg;
		return tmp;

	case SCULLC_IOCSQSET:
		ret = __get_user(scullc_qset, (int __user *) arg);
		break;

	case SCULLC_IOCTQSET:
		scullc_qset = arg;
		break;

	case SCULLC_IOCGQSET:
		ret = __put_user(scullc_qset, (int __user *)arg);
		break;

	case SCULLC_IOCQQSET:
		return scullc_qset;

	case SCULLC_IOCXQSET:
		tmp = scullc_qset;
		ret = __get_user(scullc_qset, (int __user *)arg);
		if (ret == 0)
			ret = __put_user(tmp, (int __user *)arg);
		break;

	case SCULLC_IOCHQSET:
		tmp = scullc_qset;
		scullc_qset = arg;
		return tmp;

	default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}

	return ret;
}

/*
 * The "extended" operations
 */

loff_t scullc_llseek (struct file *filp, loff_t off, int whence)
{
	struct scullc_dev *dev = filp->private_data;
	long newpos;

	switch(whence) {
	case 0: /* SEEK_SET */
		newpos = off;
		break;

	case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	case 2: /* SEEK_END */
		newpos = dev->size + off;
		break;

	default: /* can't happen */
		return -EINVAL;
	}
	if (newpos<0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}


/*
 * A simple asynchronous I/O implementation.
 */

struct async_work {
	struct kiocb *iocb;
	int result;
	struct delayed_work work;
};

/*
 * "Complete" an asynchronous operation.
 */
static void scullc_do_deferred_op(struct work_struct *p)
{
	struct async_work *stuff = container_of(p, struct async_work, work.work);
	stuff->iocb->ki_complete(stuff->iocb, stuff->result, 0);
	kfree(stuff);
}


static int scullc_defer_op(int write, struct kiocb *iocb, struct iov_iter *iter)
{
	struct async_work *stuff;
	char			*buf;
	size_t total;
	int result;
	loff_t *ppos;

	total = iov_iter_count(iter);
	ppos = &iocb->ki_pos;

	buf = kmalloc(total, GFP_KERNEL);
	if (unlikely(!buf))
		return -ENOMEM;


	/* Copy now while we can access the buffer */
	if (write)
	{
		result = copy_from_iter(buf, total, iter);

		result = scullc_do_write(iocb->ki_filp, buf, result, ppos, 1);
	}
	else
	{
		result = scullc_do_read(iocb->ki_filp, buf, total, ppos, 1);

		result = copy_to_iter(buf , result, iter);
	}

	kfree(buf);

	/* If this is a synchronous IOCB, we return our status now. */
	if (is_sync_kiocb(iocb))
		return result;

	/* Otherwise defer the completion for a few milliseconds. */
	stuff = kmalloc (sizeof (*stuff), GFP_KERNEL);
	if (stuff == NULL)
		return result; /* No memory, just complete now */
	stuff->iocb = iocb;
	stuff->result = result;
	INIT_DELAYED_WORK(&stuff->work, scullc_do_deferred_op);
	schedule_delayed_work(&stuff->work, HZ/100);
	return -EIOCBQUEUED;
}


static ssize_t scullc_read_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	return scullc_defer_op(0, iocb, iter);
}

static ssize_t scullc_write_iter(struct kiocb *iocb, struct iov_iter *iter)
{
	return scullc_defer_op(1, iocb, iter);
}


 

/*
 * The fops
 */

struct file_operations scullc_fops = {
	.owner =     THIS_MODULE,
	.llseek =    scullc_llseek,
//	.read =	     scullc_read,
//	.write =     scullc_write,
	.unlocked_ioctl =     scullc_ioctl,
	.open =	     scullc_open,
	.release =   scullc_release,
	.read_iter =  scullc_read_iter,
	.write_iter = scullc_write_iter,
};

int scullc_trim(struct scullc_dev *dev)
{
	struct scullc_dev *next, *dptr;
	int qset = dev->qset;   /* "dev" is not-null */
	int i;

	if (dev->vmas) /* don't trim: there are active mappings */
		return -EBUSY;

	for (dptr = dev; dptr; dptr = next) { /* all the list items */
		if (dptr->data) {
			for (i = 0; i < qset; i++)
				if (dptr->data[i])
					kmem_cache_free(scullc_cache, dptr->data[i]);

			kfree(dptr->data);
			dptr->data=NULL;
		}
		next=dptr->next;
		if (dptr != dev) kfree(dptr); /* all of them but the first */
	}
	dev->size = 0;
	dev->qset = scullc_qset;
	dev->quantum = scullc_quantum;
	dev->next = NULL;
	return 0;
}


static void scullc_setup_cdev(struct scullc_dev *dev, int index)
{
	int err, devno = MKDEV(scullc_major, index);
    
	cdev_init(&dev->cdev, &scullc_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scullc_fops;
	err = cdev_add (&dev->cdev, devno, 1);
	/* Fail gracefully if need be */
	if (err)
		printk(KERN_NOTICE "Error %d adding scull%d", err, index);
}



/*
 * Finally, the module stuff
 */

int scullc_init(void)
{
	int result, i;
	dev_t dev = MKDEV(scullc_major, 0);
	
	/*
	 * Register your major, and accept a dynamic number.
	 */
	if (scullc_major)
		result = register_chrdev_region(dev, scullc_devs, "scullc");
	else {
		result = alloc_chrdev_region(&dev, 0, scullc_devs, "scullc");
		scullc_major = MAJOR(dev);
	}
	if (result < 0)
		return result;

	
	/* 
	 * allocate the devices -- we can't have them static, as the number
	 * can be specified at load time
	 */
	scullc_devices = kmalloc(scullc_devs*sizeof (struct scullc_dev), GFP_KERNEL);
	if (!scullc_devices) {
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(scullc_devices, 0, scullc_devs*sizeof (struct scullc_dev));
	for (i = 0; i < scullc_devs; i++) {
		scullc_devices[i].quantum = scullc_quantum;
		scullc_devices[i].qset = scullc_qset;
		mutex_init (&scullc_devices[i].mutex);
		scullc_setup_cdev(scullc_devices + i, i);
	}

	scullc_cache = kmem_cache_create("scullc", scullc_quantum,
			0, SLAB_HWCACHE_ALIGN, NULL); /* no ctor */
	if (!scullc_cache) {
		scullc_cleanup();
		return -ENOMEM;
	}

#ifdef SCULLC_USE_PROC /* only when available */
	proc_create("scullcmem", 0, NULL, &scullc_read_mem_proc_fops);
#endif
	return 0; /* succeed */

  fail_malloc:
	unregister_chrdev_region(dev, scullc_devs);
	return result;
}



void scullc_cleanup(void)
{
	int i;

#ifdef SCULLC_USE_PROC
	remove_proc_entry("scullcmem", NULL);
#endif

	for (i = 0; i < scullc_devs; i++) {
		cdev_del(&scullc_devices[i].cdev);
		scullc_trim(scullc_devices + i);
	}
	kfree(scullc_devices);

	if (scullc_cache)
		kmem_cache_destroy(scullc_cache);
	unregister_chrdev_region(MKDEV (scullc_major, 0), scullc_devs);
}


module_init(scullc_init);
module_exit(scullc_cleanup);
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
 * Adding Asynchronous Entry Points to a Character Driver
 *
 *   (kernel driver)
 *
 * Take one of your earlier character drivers and add new entry points
 * for aio_read() and aio_write().
 *
 * To test this you'll need to write a user-space program that uses
 * the native Linux API.  Have it send out a number of write and read
 * requests and synchronize properly.
 *
 * We also present a solution using the Posix API for the user
 * application; note that this will never hit your driver unless you
 * comment out the normal read and write entry points in which case
 * the kernel will fall back on the asynchronous ones.
 *
 * Make sure you compile by linking with the right libraries; use
 * -laio for the Linux API and -lrt for the Posix API.  (You can use
 * both in either case as they don't conflict.)'
 */


#if 0

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>	
#include <linux/slab.h>	
#include <linux/aio.h>
#include <linux/uio.h>	/* iov_iter* */		


static char *membuff;
#define membuff_size (size_t) (32*PAGE_SIZE)
#define DEVICE_NAME "lddaio"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddcharDevice = NULL; 

/*
 * A simple asynchronous I/O implementation.
 */

struct async_work {
	struct delayed_work work;
	struct kiocb *iocb;
	struct iov_iter *tofrom;
};


static int chardrv_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	pr_info("LDDChar: Opening char device: %s\n\n", DEVICE_NAME);
	pr_info("LDDChar: MAJOR number = %d, MINOR number = %d\n",
		imajor(inode), iminor(inode));
	counter++;

	pr_info("LDDChar: Device opened  %d times since loaded\n", counter);
	pr_info("LDDChar: Ref=%d\n", (int)module_refcount(THIS_MODULE));

	return 0;
}

static int chardrv_release(struct inode *inode, struct file *file)
{
	pr_info("LDDChar: Closing char device: %s\n\n", DEVICE_NAME);
	return 0;
}

static ssize_t chardrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int rbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("LDDChar: Cannot read beyond end of device..\n");
		return 0;
	}
	rbytes = lbuf - copy_to_user(buf, membuff + *ppos, lbuf);
	*ppos += rbytes;
	pr_info("LDDChar: READ rbytes=%d, pos=%d\n", rbytes, (int)*ppos);
	return rbytes;
}

static ssize_t chardrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int wbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("LDDChar: Cannot write beyond end of device..\n");
		return 0;
	}
	wbytes = lbuf - copy_from_user(membuff + *ppos, buf, lbuf);
	*ppos += wbytes;
	pr_info("LDDChar: WRITE wbytes=%d, pos=%d\n", wbytes, (int)*ppos);
	return wbytes;
}

static loff_t chardrv_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t pos;
	switch (orig) {
	case SEEK_SET:
		pos = offset;
		break;
	case SEEK_CUR:
		pos = file->f_pos + offset;
		break;
	case SEEK_END:
		pos = membuff_size + offset;
		break;
	default:
		return -EINVAL;
	}
	pos = pos < membuff_size ? pos : membuff_size;
	pos = pos >= 0 ? pos : 0;
	file->f_pos = pos;
	pr_info("LDDCchar: Seeking to position=%ld\n", (long)pos);
	return pos;
}
/*
 * "Complete" an asynchronous operation.
 */
static void chardrv_do_deferred_op(struct work_struct *work)
{
	struct async_work *stuff = container_of(work, struct async_work, work.work);
	if( iov_iter_rw(stuff->tofrom) == WRITE ) {
		generic_file_write_iter(stuff->iocb, stuff->tofrom);
	} else {
		generic_file_read_iter(stuff->iocb, stuff->tofrom);
	}
	kfree(stuff);
}


static int chardrv_defer_op(struct kiocb *iocb, struct iov_iter *tofrom)
{
	struct async_work *stuff;
	int result;
	/* Otherwise defer the completion for a few milliseconds. */
	stuff = kmalloc (sizeof (*stuff), GFP_KERNEL);
	if (stuff == NULL)
		return result; /* No memory, just complete now */
	stuff->iocb = iocb;
	INIT_DELAYED_WORK(&stuff->work, chardrv_do_deferred_op);
	schedule_delayed_work(&stuff->work, HZ/100);
	return -EIOCBQUEUED;
}


ssize_t chardrv_read_iter(struct kiocb *iocb, struct iov_iter *to)
{
	/* If this is a synchronous IOCB, we return our status now. */
	if (is_sync_kiocb(iocb)) {
		return generic_file_read_iter(iocb,to); 
	}
	return chardrv_defer_op(iocb, to);
}

ssize_t chardrv_write_iter(struct kiocb *iocb, struct iov_iter *from)
{
	if (is_sync_kiocb(iocb)) {
		return generic_file_write_iter(iocb,from);
	}
	return chardrv_defer_op(iocb, from); 
}


static const struct file_operations fops = {
	.read    = chardrv_read,
	.write   = chardrv_write,
	.llseek = chardrv_lseek,
	.read_iter = chardrv_read_iter,
	.write_iter = chardrv_write_iter,
	.open    = chardrv_open,
	.release = chardrv_release,
};

static int __init chardrv_init(void)
{
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      pr_err("LDDChar: Failed to register a major number\n");
      return majorNumber;
   }
   pr_info("LDDChar: Registered char dev with major number %d\n", majorNumber);

   lddcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(lddcharClass)){            
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDChar: Failed to register device class\n");
      return PTR_ERR(lddcharClass);        
   }
   printk(KERN_INFO "LDDChar: Device class registered successfully\n");

   lddcharDevice = device_create(lddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(lddcharDevice)){               
      class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDChar: Failed to create char device\n");
      return PTR_ERR(lddcharDevice);
   }

   membuff = kmalloc(membuff_size, GFP_KERNEL);
   if(membuff == NULL){
	  device_destroy(lddcharClass, MKDEV(majorNumber, 0));  
	  class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDChar: Failed to allocate memory for char device\n");
      return -1;
   }
   printk(KERN_INFO "LDDChar: Device registered successfully\n");

   return 0;
}

static void __exit chardrv_exit(void)
{
	kfree(membuff);
    device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
    class_unregister(lddcharClass);                         
    class_destroy(lddcharClass);                             
    unregister_chrdev(majorNumber, DEVICE_NAME);           
    pr_info("LDDChar: Device unregistered successfully\n");
}

module_init(chardrv_init);
module_exit(chardrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDDChar: Character driver aio");
MODULE_LICENSE("GPL v2");
#endif
