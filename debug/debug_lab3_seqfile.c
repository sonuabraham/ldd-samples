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
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/seq_file.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <asm/current.h>

/* proc_mkdir -> to create directory
 * proc_create_data -> make a proc file, add file_operations callbacks
 * proc_remove_entry -> remove directory/file
 * To the file operations structure we add generic seq opetaions that are
 * reusable and only open() fops needs to change. The open function calls
 * seq_open, which adds a seq_operations structure to the file. The
 * seq_operations structure needs to be given the start, next, stop and show
 * functions that will be used to iterate over the list that has to be printed.
 */

/* We would like to iterate over the task_struct list and print name and pids */
static void *my_start(struct seq_file *s, loff_t *pos)
{
	/* We just return the pointer to the list_head tasks structure for current
	 * process.infinite loop. The structure is available to other functions 
	 * via the argument void *v
	 */
	pr_info("START\n");
	return &(current->tasks);
}

static void *my_next(struct seq_file *s, void *v, loff_t *pos)
{
	/* void *v contains the pointer from start() or previous call of next.
	 * We need to type cast it to the correct data structure and return the
	 * next pointer as well
	 */
	pr_info("NEXT\n");
	struct list_head *ptr = ((struct list_head *) v)->next;
	return ptr;
}

static void my_stop(struct seq_file *s, void *v)
{
	pr_info("STOP\n");
}

static int my_show(struct seq_file *s, void *v)
{
	/* typecast void *v into list_head pointer. Find container of that
	 * pointer, which will be a pointer to the task_struct structure of that
	 * particular task. Derefference and print the task's comm and pid.
	 */
	struct list_head *ptr = ((struct list_head *) v)->next;
	struct task_struct *task = container_of(ptr, struct task_struct, tasks);
	seq_printf(s, "%u\t:\t%s\n", task->pid, task->comm);
	return 0;
}

static struct seq_operations swaps_op = {
	.start =        my_start,
	.next =         my_next,
	.stop =        	my_stop,
	.show =         my_show
};

static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &swaps_op);
}

/* The file_operations structure needs only the open function, all other
 * functions like read, seek and release are provided by seq_file.h, and
 * seq_file cannot be written to.
 */

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release,
};

static struct proc_dir_entry *proc_dir, *proc_file;

static int __init seqfile_init(void)
{
	/* Instead of procfs API, we are using seq_file API to read data
	 * from proc file. This allows data more than PAGE_SIZE to be read.
	 * Seq_file can't be written to by user spcae.
	 */
	proc_dir = proc_mkdir("ldd", NULL);
        if(proc_dir == NULL)
                return -ENOMEM;

	proc_file = proc_create_data("seqfile", 0644, proc_dir, &fops, NULL);
        if(proc_file == NULL) {
		remove_proc_entry("ldd", NULL);
		return -ENOMEM;
	}
        return 0;
}

static void __exit seqfile_exit(void)
{
	remove_proc_entry("seqfile", proc_dir);
	remove_proc_entry("ldd", NULL);
}

module_init(seqfile_init);
module_exit(seqfile_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate debugging using seq_file interface");
MODULE_LICENSE("GPL v2");

#if 0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/fs.h>
#include <linux/seq_file.h>
#include <linux/slab.h>


/*
 * The sequence iterator functions.  The position as seen by the
 * filesystem is just the count that we return.
 */
static void *ct_seq_start(struct seq_file *s, loff_t *pos)
{
	loff_t *spos = kmalloc(sizeof(loff_t), GFP_KERNEL);
	if (!spos)
		return NULL;
	*spos = *pos;
	return spos;
}

static void *ct_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	loff_t *spos = (loff_t *) v;
	*pos = ++(*spos);
	return spos;
}

static void ct_seq_stop(struct seq_file *s, void *v)
{
	kfree (v);
}

/*
 * The show function.
 */
static int ct_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *) v;
	seq_printf(s, "%Ld\n", *spos);
	return 0;
}

/*
 * Tie them all together into a set of seq_operations.
 */
static struct seq_operations ct_seq_ops = {
	.start = ct_seq_start,
	.next  = ct_seq_next,
	.stop  = ct_seq_stop,
	.show  = ct_seq_show
};


/*
 * Time to set up the file operations for our /proc file.  In this case,
 * all we need is an open function which sets up the sequence ops.
 */

static int ct_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &ct_seq_ops);
};

/*
 * The file operations structure contains our open function along with
 * set of the canned seq_ ops.
 */
static struct file_operations ct_file_ops = {
	.owner   = THIS_MODULE,
	.open    = ct_open,
	.read    = seq_read,
	.llseek  = seq_lseek,
	.release = seq_release
};
	
	
/*
 * Module setup and teardown.
 */

static int ct_init(void)
{
	struct proc_dir_entry *entry;

	entry = create_proc_entry("sequence", 0, NULL);
	if (entry)
		entry->proc_fops = &ct_file_ops;
	return 0;
}

static void ct_exit(void)
{
	remove_proc_entry("sequence", NULL);
}

module_init(ct_init);
module_exit(ct_exit);


MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab11_seq.c");
MODULE_LICENSE("GPL v2");
#endif
#if 0
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
 * Using seq_file for the proc filesystem.
 *
 * Take the simple "x_busy" proc entry discussed earlier, and
 * re-implement it using the seq_file interface.
 *
 * As a parameter, input the number of lines to print out.
 *
 */

#include <linux/module.h>
#include <linux/sched.h>	/* Get "jiffies" from here */
#include <linux/proc_fs.h>
#include <linux/init.h>
#include <linux/seq_file.h>

static int items = 1;
static int x_delay = 1;
static unsigned long future;

static char const my_proc[] = { "x_busy" };

/* Sequential file iterator                                              */

static void *x_busy_seq_start(struct seq_file *sf, loff_t * pos)
{
	void *results;

	if (*pos < items) {
		future = jiffies + x_delay * HZ;
		while (time_before(jiffies, future)) ;
		results = (void *)&jiffies;
	} else {
		results = NULL;
	}
	return results;
}

static void *x_busy_seq_next(struct seq_file *sf, void *v, loff_t * pos)
{
	void *results;

	(*pos)++;
	if (*pos < items) {
		future = jiffies + x_delay * HZ;
		while (time_before(jiffies, future)) ;
		results = (void *)&jiffies;
	} else {
		results = NULL;
	}
	return results;
}

static void x_busy_seq_stop(struct seq_file *sf, void *v)
{
	/* Nothing to do here */
}

static int x_busy_seq_show(struct seq_file *sf, void *v	/* jiffies in disquise */
    )
{
	volatile unsigned long *const jp = (volatile unsigned long *)v;
	int results;

	seq_printf(sf, "jiffies = %lu.\n", *jp);
	results = 0;
	return results;
}

static struct seq_operations proc_x_busy_seq_ops = {
	.start = x_busy_seq_start,
	.next = x_busy_seq_next,
	.stop = x_busy_seq_stop,
	.show = x_busy_seq_show,
};

static int proc_x_busy_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &proc_x_busy_seq_ops);
}

static const struct file_operations proc_x_busy_operations = {
	.open = proc_x_busy_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = seq_release
};

static struct proc_dir_entry *x_proc_busy;

static int __init my_init(void)
{
	int results;

	results = -1;
	do {
		x_proc_busy = create_proc_entry(my_proc, 0, NULL);
		if (!x_proc_busy) {
			break;
		}
		x_proc_busy->proc_fops = &proc_x_busy_operations;
		results = 0;
	} while (0);
	return results;
}

static void __exit my_exit(void)
{
	if (x_proc_busy) {
		remove_proc_entry(my_proc, NULL);
	}
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap4/lab8_seqfile.c");
MODULE_LICENSE("GPL v2");
module_param(items, int, S_IRUGO);
MODULE_PARM_DESC(items, "How many items to simulate");

#endif