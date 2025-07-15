# Name of the module (without .ko)
obj-m := led_driver.o

# Kernel source path (you can override it when calling make)
KDIR := /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean object and dependency files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

