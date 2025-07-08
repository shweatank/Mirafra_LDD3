# Kbuild Makefile for “mymod_demo.ko”
obj-m  := mymod.o   # source file name

# Location of the current kernel build directory
KDIR   := /lib/modules/$(shell uname -r)/build
PWD    := $(shell pwd)

# Build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

