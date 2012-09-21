#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/kprobes.h>

static unsigned int counter = 0;
int Pre_Handler(struct kprobe *p, struct pt_regs *regs){
	printk("Pre_Handler:counter=%u\n",counter++);
	return 0;
}

void Post_Handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags){
	printk("Post_Handler : counter=%u\n",counter++);
}

static struct kprobe kp;

int myinit(void)
{
	printk("Module Inserted\n ");
	kp.pre_handler	= Pre_Handler;
	kp.post_handler = Post_Handler;
	kp.addr 	= (kprobe_opcode_t *)0xc071c9a9; /*we can use address or symbol name */
//	kp.symbol_name	= "ip_rcv";
	register_kprobe(&kp);
	return 0;
}

void myexit(void)
{
	unregister_kprobe(&kp);
	printk("module removed\n ");
}


module_init(myinit);
module_exit(myexit);
MODULE_AUTHOR("Gokul- Thanks to original author");
MODULE_DESCRIPTION("KPROBE MODULE");
MODULE_LICENSE("GPL");
