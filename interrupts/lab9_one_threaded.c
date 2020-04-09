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
 * Threaded Interrupt Handlers (one interrupt case)
 * 
 * If you are running a kernel version 2.6.30 or later, solve the
 * producer/consumer problem with a threaded interrupt handler.
 *
 * There are two types of solutions presented, one for just one shared
 * interrupt, one sharing them all, with the same delay parameter as
 * used in the earlier exercises.
 */

#include <linux/module.h>
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,30)
static int __init my_init(void)
{
	pr_warning("Threaded interrupts don't appear until 2.6.30\n");
	return -1;
}

module_init(my_init)
#else

#define THREADED_IRQ

#include "lab_one_interrupt.h"

static atomic_t nevents;

static irqreturn_t my_interrupt(int irq, void *dev_id)
{
	atomic_inc(&counter_th);
	atomic_inc(&nevents);
	mdelay(delay);		/* hoke up a delay to try to cause pileup */
	return IRQ_WAKE_THREAD;
}

static irqreturn_t thread_fun(int irq, void *thr_arg)
{
	do {
		atomic_inc(&counter_bh);
	}
	while (!atomic_dec_and_test(&nevents));
	pr_info("In BH: counter_th = %d, counter_bh = %d, nevents=%d\n",
		atomic_read(&counter_th), atomic_read(&counter_bh),
		atomic_read(&nevents));

	/* we return IRQ_NONE because we are just observing */
	return IRQ_NONE;
}

static int __init my_init(void)
{
	atomic_set(&nevents, 0);
	return my_generic_init();
}

module_init(my_init);
module_exit(my_generic_exit);
#endif

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap10/lab9_one_threaded.c");
MODULE_LICENSE("GPL v2");
