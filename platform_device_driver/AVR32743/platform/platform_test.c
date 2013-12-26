/*
 * Linux platform device test module
 *
 * Copyright (c) 2008, Atmel Corporation All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. The name of ATMEL may not be used to endorse or promote products
 *    derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/slab.h>

#include "platform_driver.h"

#define PLATFORM_DEVICE_NAME	"platform_example"

static unsigned int nr_devices = 1;
module_param(nr_devices, uint, S_IRUGO);
MODULE_PARM_DESC(nr_devices, "Number of platform devices to add");

static int value;
module_param(value, int, S_IRUGO);
MODULE_PARM_DESC(value, "Value to pass as platform data");

/* Keep track of the devices so that we can free them later */
static struct platform_device **devices;

static int __init platform_test_init(void)
{
	struct platform_device *pdev;
	struct platform_example_data data;
	unsigned int i;
	int ret;

	devices = kzalloc(nr_devices * sizeof(struct platform_device *),
			GFP_KERNEL);
	if (!devices)
		return -ENOMEM;

	/*
	 * Initialize platform data. In this case, we pass the same
	 * value with all the devices -- real platform devices usually
	 * have platform-specific data which may be different from
	 * device to device.
	 */
	data.value = value;

	for (i = 0; i < nr_devices; i++) {
		ret = -ENOMEM;
		pdev = platform_device_alloc(PLATFORM_DEVICE_NAME, i);
		if (!pdev)
			goto fail;

		/*
		 * The data is copied into a new dynamically allocated
		 * structure, so it's ok to pass variables defined on
		 * the stack here.
		 */
		ret = platform_device_add_data(pdev, &data, sizeof(data));
		if (ret)
			goto fail;

		printk(KERN_INFO "Registering device \"%s.%d\"...\n",
				pdev->name, pdev->id);
		ret = platform_device_add(pdev);
		if (ret)
			goto fail;

		devices[i] = pdev;
	}

	return 0;

fail:
	/*
	 * The last device was never registered, so we may free it
	 * directly. The others, however, were, so we must unregister
	 * them. They are freed automatically when the last reference
	 * goes away.
	 *
	 * In both cases, any dynamically allocated resources and
	 * platform data will be freed automatically.
	 */
	platform_device_put(pdev);
	while (i--)
		platform_device_del(devices[i]);

	kfree(devices);

	return ret;
}
module_init(platform_test_init);

static void __exit platform_test_exit(void)
{
	struct platform_device *pdev;
	unsigned int i;

	for (i = nr_devices; i > 0; i--) {
		pdev = devices[i - 1];

		if (pdev)
			printk(KERN_INFO "Removing device %s...\n",
					pdev->dev.init_name);

		/* If pdev is NULL, this is a no-op */
		platform_device_del(pdev);
	}

	kfree(devices);
}
module_exit(platform_test_exit);

/* Information about this module */
MODULE_DESCRIPTION("Platform device test module");
MODULE_AUTHOR("Atmel Corporation");
MODULE_LICENSE("Dual BSD/GPL");
