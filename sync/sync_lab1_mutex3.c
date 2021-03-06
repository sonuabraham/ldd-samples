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
 * Mutex Contention
 *
 * second and third module to test mutexes
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/mutex.h>
#include <asm/atomic.h>
#include <linux/errno.h>

extern struct mutex my_mutex;

static char *modname = __stringify(KBUILD_BASENAME);

static int __init my_init(void)
{
	pr_info("Trying to load module %s\n", modname);
	//pr_info("\n%s start count=%d:\n", modname,
	//	atomic_read(&my_mutex.count));
#if 0
	/* this branch should hang if not available */
	if (mutex_lock_interruptible(&my_mutex)) {
		pr_info("mutex unlocked by signal in %s\n", modname);
		return -EBUSY;
	}
#else
	/* this branch will return with failure if not available */
	if (!mutex_trylock(&my_mutex)) {
		pr_info("mutex_trylock failed in %s\n", modname);
		return -EBUSY;
	}
#endif
	//pr_info("\n%s mutex put mutex, count=%d:\n",
	//	modname, atomic_read(&my_mutex.count));

	return 0;
}

static void __exit my_exit(void)
{
	mutex_unlock(&my_mutex);
	//pr_info("\n%s mutex end count=%d:\n",
	//	modname, atomic_read(&my_mutex.count));
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap5/lab1_mutex3.c");
MODULE_LICENSE("GPL v2");
