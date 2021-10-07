#include <stdio.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h> 
#include <gpiod.h>
#include <string.h>

/**
 * Test gpio on cp210x devices using gpiolib.
 * Read and write all the gpio of cp210x device at the same time.
 * Command: ./program_name a b
 * 			argv[1] : a is the file path of gpiochip.
 *					example: /dev/gpiochip0
 *			argv[2] : b is value that is writed to gpio.
 *					0 : Low level, turn on led on cp210x ek board
 *					1 : high level, turn off led on cp210x ek board
 * build: gcc cp210x_gpio_example_gpiolib.c -lgpiod -o cp210x_gpio_example_gpiolib
 */

#define READ_LINE		1
#define WRITE_LINE		0

#define NONE_STATE		-1
#define LED_ON			0
#define LED_OFF			1

void fill_array(int arr[], int value, size_t n)
{
	char i;

	for(i = 0; i < n; i++)
		arr[i] = value;
}

void display_status_lines(int num_lines, int *value, char option)
{
    int i;

	printf("\n********************************\n");
    for(i = 0; i < num_lines; i++) {
		if(option == READ_LINE)
        	printf("Read Line %d is:\t%d\n", i, value[i]);
		else if(option == WRITE_LINE)
        	printf("WRITE %d to Line %d\n", value[i], i);
	}
	printf("\n********************************\n");
}

int main(int argc, char *argv[])
{
	struct gpiod_chip *chip;
	struct gpiod_line *line;
	struct gpiod_line_iter *iter;
	struct gpiod_line_bulk *bulk = (struct gpiod_line_bulk *) malloc (sizeof(struct gpiod_line_bulk));
	int req;
	unsigned char value;
	int value_get[64];
	int value_set[64];
	unsigned char i;

	if(argc != 3) {
		printf("Wrong number of Arguments!\n");
		printf("Usage: %s [file path gpiochip] [value of gpio]\n\n", argv[0]);
		printf("%s : Name of program\n", argv[0]);
		printf("[file path gpiochip]: this is gpiochip file. Example: /dev/gpiochip1\n");
		printf("[value of gpio]:Value to set to all of gpios on cp210x device!\n");
		return -1;
	}

	printf("Running: GPIO example using gpiolib!\n");
	value = (unsigned char) (*argv[2] - '0');

	//Create bulk line.
	chip = gpiod_chip_open(argv[1]);
	if (!chip) {
		printf("Open file gpiochip error!\n");
		printf("NOTED: Need Admin permissions to open file!\n");
		return -1;
	}
	gpiod_line_bulk_init(bulk);
	iter = gpiod_line_iter_new(chip);
	if (!iter) {
		printf("Func: gpio_line_iter_new error!\n");
		return -1;
	}
	gpiod_foreach_line(iter, line)
	gpiod_line_bulk_add(bulk, line);
	gpiod_line_iter_free(iter);

	// Set GPIOs are output with default value is LED_OFF
	fill_array(value_set, LED_OFF, bulk->num_lines);
	req = gpiod_line_request_bulk_output(bulk,"gpio_state", value_set); 
	if (req) {
		printf("Request bulk output error\n");
		return -1;
	}
	printf("num_lines: %d\n", bulk->num_lines);

	//Reset value_get and value_set
	fill_array(value_get, NONE_STATE, bulk->num_lines);
	if (value == LED_ON)
		fill_array(value_set, LED_ON, bulk->num_lines);
	else 
		fill_array(value_set, LED_OFF, bulk->num_lines);


	//Read GPIOs value
	req = gpiod_line_get_value_bulk(bulk, value_get);
	if (req) {
		printf("Func: gpiod_line_get_value_bulk error %d\n", req);
		return -1;
	}
	display_status_lines(bulk->num_lines, value_get, READ_LINE);
	//reset value_get
	fill_array(value_get, NONE_STATE, bulk->num_lines);

	//Set value of GPIOs (Turn on Led on cp210x dk-board)
	req = gpiod_line_set_value_bulk(bulk, value_set);
	if (req) {
		printf("Func: gpiod_line_set_value_bulk error %d\n", req);
		return -1;
	}
	display_status_lines(bulk->num_lines, value_set, WRITE_LINE);

	//Read GPIOs value again.
	req = gpiod_line_get_value_bulk(bulk, value_get);
	if (req) {
		printf("Func: gpiod_line_get_value_bulk erro %d\n", req);
		return -1;
	}
	display_status_lines(bulk->num_lines, value_get, READ_LINE);
	gpiod_line_release_bulk(bulk);
	gpiod_chip_close(chip);
	free(bulk);
	return 0;
}
