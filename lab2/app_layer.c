#include "app_layer.h"


int resolve_data_packet(unsigned char* packet, int* sequence_number, unsigned char* packet_data){
	// Check the control field
	if (packet[0] != DATA_PACKET)
		return 0;

	// Get the sequence number
	*sequence_number = packet[1];

	/* Compute the number of octets (K) in 
	the data field K = 256 * L2 * L1 */
	int size = 256 * packet[2] + packet[3];

	// Data field of the packet (size is K)
	for (int i = 4; i < size; i++){
		packet_data[i - 4] = packet[size];
	}
	
	return 1;
}

int resolve_control_packet(unsigned char *packet, unsigned char *filename, unsigned char *fileSize)
{
	*fileSize = 0;
	
	// Check the control field
	if (packet[0] != START_PACKET && packet[1] != END_PACKET)
		return 0;

	// Check the type
	if (packet[1] == FILESIZE){
		// L -> Check the length of the V field size 
		int v_size = packet[2];

		// Parse the file size
		for (int i = 0; i < v_size; i++){
			*fileSize = *fileSize 
		}
	}
	


	return 1;
}

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