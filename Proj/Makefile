# File: Makefile
obj-m += kernel_log.o

all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

user_app:
	gcc -o user_app user_app.c

