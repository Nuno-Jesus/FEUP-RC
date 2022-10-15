#include "packet.h"

Packet *new_packet(unsigned char control)
{
	Packet *packet = (Packet *)malloc(sizeof(Packet));
	if (!packet)
		return NULL;

	packet->control = control;
	packet->frame = NULL;

	return packet;
}

void delete_packet(Packet *packet)
{
	free(packet->frame);
	free(packet);
}

Packet *assemble_data_packet(unsigned char *data, int size)
{
	return NULL;
}

Packet *assemble_control_packet(char *filename, PacketControl control)
{
	int filesize;
	int framesize;
	Packet *packet;
	struct stat buf;

	if (!(packet = new_packet(control)))
		return NULL;

	if ((stat(filename, &buf)) == -1)
		return NULL;

	filesize = buf.st_size;

	return packet;
}