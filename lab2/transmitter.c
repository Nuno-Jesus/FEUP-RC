#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "macros.h"
#include "utils.h"
#include "protocol.h"

int main(int argc, char *argv[])
{   
	int fd;
	if ((fd = llopen(TRANSMITTER_PORT, TRANSMITTER)) == -1)
		printf("Couldn't salute receiver\n");

	if ((fd = llclose(fd, TRANSMITTER)) == -1)
		printf("Couldn't close transmitter connection\n");

	printf("Gracefully ending.\n");

    return 0;
}
