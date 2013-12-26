/*
 * Linux SPI driver example
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
#include <linux/spi/spi.h>

#include "spi_example.h"

static int spi_example_transfer(struct spi_device *spi)
{
	struct spi_message	msg;
	struct spi_transfer	xfer;
	u8			*buf;
	int			ret;

	/*
	 * Buffers must be properly aligned for DMA. kmalloc() ensures
	 * that.
	 */
	buf = kmalloc(8, GFP_KERNEL);
	if (!buf)
		return -ENOMEM;

	/* Put some "interesting" values into the TX buffer. */
	buf[0] = 0x55;
	buf[1] = 0x56;
	buf[2] = 0x57;
	buf[3] = 0x58;

	/* Initialize the SPI message and transfer data structures */
	spi_message_init(&msg);
	memset(&xfer, 0, sizeof(xfer));
	xfer.tx_buf = buf;
	xfer.rx_buf = buf + 4;
	xfer.len = 4;

	/* Add our only transfer to the message */
	spi_message_add_tail(&xfer, &msg);

	dev_info(&spi->dev, "sending %02x %02x %02x %02x...\n",
			buf[0], buf[1], buf[2], buf[3]);

	/* Send the message and wait for completion */
	ret = spi_sync(spi, &msg);
	if (ret == 0)
		dev_info(&spi->dev, "received %02x %02x %02x %02x\n",
				buf[4], buf[5], buf[6], buf[7]);

	/* Clean up and pass the spi_sync() return value on to the caller */
	kfree(buf);

	return ret;
}

static int __devinit spi_example_probe(struct spi_device *spi)
{
	struct spi_example_data *pdata = spi->dev.platform_data;
	int ret;

	/* Add per-device initialization code here */

	dev_notice(&spi->dev, "probe() called, value: %d\n",
			pdata ? pdata->value : -1);

	/* Try communicating with the device. */
	ret = spi_example_transfer(spi);

	return ret;
}

static int __devexit spi_example_remove(struct spi_device *spi)
{
	/* Add per-device cleanup code here */

	dev_notice(&spi->dev, "remove() called\n");

	return 0;
}

#ifdef CONFIG_PM
static int spi_example_suspend(struct spi_device *spi, pm_message_t state)
{
	/* Add code to suspend the device here */

	dev_notice(&spi->dev, "suspend() called\n");

	return 0;
}

static int spi_example_resume(struct spi_device *spi)
{
	/* Add code to resume the device here */

	dev_notice(&spi->dev, "resume() called\n");

	return 0;
}
#else
/* No need to do suspend/resume if power management is disabled */
# define spi_example_suspend	NULL
# define spi_example_resume	NULL
#endif

static struct spi_driver spi_example_driver = {
	.driver	= {
		.name		= "spi_example",
		.owner		= THIS_MODULE,
	},
	.probe		= spi_example_probe,
	.remove		= __devexit_p(spi_example_remove),
	.suspend	= spi_example_suspend,
	.resume		= spi_example_resume,
};

static int __init spi_example_init(void)
{
	return spi_register_driver(&spi_example_driver);
}
module_init(spi_example_init);

static void __exit spi_example_exit(void)
{
	spi_unregister_driver(&spi_example_driver);
}
module_exit(spi_example_exit);

/* Information about this module */
MODULE_DESCRIPTION("SPI driver example");
MODULE_AUTHOR("Atmel Corporation");
MODULE_LICENSE("Dual BSD/GPL");
