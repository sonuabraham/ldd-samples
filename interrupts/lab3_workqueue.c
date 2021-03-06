/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

 
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/workqueue.h>

#define KBD_DATA_REG        0x60    /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80
#define SHARED_IRQ 1  /* IRQ number for keyboard (i8042) */
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param(irq, int, S_IRUGO);

static struct workqueue_struct *my_workq;

static struct my_dat {
	atomic_t len;
	struct work_struct work;
} my_data;

static void w_fun(struct work_struct *w_arg)
{
	struct my_dat *data = container_of(w_arg, struct my_dat, work);
	pr_info("I am in w_fun, jiffies = %ld\n", jiffies);
	pr_info(" I think my current task pid is %d\n", (int)current->pid);
	pr_info(" my data is: %d\n", atomic_read(&data->len));
}


static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	char scancode;
	struct my_dat *data = (struct my_dat *)&my_data;
	irq_counter++;
	pr_info("In the ISR: counter = %d\n", irq_counter);



    scancode = inb(KBD_DATA_REG);
    pr_info("Scan Code %x %s\n",
            scancode & KBD_SCANCODE_MASK,
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");

	schedule_work(&data->work);
	atomic_set(&data->len, scancode & KBD_SCANCODE_MASK);	
	return IRQ_NONE;	
}

static int __init my_init(void)
{
	struct my_dat *data = (struct my_dat *)&my_data;
	if (request_irq
	    (irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id)) {
		pr_info("Failed to reserve irq %d\n", irq);
		return -1;
	}
	pr_info("Successfully loading ISR handler\n");
	INIT_WORK(&data->work, w_fun);
	return 0;
}

static void __exit my_exit(void)
{
	synchronize_irq(irq);
	free_irq(irq, &my_dev_id);
	pr_info("Successfully unloading,  irq_counter = %d\n", irq_counter);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Interrupt handling with Work Queue");
MODULE_LICENSE("GPL v2");
