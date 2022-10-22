#include "app_layer.h"

struct AppLayer appLayer;

int resolve_data_packet(unsigned char *packet, int *sequence_number, unsigned char *packet_data)
{
	// Check the control field
	if (packet[0] != DATA_PACKET)
		return 0;

	// Get the sequence number
	*sequence_number = packet[1];

	/* Compute the number of octets (K) in
	the data field K = 256 * L2 * L1 */
	int size = 256 * packet[2] + packet[3];

	// Data field of the packet (size is K)
	for (int i = 4; i < size; i++)
	{
		packet_data[i - 4] = packet[size];
	}

	return 1;
}

int resolve_control_packet(unsigned char *packet, int *filesize, unsigned char *filename)
{
	*filesize = 0;

	// Check the control field
	if (packet[0] != START_PACKET && packet[1] != END_PACKET)
		return 0;

	int t1, t2, l1, l2;

	// Read the parameter
	t1 = packet[1];

	// Check the parameter
	if (t1 == FILESIZE)
	{
		// L -> Check the length of the V field size
		l1 = packet[2];

		// Parse the file size
		for (int i = 0; i < l1; i++)
			*filesize = packet[i + 3] + filesize * 256;

	}
	else
		return 0;

	/* t2,l2 and v2 fields positions are computed using the formulas below*/
	int t2Pos = l1 + 3;
	int l2Pos = t2Pos + 1;
	int v2Start = l2Pos + 1;

	// Read the parameter
	t2 = packet[t2Pos];

	// Check the parameter
	if (t2 == FILENAME)
	{
		// L -> Check the length of the V field size
		l2 = packet[l2Pos];

		for (int i = 0; i < l2; i++)
			filename[i] = packet[v2Start + i]
	}
	else
		return 0;

	return 1;
}

int receive_file(char *portname)
{
	int fd;
	char *content;

	appLayer.device = RECEIVER;

	if ((appLayer.fd = llopen(portname, TRANSMITTER)) == -1)
		return 0;

	// Count the time
	struct timeval start, end;

	int filesize, bytesRead, bitsRead = 0;
	char filename[MAX_FILENAME_SIZE];

	unsigned char buf[MAX_DATA], data[MAX_DATA];

	// start counting time
	gettimeofday(&start, NULL);

	// Read from the serial port using llread()
	if ((bytesRead = llread(appLayer.fd, buf) < 0)
		return 0;

	bitsRead += bytesRead * 8;

	// if we receive the control packet
	if (buf[0] == START_PACKET)
	{
		// resolve the control packet
		if (!resolve_control_packet(buf, &filesize, filename))
			return 0;
	}
	else
		return 0;

	// create a file to write the received data to it
	FILE *filePtr = fopen(filename"received_pinguim.gif", "w");
	if (!filePtr)
		return 0;

	int expSeqNum = 0, seqNum;

	// Read data packets from transmitter
	while(1)
	{
		// Read from the serial port using llread()
		if ((bytesRead = llread(appLayer.fd, buf) < 0)
			return 0;

		bitsRead += bytesRead * 8;

		if (buf[0] == DATA_PACKET)
		{
			// resolve the received data packet
			if (!resolve_data_packet(buf, &seqNum, data))
				return 0;

			if (seqNum != expSeqNum)
				return 0;

			expSeqNum = (expSeqNum == 0) ? 1 : 0;

			// Real size of data is the packet read minus 4 (C, N, L1 & L2)
			bytesRead -= 4;

			//write to the file
			if((fwrite(data, sizeof unsigned char, bytesRead, filePtr) < bytesRead))
				return -1;
		}

		else if (buf[0] == END_PACKET)
			break;
	}

	gettimeofday(&end, NULL);

	float time_taken = end.tv_sec - start.tv_sec;

	/* Imprimir as estatisticas*/

	// check if file size matches
	if (filesize != get_file_sise(filePtr))
		return 0;

	int filesizeAtEnd;
	char filenameAtEnd[MAX_FILENAME_SIZE];

	// Parse the last packet received
	if (!resolve_control_packet(buf, &filesizeAtEnd, filenameAtEnd))
		return 0;

	// check if info on start packet matches info on end packet
	if (filesizeAtEnd != filesize || filenameAtEnd != filename)
		return 0;

	if (!llclose(fd, RECEIVER))
		return 0;
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