#ifndef PACKET_H
#define PACKET_H

#include "utils.h"
#include "file.h"

typedef struct packet_t
{
	PacketControl control;
	unsigned long dataSize;
	unsigned char *data;
} Packet;

Packet *new_packet(unsigned char);

void delete_packet(Packet *);

unsigned long get_packet_size(Packet *);

Packet *assemble_data_packet(unsigned char *, int);

Packet *assemble_control_packet(char *, PacketControl);

#endif