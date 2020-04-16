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
 * Sempahore Contention
 *
 * Now do the same thing using semaphores instead of mutexes.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <asm/atomic.h>
#include <linux/version.h>
#include <linux/semaphore.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,37)
DECLARE_MUTEX(my_sem);
#else
DEFINE_SEMAPHORE(my_sem);
#endif
EXPORT_SYMBOL(my_sem);

static int __init my_init(void)
{
	pr_info("\nInitializing semaphore, ");
	pr_info("semaphore_count=%u\n", my_sem.count);
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("\nExiting semaphore, ");
	pr_info("semaphore_count=%u\n", my_sem.count);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module #1 to illustrate semaphores");
MODULE_LICENSE("GPL v2");
