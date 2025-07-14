# Makefile for wait_q.c  — builds wait_q.ko
# ---------------------------------------------------------------

# Name of the object file to build (the .o is added automatically)
obj-m := wait_q.o

# Path to the kernel build directory
KDIR ?= /lib/modules/$(shell uname -r)/build

# Default target: build the module
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Optional: install the module (run with sudo)
install:
	sudo cp wait_q.ko /lib/modules/$(shell uname -r)/extra/
	sudo depmod -a

# Optional: remove the installed module
uninstall:
	sudo rm -f /lib/modules/$(shell uname -r)/extra/wait_q.ko
	sudo depmod -a

# Clean generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

