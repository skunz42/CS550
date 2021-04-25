obj-m += misc_proc.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := $(CROSS_COMPILE)gcc

all:
		$(MAKE) -C $(KDIR) M=${shell pwd} modules
		$(CC) user_proc_list.c -o user_proc_list
        
clean:
		-$(MAKE) -C $(KDIR) M=${shell pwd} clean || true
		-rm *.o *.ko *.mod.{c,o} modules.order Module.symvers user_proc_list || true
