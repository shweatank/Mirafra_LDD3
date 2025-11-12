# Makefile for I2C OLED Driver

# Module name
obj-m := oled_i2c.o

# Kernel source directory - adjust based on your system
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# DTC compiler for device tree
DTC := dtc

# Default target
all: module dtbo

# Build kernel module
module:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

# Compile device tree overlay
dtbo:
	$(DTC) -@ -I dts -O dtb -o oled.dtbo oled.dts

# Clean build files
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f oled.dtbo

# Install module
install: module
	sudo $(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install
	sudo depmod -a

# Install device tree overlay (Raspberry Pi example)
install-dtbo: dtbo
	sudo cp oled.dtbo /boot/overlays/

# Load module
load:
	sudo insmod oled_i2c.ko

# Unload module
unload:
	sudo rmmod oled_i2c

# Check kernel messages
dmesg:
	dmesg | tail -20

# Show module info
info:
	modinfo oled_i2c.ko

.PHONY: all module dtbo clean install install-dtbo load unload dmesg info
