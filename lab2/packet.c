#include "packet.h"

Packet *new_packet(unsigned char control)
{
	Packet *packet = (Packet *)malloc(sizeof(Packet));
	if (!packet)
		return NULL;

	packet->control = control;
	packet->size = 0;
	packet->frame = NULL;

	return packet;
}

void delete_packet(Packet *packet)
{
	if (packet->frame)
		free(packet->frame);
	free(packet);
}

Packet *assemble_data_packet(unsigned char *data, int size)
{
	return NULL;
}

Packet *assemble_control_packet(char *filename, PacketControl control)
{
	/* int framesize;
	Packet *packet;
	unsigned long filesize;
	unsigned char *fileSizeBytes;

	if (!(packet = new_packet(control)))
		return NULL;

	filesize = get_file_size(filename);
	fileSizeBytes = tobytes(filesize);
	framesize = 2 + strlen((char *)fileSizeBytes) + 2 + strlen(filename);

	if (!(packet->frame = (char *)malloc(framesize)))
	{
		delete_packet(packet);
		return NULL;
	}

	packet->frame[0] = control;
	packet->frame[1] = strlen((char *)fileSizeBytes);
	

	return packet; */

	return NULL;
}