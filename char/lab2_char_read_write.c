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



static char *membuff;
#define membuff_size (size_t) (32*PAGE_SIZE)
#define DEVICE_NAME "ldddev"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddcharDevice = NULL; 

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
MODULE_DESCRIPTION("LDDChar: Character driver read,write");
MODULE_LICENSE("GPL v2");


