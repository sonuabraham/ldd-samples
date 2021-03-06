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
#include <linux/init.h>
#include <linux/interrupt.h>

#define KBD_DATA_REG        0x60    /* I/O port for keyboard data */
#define KBD_SCANCODE_MASK   0x7f
#define KBD_STATUS_MASK     0x80
#define SHARED_IRQ 1   /* IRQ number for keyboard (i8042) */
static int irq = SHARED_IRQ, my_dev_id, irq_counter = 0;
module_param(irq, int, S_IRUGO);

static struct simp {
	char code;
} t_data = {
.code = 0};

static void tasklet_handler(unsigned long t_arg)
{
	struct simp *datum;
	datum = (struct simp *)t_arg;
	//pr_info("I am in t_fun, jiffies = %ld\n", jiffies);
	//pr_info(" I think my current task pid is %d\n", (int)current->pid);
	pr_info(" Scan Code from ISR: %x\n", datum->code);
}

/* initialize tasklet */
static DECLARE_TASKLET(t_name, tasklet_handler, (unsigned long)&t_data);


static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	char scancode;
	irq_counter++;
	//pr_info("In the ISR: counter = %d\n", irq_counter);
    scancode = inb(KBD_DATA_REG);
    pr_info("Scan Code in ISR %x %s\n",
            scancode & KBD_SCANCODE_MASK,
            scancode & KBD_STATUS_MASK ? "Released" : "Pressed");
	
	t_data.code = scancode & KBD_SCANCODE_MASK;
	tasklet_schedule(&t_name);
	return IRQ_HANDLED;	
}

static int __init my_init(void)
{
	if (request_irq
	    (irq, my_interrupt, IRQF_SHARED, "my_interrupt", &my_dev_id)) {
		pr_info("Failed to reserve irq %d\n", irq);
		return -1;
	}
	pr_info("Successfully loading ISR handler\n");
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
MODULE_DESCRIPTION("LDD: Interrupt Handling with Tasklets");
MODULE_LICENSE("GPL v2");
