# Name of the module (without extension)
obj-m += mouse_irq.o

# Kernel build directory (automatically uses the running kernel)
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target to build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Target to clean up the generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

