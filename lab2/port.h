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
} Port;

Port *new_port(char *, int);

void delete_port(Port *);

int canonical_open(char *);

int canonical_close(int);

int send_supervision_frame(FrameControl);

int receive_supervision_frame(Device, FrameControl);

int send_information_frame(unsigned char *, int);

int receive_information_frame(Device, FrameControl);

unsigned char *assemble_supervision_frame(FrameControl);

#endif