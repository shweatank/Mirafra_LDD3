# Makefile for building the UART platform driver

# Kernel build directory (adjust if needed)
KDIR := /lib/modules/$(shell uname -r)/build

# Name of the module (without .ko)
obj-m := myuart.o

# Default target
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean target
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

