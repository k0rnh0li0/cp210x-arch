#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h> // to get close() define
#include <sys/ioctl.h>

// Make sure you have installed the SiLabs's version of cp210x.c driver
// which has these IOCTLs implemented.
#define IOCTL_GPIOGET		0x8000
#define IOCTL_GPIOSET		0x8001

#define CP2108

int main()
{
	int fd;
	printf( "CP210x Serial Test\n");
	fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
		printf( "Error opening port /dev/ttyUSB0\n");
		return 1;
	}
	// mask = 3: gpio #1 and #0; value = 2: set #1, clear #0
	// Clearing GPIO turns LED on on SiLabs blue EK boards.
#ifdef CP2108
	unsigned short gpioread = 0x3333;
	unsigned long gpio = 0x00020003;
#else
	unsigned char gpioread = 0x33;
	unsigned short gpio = 0x0203;
#endif
	ioctl(fd, IOCTL_GPIOGET, &gpioread);
	printf( "gpio read = %x\n", gpioread);

	printf( "gpio to write = %lx\n", gpio);
	ioctl(fd, IOCTL_GPIOSET, &gpio);

	ioctl(fd, IOCTL_GPIOGET, &gpioread);
	printf( "gpio read = %x\n", gpioread);

	close(fd);
	return 0;
}
