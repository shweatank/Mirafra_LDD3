# Makefile for IRQ + Timer + Threads + Tasklet + Workqueue demo
obj-m += kernel_task.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	make -C $(KDIR) M=$(PWD) modules

clean:
	make -C $(KDIR) M=$(PWD) clean

