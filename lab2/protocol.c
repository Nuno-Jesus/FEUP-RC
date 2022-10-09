#include "protocol.h"

PortInfo *port;
Alarm *a;

PortInfo *new_port(const char *name, int fd)
{
	PortInfo *port = (PortInfo *)malloc(sizeof(PortInfo));
	if (!port)
		return NULL;

	port->fd = fd;
	port->name = name;
	memset(&port->oldtio, 0, sizeof(port->oldtio));
	memset(&port->newtio, 0, sizeof(port->newtio));
	
	return port;
}

void delete_port(PortInfo *port)
{
	free(port->name);
	free(port);
}

unsigned char *get_open_frame(Device device)
{
	unsigned char *frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!frame)
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	frame[2] = device == TRANSMITTER ? CONTROL_SET : CONTROL_UA;
	frame[3] = device == TRANSMITTER ? BCC_SET : BCC_UA;
	frame[4] = FLAG;

	return frame;
}

int canonical_open(const char *portname)
{
	int fd;
    PortInfo *port;

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
	// Restore the old port settings
	delete_port(port);

    if (tcsetattr(fd, TCSANOW, &port->oldtio) == -1)
   		print_error("tcsetattr()");

    close(fd);
}

int send_supervision_frame(Device device)
{
	ssize_t bytes;
	unsigned char *frame = get_open_frame(device);

	bytes = write(port->fd, frame, 5);
	free(frame);
	
	bytes >= 0 
		? printf("Sending supervision frame\n") 
		: printf("Couldn't send the supervision frame\n");
	return bytes;
}

int receive_supervision_frame(Device device)
{
	StateMachine* machine = new_state_machine(TRANSMITTER);
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



void alarm_handler(int signal)
{
	#ifdef DEBUG
		printf("Timeout #%d occured.\n", a->counter);
	#endif

	a->counter++;
	alarm(a->timeout);
}

/**
	 * @brief 
	 * 1 - Send the SET frame
	 * 2 - Wait for the UA frame
	 * 3 - Check for timeouts when receiveing or sending the frames
	 * 4 - Check for maximum number of attempts
	 */
int llopen_transmitter()
{
	// Setup the alarm structure and the alarm itself
	if (!(a = new_alarm(alarm_handler, TIMEOUT)))
		return 0;

	set_alarm(a);
    start_alarm(a);

	// Begin transmission of supervision frame
	do
	{
		if (send_supervision_frame(TRANSMITTER) != 1)
			return 0;

		if (receive_supervision_frame(TRANSMITTER))
		{
			stop_alarm();
			break;
		}
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen_receiver()
{
	// Setup the alarm structure and the alarm itself
	if (!(a = new_alarm(alarm_handler, TIMEOUT)))
		return 0;

	set_alarm(a);
    start_alarm(a);

	// Begin transmission of supervision frame
	do
	{
		if (send_supervision_frame(RECEIVER) != 1)
			return 0;

		if (receive_supervision_frame(RECEIVER))
		{
			stop_alarm();
			break;
		}
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen(const char *port, Device device)
{
	int fd;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	//NEED TO: Check for errors in each llopen helper
    if (device == RECEIVER)
		llopen_receiver();
	else 
		llopen_transmitter();

	return fd;
}

/**
 * @brief 
 * 
 * 1 - Send the DISC frame to the receiver
 * 2 - Read the DISC frame from the receiver 
 * 3 - Send the UA frame to the receiver
 * 4 - Reset the port configurations
 */
int llclose(int fd)
{
	return canonical_close(fd);
}

int llwrite(int fd, char *buffer, int length)
{
	return -1;
}

int llread(int fd, char *buffer)
{
	return -1;
}