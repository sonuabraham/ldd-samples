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
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h>
#include <linux/uaccess.h>

#define MAXBUFF 128
static char proc_buff[MAXBUFF]="HELLO PROC\0";       
static struct proc_dir_entry *proc_dir, *proc_file;
 
static ssize_t procfile_read(struct file *f, char __user *buf,
                size_t len, loff_t *off)
{
        return simple_read_from_buffer(buf, len, off, proc_buff, strlen(proc_buff));
}

static ssize_t procfile_write(struct file *f, const char __user *buf,
                    size_t len, loff_t *off)
{
        int ret = simple_write_to_buffer(proc_buff, MAXBUFF, off, buf, len);
        proc_buff[ret] = '\0';
        return ret;             
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.read  = procfile_read,
	.write = procfile_write,
};

static int __init dbgproc_init(void)
{
	/* create a directory */
	proc_dir = proc_mkdir("ldd", NULL);
        if(proc_dir == NULL)
                return -ENOMEM;
        
    /* create a file */
	proc_file = proc_create_data("procfile", 0644, proc_dir, &fops, NULL);
        if(proc_file == NULL) {
		remove_proc_entry("ldd", NULL);
		return -ENOMEM;
	}

	return 0;
}

static void __exit dbgproc_exit(void)
{
	remove_proc_entry("procfile", proc_dir);
	remove_proc_entry("ldd", NULL);
}

module_init(dbgproc_init);
module_exit(dbgproc_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Module to demonstrate debugging using proc fs");
MODULE_LICENSE("GPL v2");

