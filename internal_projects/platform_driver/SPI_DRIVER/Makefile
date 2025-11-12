# Makefile for building ili9225 kernel module
obj-m += ili9225.o

KDIR ?= /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean

install: all
	# copy module into extras and refresh
	install -D -m 0644 ili9225.ko /lib/modules/$(shell uname -r)/extra/ili9225.ko
	depmod -a

uninstall:
	rm -f /lib/modules/$(shell uname -r)/extra/ili9225.ko
	depmod -a

