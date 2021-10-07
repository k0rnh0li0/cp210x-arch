# cp210x-arch

This repo contains the CP210x USB to UART driver source as downloaded from Silabs, modified
for building on Arch Linux.

### Building/installation steps:
```
# Install packages as necessary
$ sudo pacman -S base-devel linux-headers

# Build driver
$ make

# Install driver
$ sudo cp cp210x.ko /lib/modules/$(uname -r)/kernel/drivers/usb/serial
$ sudo insmod cp210x.ko
```

### List of modifications

* Updated Makefile to reference `/usr/lib/src` as the kernel source directory
* Updated cp210x.c, changing the return type of `cp210x_port_remove` from int to void.

As of [Linux commit c5d1448](https://github.com/torvalds/linux/commit/c5d1448fa353242635fa3e1fed6ab4558e0e7d9a#diff-21953fff57a63a3429b3af54ea4d68ab6fa7591ea51286c9909730cca91dcb04),
`struct usb_serial_driver` in `<linux/usb/serial.h>` expects the callback for the remove event to have a void return type.
