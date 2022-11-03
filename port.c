#include "port.h"

Port *port;
extern LinkLayer *ll;

Port *new_port(char *name, int fd)
{
	Port *port = (Port *)malloc(sizeof(Port));
	if (!port)
		return NULL;

	port->fd = fd;
	port->name = name;
	memset(&port->oldtio, 0, sizeof(port->oldtio));
	memset(&port->newtio, 0, sizeof(port->newtio));

	return port;
}

void delete_port(Port *port)
{
	// free(port->name);
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

	#ifdef DEBUG
		printf("\n\tSENDING FRAME %d\n\n", field);
		print_frame(frame, 5);
	#endif

	bytes = write(port->fd, frame, 5);
	free(frame);

	return bytes;
}

int receive_supervision_frame(Device device, FrameControl field)
{
	StateMachine *machine = new_state_machine(device, field, FALSE);
	if (!machine)
		return 0;

	while (machine->state != END)
	{

		if (!read(port->fd, &machine->byte, 1))
			return 0;

		state_machine_multiplexer(machine);
	}

	delete_state_machine(machine);
	return 1;
}

int send_information_frame(unsigned char *frame, int size)
{
	if (!write(port->fd, frame, size))
		return 0;
	
	return 1;
}

int receive_information_frame(Device device)
{
	StateMachine *machine = new_state_machine(device, SET, TRUE);
	if (!machine)
		return 0;

	State previousState = START;

	while (machine->state != END)
	{
		if (!read(port->fd, &machine->byte, 1))
			return 0;

		state_machine_multiplexer(machine);

		// save the byte and increment the size of the frame
		if (machine->state != previousState || machine->state == WAIT_END_FLAG)
		{
			ll->frame = realloc(ll->frame, ++ll->frameSize);
			ll->frame[ll->frameSize - 1] = machine->byte;
			previousState = machine->state;
		}
	}

	// print_frame(ll->frame, ll->frameSize);
	delete_state_machine(machine);
	return 1;
}

unsigned char *assemble_supervision_frame(FrameControl field)
{
	unsigned char *frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!frame)
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	// Apply switch to further cases of other supervision frames
	switch (field)
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
		case REJ00:
			frame[2] = CONTROL_REJ(0);
			frame[3] = BCC_REJ(0);
			break;
		case REJ01:
			frame[2] = CONTROL_REJ(1);
			frame[3] = BCC_REJ(1);
			break;
		case RR00:
			frame[2] = CONTROL_RR(0);
			frame[3] = BCC_RR(0);
			break;
		case RR01:
			frame[2] = CONTROL_RR(1);
			frame[3] = BCC_RR(1);
			break;
		}
	frame[4] = FLAG;

	return frame;
}