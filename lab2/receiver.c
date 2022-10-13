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
    if (argc < 2)
		print_usage(argv[0]);
    
	int fd;
	if ((fd = llopen(RECEIVER_PORT, RECEIVER)) == -1)
		printf("Couldn't salute receiver\n");

    return 0;
}