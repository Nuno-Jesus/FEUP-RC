#include "frame.h"

Frame *new_frame(unsigned char control, Packet *packet)
{
	if (!packet)
		return NULL;

	Frame *frame = (Frame *)malloc(sizeof(Frame));
	if (!frame)
		return NULL;

	frame->control = control;
	frame->packet = packet;
	frame->bcc1 = BCC(control, ADDRESS);
	frame->bcc2 = get_bcc2(packet->data, packet->dataSize);

	return NULL;
}

void delete_frame(Frame *frame)
{
	delete_packet(frame->packet);
	free(frame);
}

unsigned char *assemble_information_frame(Frame *frame)
{
	unsigned char *str;
	unsigned long framesize;
	unsigned long packetsize;

	packetsize = get_packet_size(frame->packet);
	framesize = 4 + packetsize + 2;

	if(!(str = (char *)malloc(framesize)))
		return NULL;
	
	str[0] = FLAG;
	str[1] = ADDRESS;
	str[2] = frame->control;

	for(unsigned long i = 3; i < packetsize; i++)
		str[i] = frame->packet->data[i - 3];
	
	str[3 + packetsize] = frame->bcc2;
	str[4 + packetsize] = FLAG; 

	return str;
}