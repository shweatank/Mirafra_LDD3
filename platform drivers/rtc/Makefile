obj-m += rtc_dri.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) CFLAGS_MODULE="-O0 -g" modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

