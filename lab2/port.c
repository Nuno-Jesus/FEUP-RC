#include "port.h"

PortSettings *port;

PortSettings *new_port(char *name, int fd)
{
	PortSettings *port = (PortSettings *)malloc(sizeof(PortSettings));
	if (!port)
		return NULL;

	port->fd = fd;
	port->name = name;
	memset(&port->oldtio, 0, sizeof(port->oldtio));
	memset(&port->newtio, 0, sizeof(port->newtio));
	
	return port;
}

void delete_port(PortSettings *port)
{
	
	//free(port->name);
	free(port);
}

int canonical_open(char *portname)
{
	int fd;

	if ((fd = open(portname, O_RDWR | O_NOCTTY)) < 0)
		print_error(portname);

	port = new_port(portname, fd);
	if (port == NULL)
		print_error("malloc()");

    // Save current port settings
    if (tcgetattr(port->fd, &port->oldtio) == -1)
	{
		delete_port(port);
   		print_error("tcgetattr()");
	}

    // Clear struct for new port settings
    memset(&port->newtio, 0, sizeof(port->newtio));

    port->newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    port->newtio.c_iflag = IGNPAR;
    port->newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    port->newtio.c_lflag = 0;
    port->newtio.c_cc[VTIME] = 1;
    port->newtio.c_cc[VMIN] = 0;  

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(port->fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(port->fd, TCSANOW, &port->newtio) == -1)
		print_error("tcsetattr()");

    printf("New termios structure set\n");
	return fd;
}

int canonical_close(int fd)
{
	// Restore the port->oldtio port settings
    if (tcsetattr(fd, TCSANOW, &port->oldtio) == -1)
   		print_error("tcsetattr()");

	delete_port(port);
    close(fd);

	return 1;
}

int send_supervision_frame(FrameControl field)
{
	ssize_t bytes;
	unsigned char *frame = assemble_supervision_frame(field);

	bytes = write(port->fd, frame, 5);
	free(frame);
	
	bytes >= 0 
		? printf("Sending supervision frame\n") 
		: printf("Couldn't send the supervision frame\n");
	return bytes;
}

int receive_supervision_frame(Device device, FrameControl field)
{
	StateMachine* machine = new_state_machine(device, field);
	if (!machine)
		return 0;

	while (machine->state != END)
	{
		if (!read(port->fd, &machine->byte, 1))
			return 0;

		state_machine_multiplexer(machine);
	}

	return 1;
}

unsigned char *assemble_supervision_frame(FrameControl field)
{
	unsigned char *frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!frame)
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	//Apply switch to further cases of other supervision frames
	switch(field)
	{
		case SET:
			frame[2] = CONTROL_SET;
			frame[3] = BCC_SET;
			break;
		case UA:
			frame[2] = CONTROL_UA;
			frame[3] = BCC_UA;
			break;
		case DISC:
			frame[2] = CONTROL_DISC;
			frame[3] = BCC_DISC;
			break;
	}
	frame[4] = FLAG;

	return frame;
}
