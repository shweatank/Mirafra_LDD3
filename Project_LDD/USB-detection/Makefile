# This line tells the kernel build system which object file to build as a module
obj-m = usb.o  # 'kbd_tasklet.c' will be compiled into 'kbd_tasklet.ko'

# 'all' target - used to compile the kernel module
all:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules
	# -C tells make to change to the kernel build directory
	# $(shell uname -r) gets the current kernel version
	# M=$(PWD) passes the current directory to the kernel build system as the module source location
	# 'modules' tells the kernel build system to build the module(s) specified by obj-m

# 'clean' target - used to clean the build artifacts (like *.o, *.ko, *.mod.*)
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	# Same as above but 'clean' tells the kernel build system to remove generated files from the build

