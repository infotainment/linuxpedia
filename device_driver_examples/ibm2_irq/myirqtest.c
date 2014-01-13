#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/interrupt.h>

static int irq;
static char *interface;

module_param(interface,charp, 0000);
MODULE_PARM_DESC(interface, "A network interface");
module_param(irq, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(irq, "The IRQ of the network interface");



static irqreturn_t myinterrupt(int irq, void *dev_id, struct pt_regs *regs)
{
static int mycount = 0;
if (mycount < 100) {
printk("Interrupt!\n");
mycount++;
}
return IRQ_NONE;
}

static int __init myirqtest_init(void)
{
//printk ("My module worked!\n");
//return 0;

if (request_irq(irq, &myinterrupt, IRQF_SHARED, interface, &irq)) 
	{
		printk(KERN_ERR "myirqtest: cannot register IRQ %d\n", irq);
		return -EIO;
	}	

printk("Request on IRQ %d succeeded\n", irq);

return 0;
}

static void __exit myirqtest_exit(void)
{
//printk ("Unloading my module.\n");

free_irq(irq, &irq);
printk("Freeing IRQ %d\n", irq);

return;
}

module_init(myirqtest_init);
module_exit(myirqtest_exit);
MODULE_LICENSE("GPL");
