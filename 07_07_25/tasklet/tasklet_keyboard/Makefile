# Name of your module source file (change if your .c file has a different name)
obj-m += tasklet_key.o

# The directory of the kernel build system
KDIR := /lib/modules/$(shell uname -r)/build

# The current directory (your module source folder)
PWD := $(shell pwd)

# Default target to build the module
all:
	make -C $(KDIR) M=$(PWD) modules

# Clean up all generated files
clean:
	make -C $(KDIR) M=$(PWD) clean

