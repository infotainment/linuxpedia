obj-m +=procex.o
KDIR= /lib/modules/$(shell uname -r)/build
all:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

modules clean distclean mrproper:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) $@
#clean:
#	rm -rf *.o *.ko *.mod.* .c* .t*
