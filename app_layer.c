#include "app_layer.h"


AppLayer *app;

unsigned char *assemble_control_packet(int *packetSize, PacketControl control, char *filename, int filesize)
{
	int len;
	unsigned char *res;
	unsigned char *filesizeStr = (unsigned char *)malloc(0);

	len = tobytes(filesize, filesizeStr) - 1;
	*packetSize = 5 + strlen(filename) + len;
	if (!(res = (unsigned char *)malloc(*packetSize * sizeof(char))))
		return 0;

	res[0] = control;
	res[1] = FILESIZE;
	res[2] = (unsigned char)len;
	memcpy(res + 3, filesizeStr, len);

	res[3 + len] = FILENAME;
	res[3 + len + 1] = (unsigned char)strlen(filename);
	memcpy(res + (3 + len + 2), filename, strlen(filename));


	return res;
}

unsigned char *assemble_data_packet(int *packetSize, unsigned char *data, int dataSize, int sequenceNumber)
{
	unsigned char *res;

	*packetSize = 4 + dataSize;
	if (!(res = (unsigned char *)malloc(*packetSize * sizeof(char))))
		return 0;

	res[0] = DATA_PACKET;
	res[1] = sequenceNumber;
	res[2] = dataSize / 256;
	res[3] = dataSize % 256;
	memcpy(res + 4, data, dataSize);

	return res;
}

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

	memcpy(packet_data, &packet[4], size);

	return 1;
}

int resolve_control_packet(unsigned char *packet, int *filesize, char *filename)
{
	*filesize = 0;

	// Check the control field
	if (packet[0] != START_PACKET && packet[0] != END_PACKET)
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
			*filesize = packet[i + 3] + *filesize * 256;
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

		for (int i = 0; i < l2; i++){
			filename[i] = packet[v2Start + i];
			//printf("Filename[%d] -> %c\n", i, filename[i]);
		}
		filename[l2] = '\0';
	}
	else
		return 0;

	printf("Received Control Packet\n");
	return 1;
}

int receive_file(char *portname)
{
	if (!(app = new_app_layer(RECEIVER)))
		return 0;

	if ((app->fd = llopen(portname, app->device)) == -1)
		return 0;


	int filesize, bytesRead, bitsRead = 0;
	char filename[MAX_FILENAME_SIZE];

	unsigned char buf[MAX_PACKET_SIZE];
	unsigned char data[MAX_DATA_SIZE];

	// Start measuring time
    struct timeval t1, t2;
    double timeTaken;
	gettimeofday(&t1, NULL);

	// Read from the serial port using llread()
	bytesRead = llread(app->fd, (char *)buf);
	if (bytesRead < 0)
		return 0;

	bitsRead += bytesRead * 8;

	// if we receive the control packet
	if (buf[0] == START_PACKET)
	{
		// resolve the control packet
		if (!resolve_control_packet(buf, &filesize, (char *)filename))
			return 0;
	}
	else
		return 0;

	// create a file to write the received data to it
	FILE *filePtr = fopen("received_pinguim.gif", "w");
	if (!filePtr)
		return 0;

	int expSeqNum = 0, seqNum;

	// Read data packets from transmitter
	int i = 0;
	while (1)
	{
		bytesRead = llread(app->fd, (char *)buf);
		if (bytesRead < 0)
			return 0;

		bitsRead += bytesRead * 8;

		if (buf[0] == DATA_PACKET)
		{
			// resolve the received data packet
			if (!resolve_data_packet(buf, &seqNum, data))
				return 0;

			// Check if the sequence number match
			if (expSeqNum != seqNum)
				return 0;

			// Update the expected sequence number
			expSeqNum = (expSeqNum + 1) % 256;

			// Real size of data is the packet read minus 4 (C, N, L1 & L2)
			bytesRead -= 4;

			// printf("I will write %d bytes to the file\n", bytesRead);

			// write to the file
			if (((int)fwrite(data, 1, bytesRead, filePtr) < bytesRead))
				return 0;
		}

		else if (buf[0] == END_PACKET)
			break;

		i++;
	}

	gettimeofday(&t2, NULL);

	// compute and print the elapsed time in millisec
    timeTaken = (t2.tv_sec - t1.tv_sec);
    timeTaken += (t2.tv_usec - t1.tv_usec) / 1e6;   // us to s

	printf("\n\t\t\t**** STATISTICS ****\n\n");
	printf("\t\tNumber of bits read = %d\n", bitsRead);
	printf("\t\tTime it took to send the file =  %fs\n", timeTaken);

	double R = bitsRead/timeTaken;
	double baudRate = 180000.0;
	double S = R / baudRate;

	printf("\t\tBaudrate = %lf\n", R);
	printf("\t\tS = %lf\n\n", S);


	// check if file sizes match
	/* if (filesize != (int)get_file_size("received_pinguim.gif"))
		return 0; */

	int filesizeAtEnd;
	char filenameAtEnd[MAX_FILENAME_SIZE];

	// Parse the last packet received (end packet)
	if (!resolve_control_packet(buf, &filesizeAtEnd, (char *)filenameAtEnd))
		return 0;

	// check if info on start packet matches info on end packet
	if (filesizeAtEnd != filesize || memcmp(filename, filenameAtEnd, strlen(filename)))
		return 0; 

	// Close the port
	if (!llclose(app->fd, app->device))
		return 0;

	// check the file content
	if (!check_file_diff("pinguim.gif", "received_pinguim.gif"))
		return 0;

	return 1;
}

int send_file(char *portname, char *filename)
{
	int fileSize;
	int packetSize;
	char *file;
	unsigned char *packet;

	if (!(app = new_app_layer(TRANSMITTER)))
		return 0;

	if ((app->fd = llopen(portname, app->device)) == -1)
		return 0;

	if (!(file = get_file_content(filename)))
		return 0;

	fileSize = get_file_size(filename);

	if (!(packet = assemble_control_packet(&packetSize, START_PACKET, filename, fileSize)))
		return 0;

	#ifdef DEBUG
		printf("\n\tAssembled Control Packet\n\n");
		print_frame(packet, packetSize);
	#endif

	//Send start control packet
	if (llwrite(app->fd, packet, packetSize) == -1)
	{
		canonical_close(app->fd);
		return 0;
	}
	free(packet);

	int seqNum = 0;
	for (int i = 0; i < fileSize; i = i + MAX_DATA_SIZE)
	{
		if (i + MAX_DATA_SIZE > fileSize)
			packetSize = fileSize % MAX_DATA_SIZE;
		else
			packetSize = MAX_DATA_SIZE;

		if (!(packet = assemble_data_packet(&packetSize, (unsigned char *)file + i, packetSize, seqNum)))
			return 0;

		if (llwrite(app->fd, packet, packetSize) == -1)
		{
			canonical_close(app->fd);
			return 0;
		}

		free(packet);
		seqNum = (seqNum + 1) % 256;
	}

	//Send end control packet
	if (!(packet = assemble_control_packet(&packetSize, END_PACKET, filename, fileSize)))
		return 0;

	llwrite(app->fd, packet, packetSize);

	if (llclose(app->fd, app->device) == -1)
		return 0;

	return 1;
}

AppLayer *new_app_layer(Device device)
{
	AppLayer *app = (AppLayer *)malloc(sizeof(AppLayer));
	if (!app)
		return NULL;

	app->fd = 0;
	app->device = device;

	return app;
}