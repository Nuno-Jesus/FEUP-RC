#include "utils.h"

void print_usage(char *command)
{
	printf(
		"Incorrect program usage\n Usage: %s <SerialPort>\n"
		"Example: %s /dev/ttyS1\n", command, command
	);
    exit(1);
}

void print_error(char *message)
{
	perror(message);
	exit(-1);
}
