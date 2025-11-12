# Makefile for PCI Driver

# Module name
obj-m := pci_driver.o

# Kernel source directory
KERNEL_SRC ?= /lib/modules/$(shell uname -r)/build

# Current directory
PWD := $(shell pwd)

# Compiler flags
ccflags-y := -Wall -Wextra

# Default target
all: module

# Build kernel module
module:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules

# Clean build files
clean:
	$(MAKE) -C $(KERNEL_SRC) M=$(PWD) clean
	rm -f Module.symvers modules.order

# Install module
install: module
	sudo $(MAKE) -C $(KERNEL_SRC) M=$(PWD) modules_install
	sudo depmod -a

# Uninstall module
uninstall:
	sudo rm -f /lib/modules/$(shell uname -r)/extra/pci_driver.ko
	sudo depmod -a

# Load module
load:
	sudo insmod pci_driver.ko

# Unload module
unload:
	sudo rmmod pci_driver

# Reload module
reload: unload load

# Check kernel messages
dmesg:
	dmesg | tail -30

# Show module info
info:
	modinfo pci_driver.ko

# Check if module is loaded
check:
	lsmod | grep pci_driver

# List PCI devices
lspci:
	lspci -v

# Show device node
show-dev:
	ls -l /dev/pcidev

# Test read from device
test-read:
	cat /dev/pcidev

# Test write to device
test-write:
	echo "Test data" | sudo tee /dev/pcidev

# Full test sequence
test: load
	@echo "Waiting 2 seconds for device initialization..."
	@sleep 2
	@echo "\n=== Module Status ==="
	@$(MAKE) check
	@echo "\n=== Device Node ==="
	@$(MAKE) show-dev
	@echo "\n=== Reading Device Info ==="
	@sudo cat /dev/pcidev
	@echo "\n=== Kernel Messages ==="
	@$(MAKE) dmesg

# Help target
help:
	@echo "Available targets:"
	@echo "  make all         - Build the module (default)"
	@echo "  make module      - Build the kernel module"
	@echo "  make clean       - Clean build files"
	@echo "  make install     - Install module permanently"
	@echo "  make uninstall   - Remove installed module"
	@echo "  make load        - Load module into kernel"
	@echo "  make unload      - Unload module from kernel"
	@echo "  make reload      - Unload and load module"
	@echo "  make dmesg       - Show kernel messages"
	@echo "  make info        - Show module information"
	@echo "  make check       - Check if module is loaded"
	@echo "  make lspci       - List PCI devices"
	@echo "  make show-dev    - Show device node"
	@echo "  make test-read   - Test reading from device"
	@echo "  make test-write  - Test writing to device"
	@echo "  make test        - Run full test sequence"
	@echo "  make help        - Show this help message"

.PHONY: all module clean install uninstall load unload reload dmesg info check lspci show-dev test-read test-write test help
