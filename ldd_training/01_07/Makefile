# Add your module to the list
obj-m += enum_driver.o

# Kernel build directory
KDIR := /lib/modules/$(shell uname -r)/build

# Current working directory
PWD := $(shell pwd)

# Default target to build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Target to clean built files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

