#include "app_layer.h"

int send_file(char *portname, char *filename)
{
	int fd;
	char *content;

	if ((fd = llopen(portname, TRANSMITTER)) == -1)
		return 0;

	if (!(content = get_file_content(filename)))
		return 0;

	/**
	 * 
	 * Assemble and send the start control packet
	 * 
	 * While there is content from the file to be sent:
	 * 	-> Assemble a data packet with control = 1 and
	 * 		a frame of size DATA_SIZE
	 * 	-> Send the data packet through ll_write()
	 * 
	 * Assemble and send the end control packet
	 * 
	 */

	if (llclose(fd, TRANSMITTER) == -1)
		return 0;

}