obj-m := basic_driver.o

KDIR := /lib/modules/$(shell uname -r)/build 

PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=gcc-12 modules 
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
