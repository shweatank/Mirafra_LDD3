obj-m += gpio_button_driver.o

KDIR := /home/mirafra/platform_driver/gipos-07-08/linux
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- M=$(PWD) clean

