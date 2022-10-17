#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include "utils.h"
#include "alarm.h"
#include "port.h"
#include "frame.h"


//To create a global variable
typedef struct link_layer_t
{
	Port *port;
	Frame *frame;
	unsigned int sequenceNumber;
	unsigned int timeout;
	unsigned int maxTransmissions;
} LinkLayer;

int llopen(char *, Device);

int llclose(int, Device);

int llwrite(int, char *, int);

int llread(int, char *);

int llopen_transmitter();

int llopen_receiver();

int llclose_transmitter();

int llclose_receiver();

LinkLayer *new_link_layer();

#endif