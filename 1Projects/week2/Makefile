# Makefile for keylogger_driver

obj-m += keylogger_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

insert:
	sudo insmod keylogger_driver.ko || dmesg | tail -n 10

remove:
	sudo rmmod keylogger_driver || dmesg | tail -n 10
