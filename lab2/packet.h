#ifndef PACKET_H
#define PACKET_H

#include "utils.h"

typedef struct packet_t
{
	PacketControl control;
	unsigned char *frame;
} Packet;

Packet *new_packet(unsigned char);

void delete_packet(Packet *);

Packet *assemble_data_packet(unsigned char *, int);

Packet *assemble_control_packet(char *, PacketControl);

#endif