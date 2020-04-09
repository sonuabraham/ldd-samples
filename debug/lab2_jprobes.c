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
 * Using jprobes
 *
 * Test the jprobes facility by instrumenting a commonly used exported
 * kernel function.
 *
 * Keep a counter of how many times the function is called.  If you
 * print it out each time, be careful not to get overwhelmed with
 * output.
 *
 */

#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

static long mod_timer_count = 0;

static void mod_timer_inst(struct timer_list *timer, unsigned long expires)
{
	mod_timer_count++;
	if (mod_timer_count % 10 == 0)
		pr_info("mod_timer_count=%ld\n", mod_timer_count);
	jprobe_return();
}

static struct jprobe jp = {
	.kp.addr = (kprobe_opcode_t *) mod_timer,
	.entry = (kprobe_opcode_t *) mod_timer_inst,
};

static int __init my_init(void)
{
	register_jprobe(&jp);
	pr_info("plant jprobe at %p, handler addr %p\n", jp.kp.addr, jp.entry);
	return 0;
}

static void __exit my_exit(void)
{

	unregister_jprobe(&jp);
	pr_info("jprobe unregistered\n");
	pr_info("FINAL:mod_timer_count=%ld\n", mod_timer_count);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab2_jprobes.c");
MODULE_LICENSE("GPL v2");
