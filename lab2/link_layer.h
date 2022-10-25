#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include "utils.h"
#include "alarm.h"
#include "port.h"

// To create a global variable
typedef struct link_layer_t
{
	unsigned char *frame;
	int frameSize;
	unsigned char sequenceNumber;
} LinkLayer;

int llopen(char *, Device);

int llclose(int, Device);

int llwrite(int, char *, int);

/**
 * @brief Function that reads data from the serial port
 * @param fd Connection Identifier
 * @param buffer Array of characters where received data will be stored
 * @return If the read was sucessful, return the number of characters read, otherwise return a negative value
 */
int llread(int, char *);

int llopen_transmitter();

int llopen_receiver();

int llclose_transmitter();

int llclose_receiver();

LinkLayer *new_link_layer();

#endif