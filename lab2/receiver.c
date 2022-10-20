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
#include "link_layer.h"

int main(int argc, char *argv[])
{
    /* if (argc < 2)
		print_usage(argv[0]);
    
	int fd;
	if ((fd = llopen(RECEIVER_PORT, RECEIVER)) == -1)
		printf("Couldn't salute transmitter\n");
	
	if ((fd = llclose(fd, RECEIVER)) == -1)
		printf("Couldn't close receiver connection\n");
 	*/
	
	if (!receive_file(TRANSMITTER_PORT, argv[1]))
		print_error("receive_file(): error\n");

	printf("Gracefully ending.\n");
    return 0;
}