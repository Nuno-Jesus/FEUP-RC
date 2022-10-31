#include "app_layer.h"

AppLayer *app;

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

int resolve_control_packet(unsigned char *packet, int *filesize, char *filename)
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

		for (int i = 0; i < l2; i++)
			filename[i] = packet[v2Start + i];
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

	// Count the time
	struct timeval start, end;

	int filesize, bytesRead, bitsRead = 0;
	char filename[MAX_FILENAME_SIZE];

	unsigned char buf[MAX_DATA], data[MAX_DATA];

	// start counting time
	gettimeofday(&start, NULL);

	// Read from the serial port using llread()
	if ((bytesRead = llread(app->fd, (char *)buf)) < 0)
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
	while(1)
	{
		// Read from the serial port using llread()
		if ((bytesRead = llread(app->fd, (char *)buf) < 0))
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
			if(((int)fwrite(data, sizeof(unsigned char), bytesRead, filePtr) < bytesRead))
				return -1;
		}

		else if (buf[0] == END_PACKET)
			break;
	}

	gettimeofday(&end, NULL);

	//float time_taken = end.tv_sec - start.tv_sec;

	/* Imprimir as estatisticas*/

	// check if file size matches
	if (filesize != (int)get_file_size("received_pinguim.gif"))
		return 0;

	int filesizeAtEnd;
	char filenameAtEnd[MAX_FILENAME_SIZE];

	// Parse the last packet received
	if (!resolve_control_packet(buf, &filesizeAtEnd, filenameAtEnd))
		return 0;

	// check if info on start packet matches info on end packet
	if (filesizeAtEnd != filesize || filenameAtEnd != filename)
		return 0;

	if (!llclose(app->fd, app->device))
		return 0;
}

unsigned char* assemble_control_packet(int *packetSize, PacketControl control, char *filename, int filesize)
{
	int len;
	unsigned char *res;
	unsigned char *filesizeStr = (unsigned char *)malloc(0);

	len = tobytes(filesize, filesizeStr) - 1;
	*packetSize = 5 + strlen(filename) + len;
	if(!(res = (unsigned char *) malloc(*packetSize * sizeof(char))))
		return 0;

	res[0] = control;
	res[1] = FILESIZE;
	res[2] = (unsigned char) len;
	memcpy(res + 3, filesizeStr, len);

	res[3 + len] = FILENAME;
	res[3 + len + 1] = (unsigned char) strlen(filename);
	memcpy(res + (3 + len + 2), filename, strlen(filename));

	return res;
}

int assemble_data_packet(unsigned char *p, unsigned char* data, int dataSize, int sequenceNumber)
{
	int packetSize = 4 + dataSize;

	if(!(p = (unsigned char *)malloc(packetSize * sizeof(char))))
		return 0;

	p[0] = DATA_PACKET;
	p[1] = sequenceNumber;
	p[2] = dataSize / 256;
	p[3] = dataSize % 256;
	memcpy(p + 4, data, dataSize);

	return packetSize;
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

	if (llwrite(app->fd, packet, packetSize) == -1)
	{
		canonical_close(app->fd);
		return 0;
	}

	free(packet);

	int seqNum = 0;
	for(int i = 0; i < fileSize; i = i + MAX_DATA)
	{
		if (i + MAX_DATA > fileSize)
			packetSize = fileSize % MAX_DATA;
		else
			packetSize = MAX_DATA;

		if (!(packetSize = assemble_data_packet((unsigned char *)packet,(unsigned char *) file + i, packetSize, seqNum)))
			return 0;

		if (llwrite(app->fd, packet, packetSize) == -1)
		{
			canonical_close(app->fd);
			return 0;
		}

		free(packet);
		seqNum = (seqNum + 1) % 256;
	}

	if (!(packet = assemble_control_packet(&packetSize, END_PACKET, filename, fileSize)))
		return 0;

	llwrite(app->fd, packet, packetSize);

	if (llclose(app->fd, app->device) == -1)
		return 0;

	return 1;
}

AppLayer *new_app_layer(Device device)
{
	AppLayer* app = (AppLayer *)malloc(sizeof(AppLayer));
	if(!app)
		return NULL;

	app->fd = 0;
	app->device = device;

	return app;
}