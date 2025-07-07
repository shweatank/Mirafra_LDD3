# Name of the module source file
obj-m += keyboard_driver.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target to remove generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

