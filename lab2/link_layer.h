#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include "alarm.h"
#include "port.h"

int llopen(char *, Device);

int llclose(int, Device);

int llwrite(int, char *, int);

int llread(int, char *);

int llopen_transmitter();

int llopen_receiver();

int llclose_transmitter();

int llclose_receiver();

#endif