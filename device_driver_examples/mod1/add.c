/*Add module programe practical number-3*/
#include<linux/module.h>//Needed by all the modules
#include<linux/kernel.h>//needed for KERN_INFO
#include<linux/init.h>//NEEDED FOR MACROS
#define DRIVER_AUTHOR "xxxx"
#define DRIVER_DESC "A simple sample driver-hello world."
#include "moduleadd.h"

static int __init add_init(void)
{
printk("I am in init module\n" );
sum=add_numbers(par1,par2);
printk("sum of numbers is:%d\n",sum);
return(0);
}

static void __exit add_exit(void)
{
printk(KERN_INFO"bye every one!\n" );
printk("I am in the clean up module\n" );
}


int add_numbers(int p,int p2)
{
int su;
su = p + p2;
return su;
}

EXPORT_SYMBOL(add_numbers);
//EXPORT_SYMBOL_GPL(add);
MODULE_LICENSE("GPL" );
MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_SUPPORTED_DEVICE("FIRST DEVICE" );
MODULE_DESCRIPTION(DRIVER_DESC);

module_init(add_init);
module_exit(add_exit); 
