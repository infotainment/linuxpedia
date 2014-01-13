/*Add module programe practical number-3*/
#include<linux/module.h>//Needed by all the modules
#include<linux/kernel.h>//needed for KERN_INFO
#include<linux/init.h>//NEEDED FOR MACROS
#include<linux/moduleparam.h>
#define DRIVER_AUTHOR "XXX"
#define DRIVER_DESC "A simple sample driver-hello world."
#include "moduleavg.h"
int add_numbers(int,int);
static int __init avg_init(void)
{
printk("I am in init module\n");
a=avg(par1,par2);
printk("avg of numbers is:%d\n",a);
return(0);
}

static void __exit avg_exit(void)
{
printk(KERN_INFO"bye every one!\n");
printk("I am in the clean up module\n");
}


int avg(int p1,int p2)
{
int r;
r=add_numbers(p1,p2);
r=r/2;
return r;

}

MODULE_LICENSE("GPL");
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_SUPPORTED_DEVICE("AVG DRIVER");
MODULE_DESCRIPTION(DRIVER_DESC);

module_init(avg_init);
module_exit(avg_exit); 
