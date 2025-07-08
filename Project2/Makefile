EXTRA_CFLAGS += -I$(PWD)

obj-m += mouse_drv.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	# $(CC) user_test.c -o user_test

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) user_test

