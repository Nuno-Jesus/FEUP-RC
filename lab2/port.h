#ifndef PORT_H
#define PORT_H

#include "utils.h"
#include "state_machine.h"

typedef struct port_t
{
	int fd;
	char *name;
	struct termios oldtio;
	struct termios newtio;
} PortSettings;

PortSettings *new_port(char *, int);

void delete_port(PortSettings *);

int canonical_open(char *);

int canonical_close(int);

int send_supervision_frame(Frame);

int receive_supervision_frame(Device, Frame);

unsigned char *assemble_supervision_frame(Frame);

#endif