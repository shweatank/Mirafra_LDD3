#obj-m=file_sys.o
#obj-m=sys_fs.o
obj-m=proc_fs.o
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
