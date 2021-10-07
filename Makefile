obj-m = cp210x.o
KDIR = /lib/modules/`uname -r`/build
SRCDIR = $(PWD)
# try this instead if you don't have PWD defined
# SRCDIR = $(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
all:
	$(MAKE) -C $(KDIR) M=$(SRCDIR) modules
clean:
	$(MAKE) -C $(KDIR) M=$(SRCDIR) clean
