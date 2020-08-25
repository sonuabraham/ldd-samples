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
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>	
#include <linux/slab.h>		


static int size = PAGE_SIZE;
static struct kmem_cache *ldd_cache;
module_param(size, int, S_IRUGO);
static char *membuff;
unsigned int membuff_size;

#define DEVICE_NAME "cachedev"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddcacheDevice = NULL; 

static int chardrv_open(struct inode *inode, struct file *file)
{
	/* allocate a memory cache object */

	if (!(membuff = kmem_cache_alloc(ldd_cache, GFP_ATOMIC))) {
		pr_err("LDDCache: Failed to create cache object\n");
		return -ENOMEM;
	}
	pr_info("LDDCache: Successfully created cache object\n");

	return 0;
}

static int chardrv_release(struct inode *inode, struct file *file)
{
	pr_info("LDDCache: Closing char device: %s\n\n", DEVICE_NAME);
		/* destroy a memory cache object */
	kmem_cache_free(ldd_cache, membuff);
	return 0;
}

static ssize_t chardrv_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	int rbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("LDDCache: Cannot read beyond end of device..\n");
		return 0;
	}
	rbytes = lbuf - copy_to_user(buf, membuff + *ppos, lbuf);
	*ppos += rbytes;
	pr_info("LDDCache: READ rbytes=%d, pos=%d\n", rbytes, (int)*ppos);
	return rbytes;
}

static ssize_t chardrv_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	int wbytes;
	if ((lbuf + *ppos) > membuff_size) {
		pr_info("LDDCache: Cannot write beyond end of device..\n");
		return 0;
	}
	wbytes = lbuf - copy_from_user(membuff + *ppos, buf, lbuf);
	*ppos += wbytes;
	pr_info("LDDCache: WRITE wbytes=%d, pos=%d\n", wbytes, (int)*ppos);
	return wbytes;
}

static const struct file_operations fops = {
	.read    = chardrv_read,
	.write   = chardrv_write,
	.open    = chardrv_open,
	.release = chardrv_release,
};

static int __init chardrv_init(void)
{
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber<0){
      pr_err("LDDCache: Failed to register a major number\n");
      return majorNumber;
   }
   pr_info("LDDCache: Registered char dev with major number %d\n", majorNumber);

   lddcharClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(lddcharClass)){            
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDCache: Failed to register device class\n");
      return PTR_ERR(lddcharClass);        
   }
   printk(KERN_INFO "LDDCache: Device class registered successfully\n");

   lddcacheDevice = device_create(lddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(lddcacheDevice)){               
      class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDCache: Failed to create char device\n");
      return PTR_ERR(lddcacheDevice);
   }

   	/* create a memory cache */
	if (size > (1024 * PAGE_SIZE)) {
		pr_info
		    ("LDDCache: size=%d is too large; can't have more than 1024 pages!\n",
		     size);
	    device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
	    class_destroy(lddcharClass);           
        unregister_chrdev(majorNumber, DEVICE_NAME);
		return -1;
	}

	if (!(ldd_cache = kmem_cache_create("lddcache", size, 0,
					   SLAB_HWCACHE_ALIGN, NULL))) {
		pr_err("LDDCache: kmem_cache_create failed\n");
		device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
	    class_destroy(lddcharClass);           
        unregister_chrdev(majorNumber, DEVICE_NAME);
		return -ENOMEM;
	}
	pr_info("LDDCache: Allocated memory cache successfully\n");
	membuff_size = size;
   printk(KERN_INFO "LDDCache: Device registered successfully\n");

   return 0;
}

static void __exit chardrv_exit(void)
{
    device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
    class_unregister(lddcharClass);                         
    class_destroy(lddcharClass);                             
    unregister_chrdev(majorNumber, DEVICE_NAME);  
	(void)kmem_cache_destroy(ldd_cache);         
    pr_info("LDDCache: Device unregistered successfully\n");
}

module_init(chardrv_init);
module_exit(chardrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Character driver using cache for memory allocation");
MODULE_LICENSE("GPL v2");
