# Name of the module
obj-m := driver.o

# Kernel source directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean object files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

