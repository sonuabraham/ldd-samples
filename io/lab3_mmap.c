/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/*  Memory Mapping an Allocated Region
 *
 * Write a character driver that implements a mmap() entry point that
 * memory maps a kernel buffer, allocated dynamically (probably during
 * intialization).
 *
 * There should also be read() and write() entry points.
 *
 * Optionally, you may want to use an ioctl() command to tell user
 * space the size of the kernel buffer being memory mapped.  An
 * ioctl() command can be used to return to user-space the size of
 * the kernel buffer being memory mapped.
 *
 */


#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>	
#include <linux/slab.h>		
#include <linux/mm.h>
#include <linux/io.h>		/* for virt_to_phys() */



static char *membuff;
#define membuff_size (size_t) (32*PAGE_SIZE)
#define DEVICE_NAME "lddmmap"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddcharDevice = NULL; 


#define MMAP_DEV_CMD_GET_BUFSIZE 1	/* defines our IOCTL cmd */

static int chrdrv_mmap(struct file *filp, struct vm_area_struct *vma)
{
	unsigned long pfn;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long len = vma->vm_end - vma->vm_start;

	if (offset >= membuff_size)
		return -EINVAL;
	if (len > (membuff_size - offset))
		return -EINVAL;
	pr_info("%s: mapping %ld bytes of ramdisk at offset %ld\n",
		__stringify(KBUILD_BASENAME), len, offset);

	/* need to get the pfn for remap_pfn_range -- either of these two
	   following methods will work */

	/*    pfn = page_to_pfn (virt_to_page (membuff + offset)); */
	pfn = virt_to_phys(membuff + offset) >> PAGE_SHIFT;

	if (remap_pfn_range(vma, vma->vm_start, pfn, len, vma->vm_page_prot)) {
		return -EAGAIN;
	}
	return 0;
}

static long
chardrv_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned long tbs = membuff_size;
	void __user *ioargp = (void __user *)arg;

	switch (cmd) {
	default:
		return -EINVAL;

	case MMAP_DEV_CMD_GET_BUFSIZE:
		if (copy_to_user(ioargp, &tbs, sizeof(tbs)))
			return -EFAULT;
		return 0;
	}
}

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

static const struct file_operations fops = {
	.read    = chardrv_read,
	.write   = chardrv_write,
	.unlocked_ioctl = chardrv_unlocked_ioctl,
	.llseek = chardrv_lseek,
	.open    = chardrv_open,
	.release = chardrv_release,
	.mmap    = chrdrv_mmap,
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
MODULE_DESCRIPTION("LDDChar: Character driver mmap");
MODULE_LICENSE("GPL v2");
