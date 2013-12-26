# Change this to reflect the location of the Linux kernel sources
KDIR		:= /lib/modules/$(shell uname -r)/build

#ARCH		?= avr32
#CROSS_COMPILE	?= avr32-linux-
#export ARCH CROSS_COMPILE

KBUILD_TARGETS	:= clean help modules modules_install

.PHONY: all $(KBUILD_TARGETS)
all: modules

$(KBUILD_TARGETS):
	$(MAKE) -C $(KDIR) M=$(shell pwd) $@
