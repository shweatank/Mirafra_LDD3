#obj-m=waitqueue_demo.o
#obj-m=kthread_demo.o
obj-m=interrupt_threaded_demo.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
