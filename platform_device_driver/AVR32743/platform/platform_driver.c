/*
 * Linux platform driver example
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
#include <linux/device.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include "platform_driver.h"

static int platform_example_probe(struct platform_device *pdev)
{
	struct platform_example_data *pdata = pdev->dev.platform_data;

	/* Add per-device initialization code here */

	dev_notice(&pdev->dev, "probe() called, value: %d\n",
			pdata ? pdata->value : -1);

	return 0;
}

static int platform_example_remove(struct platform_device *pdev)
{
	/* Add per-device cleanup code here */

	dev_notice(&pdev->dev, "remove() called\n");

	return 0;
}

#ifdef CONFIG_PM
static int
platform_example_suspend(struct platform_device *pdev, pm_message_t state)
{
	/* Add code to suspend the device here */

	dev_notice(&pdev->dev, "suspend() called\n");

	return 0;
}

static int platform_example_resume(struct platform_device *pdev)
{
	/* Add code to resume the device here */

	dev_notice(&pdev->dev, "resume() called\n");

	return 0;
}
#else
/* No need to do suspend/resume if power management is disabled */
# define platform_example_suspend	NULL
# define platform_example_resume	NULL
#endif

static struct platform_driver platform_example_driver = {
	.probe		= platform_example_probe,
	.remove		= platform_example_remove,
	.suspend	= platform_example_suspend,
	.resume		= platform_example_resume,
	.driver	= {
		.name	= "platform_example",
	},
};

static int __init platform_example_init(void)
{
	return platform_driver_register(&platform_example_driver);
}
module_init(platform_example_init);

static void __exit platform_example_exit(void)
{
	platform_driver_unregister(&platform_example_driver);
}
module_exit(platform_example_exit);

/* Information about this module */
MODULE_DESCRIPTION("Platform driver example");
MODULE_AUTHOR("Atmel Corporation");
MODULE_LICENSE("Dual BSD/GPL");
