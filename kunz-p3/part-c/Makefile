# Simple Makefile to build a simple misc driver
# Nick Glynn <Nick.Glynn@feabhas.com>
#

obj-m += misc_example.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

CC := $(CROSS_COMPILE)gcc

all:
		$(MAKE) -C $(KDIR) M=${shell pwd} modules
		$(CC) test_driver.c -o test_driver
        
clean:
		-$(MAKE) -C $(KDIR) M=${shell pwd} clean || true
		-rm *.o *.ko *.mod.{c,o} modules.order Module.symvers test_driver || true
