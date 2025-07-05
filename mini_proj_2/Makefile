# If your module name is char_dev.c, then obj-m becomes char_dev.o
obj-m +=kernel_module.o

# Kernel directory path
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean the build files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

