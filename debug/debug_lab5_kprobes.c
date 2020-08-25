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
#include <linux/kprobes.h>
#include <linux/kallsyms.h>

#define PRCUR(t) pr_info("current->comm=%s, current->pid=%d\n", t->comm, t->pid);

static char *name = "do_fork";
module_param(name, charp, S_IRUGO);

static struct kprobe kp;

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
	pr_info("pre_handler: p->addr=0x%p\n", p->addr);
	PRCUR(current);
	return 0;
}

static void handler_post(struct kprobe *p, struct pt_regs *regs,
			 unsigned long flags)
{
	pr_info("post_handler: p->addr=0x%p\n", p->addr);
	PRCUR(current);
}

static int handler_fault(struct kprobe *p, struct pt_regs *regs, int trapnr)
{
	pr_info("fault_handler:p->addr=0x%p\n", p->addr);
	PRCUR(current);
	return 0;
}

static int __init my_init(void)
{
	/* set the handler functions */

	kp.pre_handler = handler_pre;
	kp.post_handler = handler_post;
	kp.fault_handler = handler_fault;
	kp.symbol_name = name;

	if (register_kprobe(&kp)) {
		pr_info("Failed to register kprobe, quitting\n");
		return -1;
	}

	pr_info("Hello: module loaded at 0x%p\n", my_init);

	return 0;
}

static void __exit my_exit(void)
{
	unregister_kprobe(&kp);
	pr_info("Bye: module unloaded from 0x%p\n", my_exit);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab1_kprobes.c");
MODULE_LICENSE("GPL v2");
