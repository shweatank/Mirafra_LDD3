# SPDX-License-Identifier: GPL-2.0
# Makefile for SSD1306 Platform Driver (using I2C)
# Author: Harsha Sri Sampath

# Module name
obj-m := i2c_driver.o

# Kernel source path (use uname -r to find current kernel version)
KDIR := /lib/modules/$(shell uname -r)/build

# Default target - build module for current kernel
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Clean build files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

