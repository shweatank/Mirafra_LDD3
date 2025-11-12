obj-m += uart_driver.o

KDIR := /home/mirafra/LDD_training/platform_drivers/gpio/linux
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean
