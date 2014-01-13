obj-m+=avg.o add.o

PWD:=$(shell pwd)

KERNDIR:=/lib/modules/$(shell uname -r)/build/

all:
	$(MAKE) -C $(KERNDIR) M=$(PWD) modules

clean :
	$(MAKE) -C $(KERNDIR) M=$(PWD) clean 
