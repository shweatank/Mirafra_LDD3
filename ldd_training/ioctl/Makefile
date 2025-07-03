# Name of the module
obj-m += basic_ioctl_driver.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target to build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target to remove build files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

