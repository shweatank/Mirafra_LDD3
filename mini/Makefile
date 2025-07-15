# Makefile

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

# Kernel module
obj-m := temp_driver.o

all: driver.ko user_sysfs user_ioctl

driver.ko:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user_sysfs: user_wait.c
	$(CC) -o user_wait user_wait.c

user_ioctl: user_ioctl.c
	$(CC) -o user_ioctl user_ioctl.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user_wait user_ioctl

.PHONY: all clean

