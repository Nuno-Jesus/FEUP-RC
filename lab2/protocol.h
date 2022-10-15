#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "alarm.h"
#include "port.h"

int llopen(char *, Device);

int llclose(int, Device);

int llwrite(int , char *, int);

int llread(int , char *);

#endif