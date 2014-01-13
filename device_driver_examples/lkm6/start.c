/*
 *  start.c - Illustration of multi filed modules
 */

#include <linux/kernel.h>	/* We're doing kernel work */
#include <linux/module.h>	/* Specifically, a module */

int init_module(void)
{
	int a=10;
	printk(KERN_INFO "Hello, world - this is the kernel speaking\n");

	printk(KERN_INFO "the value of a is%d\n",a);
	return 0;
}
