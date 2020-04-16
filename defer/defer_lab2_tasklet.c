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
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "taskletdev"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddtaskletDevice = NULL; 

static struct ldd_tasklet_data {
	unsigned long data;
}ldd_tdata = {
.data = 0 };

static void ldd_tasklet_function(unsigned long t_arg)
{
	struct ldd_tasklet_data *tdata;
	tdata = (struct ldd_tasklet_data *)t_arg;

  pr_info("LDDTasklet: Invoked tasklet at jiffies: %lu\n", jiffies);
	pr_info("LDDTasklet: Current task pid: %d\n", (int)current->pid);
	pr_info("LDDTasklet: Created tasklet at jiffies: %lu\n", tdata->data);
}

/* initialize tasklet */
static DECLARE_TASKLET(ldd_tasklet, ldd_tasklet_function, (unsigned long)&ldd_tdata);

static int tsklet_open(struct inode *inode, struct file *file)
{
	pr_info("LDDTasklet: Opening char device: %s\n\n", DEVICE_NAME);
  return 0;
}

static int tsklet_release(struct inode *inode, struct file *file)
{
	pr_info("LDDTasklet: Closing char device: %s\n\n", DEVICE_NAME);
	return 0;
}

static ssize_t tsklet_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	return 0;
}

static ssize_t tsklet_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
  pr_info("LDDTasklet: Queuing tasklet from %s\n", __FUNCTION__);
  ldd_tdata.data = jiffies;
	pr_info("LDDTasklet: Tasklet queued at jiffies: %lu\n", jiffies);
  tasklet_schedule(&ldd_tasklet);
	return lbuf;
}
static const struct file_operations fops = {
  .read    = tsklet_read,
	.write   = tsklet_write,
  .open    = tsklet_open,
	.release = tsklet_release,
};

static int __init tsklet_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
      pr_err("LDDTasklet: Failed to register a major number\n");
      return majorNumber;
    }
    pr_info("LDDTasklet: Registered char dev with major number %d\n", majorNumber);

    lddcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(lddcharClass)){   
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDTasklet: Failed to register device class\n");
      return PTR_ERR(lddcharClass);        
    }
    pr_info("LDDTasklet: Device class registered successfully\n");

    lddtaskletDevice = device_create(lddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(lddtaskletDevice)){
      class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDTasklet: Failed to create char device\n");
      return PTR_ERR(lddtaskletDevice);
    }
    return 0;
}

static void __exit tsklet_exit(void)
{
  device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
  class_unregister(lddcharClass);                         
  class_destroy(lddcharClass);                             
  unregister_chrdev(majorNumber, DEVICE_NAME); 
  pr_info("LDDTasklet: Unloaded char device\n");      
}

module_init(tsklet_init);
module_exit(tsklet_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate tasklets");
MODULE_LICENSE("GPL v2");

