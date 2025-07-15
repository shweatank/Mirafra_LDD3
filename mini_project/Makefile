# Makefile - Build kernel module and user-space application

obj-m += temp_driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

USER_APP = user

.PHONY: all clean

all: $(USER_APP) module

# Build the kernel module
module:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Build the user-space application
$(USER_APP): user.c
	$(CC) -o $(USER_APP) user.c -lpthread

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(USER_APP)

