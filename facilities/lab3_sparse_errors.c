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
 * Finding Errors With Sparse (CONTAINING ERRORS)
 *
 * We give you a minimal module that compiles cleanly, but has at
 * least two errors that show up with the use of sparse.
 *
 * Install sparse according to the description given earlier and
 * correct the errors.
 *
 * Two possible solutions are given, depending on the intent in the
 * orginal module.
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/uaccess.h>

static void my_fun(char *buf1, char *buf2, int count, struct task_struct *s)
{
	int rc;
	rc = copy_from_user(buf2, buf1, count);
}

static int __init my_init(void)
{
	int count = 32;
	char buf1[32], buf2[32];
	my_fun(buf1, buf2, count, 0);
	return 0;
}

static void __exit my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap11/lab3_sparse_errors.c");
MODULE_LICENSE("GPL v2");
