# Module name (change if your .c file is named differently)
obj-m += sysa_k.o network.o kernel_rpi.o

# Path to the kernel build directory (automatically set by `make`)
KDIR := /lib/modules/$(shell uname -r)/build

# Use current directory as the working directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean up
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

