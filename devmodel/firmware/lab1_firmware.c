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
#include <linux/device.h>
#include <linux/firmware.h>

#define FWFILE "fwfile"
static const struct firmware *fw;

static void firmdrv_release(struct device *dev)
{
	pr_info("LDDFirm: Releasing firmware device\n");
}

static struct device dev = {
	.release = firmdrv_release
};

static int __init firmdrv_init(void)
{
	dev_set_name(&dev, "Firm0");
	device_register(&dev);
	pr_info("LDDFirm: Firmware device regisered\n");

	if (request_firmware(&fw, FWFILE, &dev)) {
		pr_err("LDDFirm: Requesting firmware failed\n");
		device_unregister(&dev);
		return -1;
	}
	pr_info("LDDFirm: Firmware contents=\n%s\n", fw->data);
	return 0;
}

static void __exit firmdrv_exit(void)
{
	release_firmware(fw);
	device_unregister(&dev);
	pr_info("LDDFirm: Released firmware and unregistered device\n");
}

module_init(firmdrv_init);
module_exit(firmdrv_exit);

MODULE_AUTHOR("Sonu Abraham");
MODULE_DESCRIPTION("LDD: Firmware");
MODULE_LICENSE("GPL v2");
