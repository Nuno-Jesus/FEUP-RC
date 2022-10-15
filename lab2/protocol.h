#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "state_machine.h"
#include "alarm.h"
#include "utils.h"

typedef struct port_t
{
	int fd;
	char *name;
	struct termios oldtio;
	struct termios newtio;
} PortSettings;

PortSettings *new_port(char *, int);

void delete_port(PortSettings *);

int llopen(char *, Device);

int llclose(int, Device);

int llwrite(int , char *, int);

int llread(int , char *);

#endif