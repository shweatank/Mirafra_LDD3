#obj-m=calculator.o
#obj-m=thread_demo.o
obj-m=thread_interrupt.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
