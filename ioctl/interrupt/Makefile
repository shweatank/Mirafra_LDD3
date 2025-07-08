# Build this single object into a loadable‐module
obj-m += keyboard_interrupt.o

# Default target: compile the module
all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Optional: install into the standard modules tree
install:
	sudo $(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules_install
	sudo depmod -a

# Clean intermediate files
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

