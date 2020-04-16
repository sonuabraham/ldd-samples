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
#include <asm/hw_irq.h>

#define SHARED_IRQ 11
static int irq = SHARED_IRQ, wq_dev_id, irq_counter = 0;
module_param(irq, int, S_IRUGO);

#define DEVICE_NAME "wqdev"
#define CLASS_NAME "ldd"

static int majorNumber;   
static struct class*  lddcharClass  = NULL;  
static struct device* lddwqDevice = NULL; 

void ldd_workqueue_function(struct work_struct *work); 
/*Create work queue using static method */
DECLARE_WORK(ldd_workqueue,ldd_workqueue_function);

void ldd_workqueue_function(struct work_struct *work)
{
  pr_info("LDDWorkQueue: Executing workqueue function\n");
}
static irqreturn_t wq_interrupt(int irq, void *dev_id)
{
	irq_counter++;
	pr_info("LDDWorkQueue: Executing Interrupt Service Rourtine(ISR) function counter: %d\n", irq_counter);
  schedule_work(&ldd_workqueue);
	return IRQ_NONE;	
}

static int wq_open(struct inode *inode, struct file *file)
{
	pr_info("LDDWorkQueue: Opening char device: %s\n\n", DEVICE_NAME);
	return 0;
}

static int wq_release(struct inode *inode, struct file *file)
{
	pr_info("LDDWorkQueue: Closing char device: %s\n\n", DEVICE_NAME);
	return 0;
}

static ssize_t wq_read(struct file *file, char __user * buf, size_t lbuf, loff_t * ppos)
{
	struct irq_desc *desc;

  pr_info("LDDWorkQueue: Triggerring s/w interrupt from %s \n",__FUNCTION__);
  desc = irq_to_desc(11);
  if (!desc) return -EINVAL;
  __this_cpu_write(vector_irq[59], desc);
  asm("int $0x3B"); 
  return 0;
}

static ssize_t wq_write(struct file *file, const char __user * buf, size_t lbuf,
	     loff_t * ppos)
{
	pr_info("LDDWorkQueue: WRITE \n");
	return 0;
}

static const struct file_operations fops = {
  .read    = wq_read,
	.write   = wq_write,
	.open    = wq_open,
	.release = wq_release,
};

static int __init wq_init(void)
{
  if (request_irq
	    (irq, wq_interrupt, IRQF_SHARED, "wq_interrupt", &wq_dev_id)) {
	  pr_info("LDDWorkQueue: Failed to reserve irq %d\n", irq);
	  return -1;
	}

  majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
  if (majorNumber<0){
    free_irq(irq, &wq_dev_id);
    pr_err("LDDWorkQueue: Failed to register a major number\n");
    return majorNumber;
  }
  pr_info("LDDWorkQueue: Registered char dev with major number %d\n", majorNumber);

  lddcharClass = class_create(THIS_MODULE, CLASS_NAME);
  if (IS_ERR(lddcharClass)){   
    free_irq(irq, &wq_dev_id);         
    unregister_chrdev(majorNumber, DEVICE_NAME);
    pr_err("LDDWorkQueue: Failed to register device class\n");
    return PTR_ERR(lddcharClass);        
  }
  pr_info("LDDWorkQueue: Device class registered successfully\n");

  lddwqDevice = device_create(lddcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
  if (IS_ERR(lddwqDevice)){
    free_irq(irq, &wq_dev_id);                 
    class_destroy(lddcharClass);           
    unregister_chrdev(majorNumber, DEVICE_NAME);
    pr_err("LDDWorkQueue: Failed to create char device\n");
    return PTR_ERR(lddwqDevice);
  }
  pr_info("LDDWorkQueue: Successfully loading ISR handler\n");
	
  return 0;
}

static void __exit wq_exit(void)
{
  synchronize_irq(irq);
	free_irq(irq, &wq_dev_id);
  device_destroy(lddcharClass, MKDEV(majorNumber, 0));     
  class_unregister(lddcharClass);                         
  class_destroy(lddcharClass);                             
  unregister_chrdev(majorNumber, DEVICE_NAME);       
	pr_info("LDDWorkQueue: Successfully unloading,  irq_counter = %d\n", irq_counter);
}

module_init(wq_init);
module_exit(wq_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate workqueue");
MODULE_LICENSE("GPL v2");
