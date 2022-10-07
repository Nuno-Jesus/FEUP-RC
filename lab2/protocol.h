#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "state_machine.h"

typedef struct port_t
{
	char *name;
	struct termios oldtio;
	struct termios newtio;
} PortInfo;

PortInfo *new_port(const char *);

void delete_port(PortInfo *);

int llopen(const char *, Device );

int llclose(int );

int llwrite(int , char *, int );

int llread(int , char *);

#endif