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
 * Sharing All Interrupts, Bottom Halves (tasklet solution)
 *
 * Extend the solution to share all possible interrupts, and evaluate
 * the consumer/producer problem.
 *
 */
 
#include <linux/module.h>
#include "lab_all_interrupt.h"

static struct my_dat {
	int irq;
} my_data;

static void t_fun(unsigned long t_arg)
{
	struct my_dat *data = (struct my_dat *)t_arg;
	atomic_inc(&bhs[data->irq]);
}

static DECLARE_TASKLET(t, t_fun, (unsigned long)&my_data);

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	struct my_dat *data = (struct my_dat *)&my_data;
	data->irq = irq;
	atomic_inc(&interrupts[irq]);
	tasklet_schedule(&t);
	mdelay(delay);
	/* we return IRQ_NONE because we are just observing */
	return IRQ_NONE;
}

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap10/lab6_all_tasklet.c");
MODULE_LICENSE("GPL v2");
