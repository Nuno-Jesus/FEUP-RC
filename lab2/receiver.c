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
#include "app_layer.h"

int main(int argc, char *argv[])
{
	if (!receive_file(TRANSMITTER_PORT, argv[1]))
		print_error("receive_file(): error\n");

	printf("Gracefully ending.\n");
    return 0;
}