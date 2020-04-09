/*
 * Copyright (C) 2013 Sonu Abraham
 * Copyright (C) 2013 Tech Training Engineers
 *
 * The source code in this file can be freely used, adapted,
 * and redistributed in source or binary form, so long as an
 * acknowledgment appears in derived source files. 
 *
 */

/* Sample Character Driver with mmap'ing */

#include <linux/module.h>
#include <linux/mm.h>

/* either of these (but not both) will work */
//#include "lab_char.h"
#include "lab_miscdev.h"

static int mycdrv_mmap(struct file *file, struct vm_area_struct *vma)
{
	pr_info("I entered the mmap function\n");
	if (remap_pfn_range(vma, vma->vm_start,
			    vma->vm_pgoff,
			    vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		return -EAGAIN;
	}

	return 0;
}

/* don't bother with open, release, read and write */

static const struct file_operations mycdrv_fops = {
	.owner = THIS_MODULE,
	.mmap = mycdrv_mmap,
};

module_init(my_generic_init);
module_exit(my_generic_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD 2:0 Sample Memory Map Driver Entry");
MODULE_LICENSE("GPL v2");
