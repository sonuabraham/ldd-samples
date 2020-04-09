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
 * Multiple Periodic Kernel Timers
 *
 * Write a module that launches two low-resolution periodic kernel
 * timer functions; i.e., they should re-install themselves.
 *
 * One periodic sequence should be for less than 256 ticks (so it
 * falls in the tv1 vector), and the other should be for less than 16
 * K ticks (so it falls in the tv2 vector.)
 *
 * Each time the timer functions execute, print out the total elapsed
 * time since the module was loaded (in jiffies).
 *
 */

#include <linux/module.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/jiffies.h>
#include <linux/slab.h>

static struct my_data {
	unsigned long period;
	unsigned long start_time;	/* jiffies */
	struct timer_list timer;
	char struct_id;		/* 'A' or 'B' */
} *data_array;			/* will kmalloc() an array of these */

static void my_timer_func(unsigned long var)
{
	struct my_data *dat = (struct my_data *)var;

	pr_info("%c: period = %ld  elapsed = %ld\n",
		dat->struct_id, dat->period, jiffies - dat->start_time);
	dat->start_time = jiffies;

	mod_timer(&dat->timer, dat->period + jiffies);
}

static int __init my_init(void)
{
	int i, period_in_secs;
	struct my_data *d;

	data_array = kmalloc(2 * sizeof(struct my_data), GFP_KERNEL);

	for (d = data_array, i = 0; i < 2; i++, d++) {
		init_timer(&d->timer);

		period_in_secs = (i == 0) ? 1 : 10;
		d->period = period_in_secs * HZ;
		d->struct_id = 'A' + i;
		d->start_time = jiffies;
		d->timer.function = my_timer_func;
		d->timer.expires = jiffies + d->period;
		d->timer.data = (unsigned long)d;

		add_timer(&d->timer);
	}
	pr_info("Module loaded, two timers started\n");
	return 0;
}

static void __exit my_exit(void)
{
	int i;
	struct my_data *d = data_array;
	for (i = 0; i < 2; i++, d++) {
		pr_info("deleted timer %c:  rc = %d\n",
			d->struct_id, del_timer(&d->timer));
	}
	kfree(data_array);
	pr_info("Module unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap7/lab4_periodic_timers_alt.c");
MODULE_LICENSE("GPL v2");
