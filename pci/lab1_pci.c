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
 * PCI Devices
 *
 * Write a module that scans your PCI devices, and gathers information
 * about them.
 *
 * For each found device, read some information from its configuration
 * register. (Make sure you read /usr/src/linux/include/linux/pci.h to
 * get symbolic names.)  Fields you may wish to obtain could include:
 * PCI_VENDOR_ID, PCI_DEVICE_ID, PCI_REVISION_ID, PCI_INTERRUPT_LINE,
 * PCI_LATENCY_TIMER, PCI_COMMAND.
 *
 * The information you obtain should agree with that in /proc/pci.
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/errno.h>
#include <linux/init.h>

static int __init my_init(void)
{
	u16 dval;
	char byte;
	int j = 0;
	struct pci_dev *pdev = NULL;

	pr_info("LOADING THE PCI_DEVICE_FINDER\n");

	/* either of the following looping constructs will work */

	for_each_pci_dev(pdev) {

		/*    while ((pdev = pci_get_device
		   (PCI_ANY_ID, PCI_ANY_ID, pdev))) { */

		pr_info("\nFOUND PCI DEVICE # j = %d, ", j++);
		pr_info("READING CONFIGURATION REGISTER:\n");

		pr_info("Bus,Device,Function=%s", pci_name(pdev));

		pci_read_config_word(pdev, PCI_VENDOR_ID, &dval);
		pr_info(" PCI_VENDOR_ID=%x", dval);

		pci_read_config_word(pdev, PCI_DEVICE_ID, &dval);
		pr_info(" PCI_DEVICE_ID=%x", dval);

		pci_read_config_byte(pdev, PCI_REVISION_ID, &byte);
		pr_info(" PCI_REVISION_ID=%d", byte);

		pci_read_config_byte(pdev, PCI_INTERRUPT_LINE, &byte);
		pr_info(" PCI_INTERRUPT_LINE=%d", byte);

		pci_read_config_byte(pdev, PCI_LATENCY_TIMER, &byte);
		pr_info(" PCI_LATENCY_TIMER=%d", byte);

		pci_read_config_word(pdev, PCI_COMMAND, &dval);
		pr_info(" PCI_COMMAND=%d\n", dval);

		/* decrement the reference count and release */
		pci_dev_put(pdev);

	}
	return 0;
}

static void __exit my_exit(void)
{
	pr_info("UNLOADING THE PCI DEVICE FINDER\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD:2.0 chap12/lab1_pci.c");
MODULE_LICENSE("GPL v2");
