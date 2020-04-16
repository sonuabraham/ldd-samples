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
#include <linux/slab.h>
#include <linux/timer.h>

#define DEVICE_NAME "timerdev"
#define CLASS_NAME "ldd"
#define from_timer(var, callback_timer, timer_fieldname) \
    container_of(callback_timer, typeof(*var), timer_fieldname) 

static int majorNumber;   
static struct class*  lddcharClass  = NULL; 
static struct device* lddtimerDevice = NULL; 

struct ldd_timer_data{
  struct timer_list timer;
  unsigned long data;
};

static struct ldd_timer_data *tmd;

static void ldd_timer_function(struct timer_list *t)
{
  unsigned long data = 0;
  struct ldd_timer_data *tmd = from_timer(tmd, t, timer);
  data = tmd -> data;

	pr_info("LDDTimer: Invoked timer at jiffies: %lu\n", jiffies);
	pr_info("LDDTimer: Current task pid: %d\n", (int)current->pid);
	pr_info("LDDTimer: Created timer at jiffies: %lu\n", data);
}


static int timer_open(struct inode *inode, struct file *file)
{
	pr_info("LDDTimer: Opening char device: %s\n\n", DEVICE_NAME);
  return 0;
}

static int timer_release(struct inode *inode, struct file *file)
{
	pr_info("LDDTimer: Closing char device: %s\n\n", DEVICE_NAME);
	return 0;
}
static const struct file_operations fops = {
  .open    = timer_open,
	.release = timer_release,
};

static int __init timer_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber<0){
      pr_err("LDDTimer: Failed to register a major number\n");
      return majorNumber;
    }
    pr_info("LDDTimer: Registered char dev with major number %d\n", majorNumber);

    lddcharClass = class_create(THIS_MODULE, CLASS_NAME);
    if (IS_ERR(lddcharClass)){   
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDTimer: Failed to register device class\n");
      return PTR_ERR(lddcharClass);        
    }
    pr_info("LDDTimer: Device class registered successfully\n");

    lddtimerDevice = device_create(lddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(lddtimerDevice)){
      class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDTimer: Failed to create char device\n");
      return PTR_ERR(lddtimerDevice);
    }

    tmd = kmalloc(sizeof(*tmd), GFP_KERNEL);
    if (!tmd)
    {
      device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
      class_destroy(lddcharClass);           
      unregister_chrdev(majorNumber, DEVICE_NAME);
      pr_err("LDDTimer: Failed to allocate memory for timer data\n");
      return -ENOMEM;
    }
    
    tmd->data = (unsigned long) jiffies;
    timer_setup(&tmd->timer, ldd_timer_function, 0);
    tmd->timer.expires = jiffies + HZ;	/*set time to expire after one second delay */
    add_timer(&tmd->timer);
    pr_info("LDDTimer: Added timer at jiffies: %lu\n", jiffies);
	  return 0;
}

static void __exit timer_exit(void)
{
  del_timer(&tmd->timer);
  kfree(tmd);
  device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
  class_unregister(lddcharClass);                         
  class_destroy(lddcharClass);                             
  unregister_chrdev(majorNumber, DEVICE_NAME); 
  pr_info("LDDTimer: Unloaded char device\n");      
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate kernel timer");
MODULE_LICENSE("GPL v2");
