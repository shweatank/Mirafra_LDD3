# ---------- Makefile -----------------------------------------------------
# Builds:
#   1) Kernel module  -> enum.ko      (from enum.c)
#   2) User program   -> enumm        (from enumm.c)
#
# Targets:
#   make        – build both
#   make kernel – build only kernel module
#   make user   – build only user program
#   make clean  – remove all build artifacts
# ------------------------------------------------------------------------

# -------- kernel module --------------------------------------------------
obj-m := enum.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

# -------- user program ---------------------------------------------------
USERPROG := enumm
USER_SRC := enumm.c

# -------- rules ----------------------------------------------------------
.PHONY: all kernel user clean

all: kernel user

kernel:
	@echo "==> Building kernel module"
	$(MAKE) -C $(KDIR) M=$(PWD) modules

user: $(USERPROG)

$(USERPROG): $(USER_SRC)
	@echo "==> Building user program"
	$(CC) -Wall -O2 $< -o $@

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	$(RM) $(USERPROG)

# ------------------------------------------------------------------------
