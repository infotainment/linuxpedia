#include<linux/module.h>
#include<linux/version.h>
#include<linux/kernel.h>
#include<linux/init.h>
#include<linux/kprobes.h>
#include<net/ip.h>
//extern int ip_rcv(struct sk_buff *skb, struct net_device *dev,struct packet_type *pt, struct net_device *orig_dev);
int my_handler (struct sk_buff *skb,struct net_device *dev,struct packet_type *pt, struct net_device *orig_dev) {
	struct iphdr *my_iph;
	u32 S_ip,D_ip;
	my_iph = ip_hdr(skb);
	S_ip   = my_iph->saddr;
	D_ip   = my_iph->daddr;
//	printk("Source IP: "NIPQUAD_FMT,NIPQUAD(S_ip));
//	printk("Desti IP: %u\n"NIPQUAD_FMT,NIPQUAD(D_ip));
	printk("Source IP:%u.%u.%u.%u",((unsigned char *)&S_ip)[0], \
         ((unsigned char *)&S_ip)[1], \
         ((unsigned char *)&S_ip)[2], \
         ((unsigned char *)&S_ip)[3]);
		jprobe_return();
}
	
static struct jprobe my_probe;
int myinit(void)
{
	my_probe.kp.addr = (kprobe_opcode_t *)0xc071c9a9;  /*kp.symbol_name  = "ip_rcv"; */
	my_probe.entry	 = (kprobe_opcode_t *)my_handler;
//	my_probe.kp.symbol_name	= "ip_rcv";
//	my_probe.entry		= *myhandler;
	register_jprobe(&my_probe);
	return 0;
}

void myexit(void)
{
	unregister_jprobe(&my_probe);
	printk("module removed\n ");
}

module_init(myinit);
module_exit(myexit);

MODULE_AUTHOR("Gokul- thanks to original author");
MODULE_DESCRIPTION("jprobe module example");
MODULE_LICENSE("GPL");
