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
 * Sharing All Interrupts, Bottom Halves (workqueue solution)
 *
 * Extend the solution to share all possible interrupts, and evaluate
 * the consumer/producer problem.
 *
 */

#include <linux/module.h>
#include "lab_all_interrupt.h"

static struct my_dat {
	int irq;
	struct work_struct work;
} my_data;

static void w_fun(struct work_struct *w_arg)
{
	struct my_dat *data = container_of(w_arg, struct my_dat, work);
	atomic_inc(&bhs[data->irq]);
}

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	struct my_dat *data = (struct my_dat *)&my_data;
	data->irq = irq;
	atomic_inc(&interrupts[irq]);
	schedule_work(&data->work);
	mdelay(delay);
	/* we return IRQ_NONE because we are just observing */
	return IRQ_NONE;
}

static int my_init(void)
{
	struct my_dat *data = (struct my_dat *)&my_data;
	INIT_WORK(&data->work, w_fun);
	return my_generic_init();
}

module_init(my_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap10/lab6_all_workqueue.c");
MODULE_LICENSE("GPL v2");
