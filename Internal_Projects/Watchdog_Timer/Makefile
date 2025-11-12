obj-m += driver.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o user user.c

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f user

