obj-m += bitwise_logk.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

USER_APP := bitwise_log

all:
	@echo "Building kernel module and user app..."
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o $(USER_APP) bitwise_log.c

clean:
	@echo "Cleaning build files..."
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(USER_APP)


