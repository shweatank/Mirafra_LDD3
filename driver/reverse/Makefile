# Name of the object file (no .o here)
obj-m := reverse.o

# Default target: build the module against the running kernel
all:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

# Remove the built files
clean:
	$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

