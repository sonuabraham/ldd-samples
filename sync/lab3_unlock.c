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
 * Mutex Unlocking from an Interrupt.
 *
 * Modify the simple interrupt sharing lab to have a mutex taken out
 * and then released in the interrupt handler.
 *
 * This is supposed to be illegal.  Is this ignored, enforced, or
 * warned against?  Why?
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/interrupt.h>

static DEFINE_MUTEX(my_mutex);
#define SHARED_IRQ 19
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param(irq, int, S_IRUGO);

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	irq_counter++;
	mutex_lock(&my_mutex);
	//pr_info("\nInit mutex in locked state, count=%d:\n",
	//	atomic_read(&my_mutex.count));
	pr_info("In the ISR: counter = %d\n", irq_counter);
	mutex_unlock(&my_mutex);
	return IRQ_NONE;	/* we return IRQ_NONE because we are just observing */
}

static int __init my_init(void)
{
	if (request_irq
	    (irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id))
		return -1;
	pr_info("Successfully loading ISR handler\n");

	return 0;
}

static void __exit my_exit(void)
{
	//pr_info("\nExiting with  mutex having count=%d:\n",
	//	atomic_read(&my_mutex.count));
	synchronize_irq(irq);
	free_irq(irq, &my_dev_id);
	pr_info("Successfully unloading,  irq_counter = %d\n", irq_counter);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap5/lab3_unlock.c");
MODULE_LICENSE("GPL v2");
