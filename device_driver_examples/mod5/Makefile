obj-m += module5.o
module5-objs += module5_init.o module5_fops.o
M_TARGET = module5.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
