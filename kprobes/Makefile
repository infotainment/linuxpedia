obj-m +=kprobe.o kprobe_example.o jprobe_example.o 
KDIR= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

modules clean distclean mrproper:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) $@
