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
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/version.h>
#include <linux/ktime.h>
#include <linux/hrtimer.h>
#include <linux/slab.h>

static struct kt_data {
	struct hrtimer timer;
	ktime_t period;
} *data;

static enum hrtimer_restart ktfun(struct hrtimer *var)
{
	ktime_t now = var->base->get_time();
	pr_info("timer running at jiffies=%ld\n", jiffies);
	hrtimer_forward(var, now, data->period);
	return HRTIMER_RESTART;
}

static int __init my_init(void)
{
	data = kmalloc(sizeof(*data), GFP_KERNEL);
	data->period = ktime_set(1, 0);	/* short period, 1 second  */
	hrtimer_init(&data->timer, CLOCK_REALTIME, HRTIMER_MODE_REL);
	data->timer.function = ktfun;
	hrtimer_start(&data->timer, data->period, HRTIMER_MODE_REL);

	return 0;
}

static void __exit my_exit(void)
{
	hrtimer_cancel(&data->timer);
	kfree(data);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap7/lab7_hrexamp.c");
MODULE_LICENSE("GPL v2");
