#include "protocol.h"

PortInfo *port;
Alarm *a;

PortInfo *new_port(char *name, int fd)
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
	
	//free(port->name);
	free(port);
}

unsigned char *assemble_frame(Frame f)
{
	unsigned char *frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!frame)
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	//Apply switch to further cases of other supervision frames
	switch(f)
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

int send_supervision_frame(Frame f)
{
	ssize_t bytes;
	unsigned char *frame = assemble_frame(f);

	bytes = write(port->fd, frame, 5);
	free(frame);
	
	bytes >= 0 
		? printf("Sending supervision frame\n") 
		: printf("Couldn't send the supervision frame\n");
	return bytes;
}

int receive_supervision_frame(Device device, Frame frame)
{
	StateMachine* machine = new_state_machine(device, frame);
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

int llopen_transmitter()
{
	if (!send_supervision_frame(SET))
		return 0;
	

    start_alarm(a);

	do
	{
		printf("Sending SET frame.\n");
		if (receive_supervision_frame(TRANSMITTER, UA))
		{
			printf("REceived UA frame.\n");
			stop_alarm();
			break;
		}
		else
			if (!send_supervision_frame(SET))
				return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen_receiver()
{
	if (!receive_supervision_frame(RECEIVER, SET))
		return 0;

    start_alarm(a);

	do
	{
		printf("Received SET frame.\n");
		if (send_supervision_frame(UA))
		{
			printf("Sending UA frame.\n");
			stop_alarm();
			break;
		}
		else
			if (!receive_supervision_frame(RECEIVER, SET))
				return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen(char *port, Device device)
{
	int fd;
	int ret;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	if (!(a = new_alarm(alarm_handler, TIMEOUT)))
		return 0;

	set_alarm(a);

	ret = device == TRANSMITTER ? llopen_transmitter() : llopen_receiver();
	if (ret == 0)
	{
		canonical_close(fd);
		return -1;
	}
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


int llclose_receiver()
{
    if (!receive_supervision_frame(RECEIVER,DISC))
        return 0;

	start_alarm(a);
    
	do
    {
        printf("Sending DISC frame.\n");
        if (send_supervision_frame(DISC))
        {
            printf("Received DISC frame.\n");
            stop_alarm();
            break;
        }
        else
            if (!receive_supervision_frame(RECEIVER,DISC))
                return 0;
        // In case of a timeout when reading the UA frame, a new alarm is setted up
        // and a->counter is incremented. It pretty much works like calling the handler
        // at the end of a send/receive pair
    } while(a->counter < MAXTRANSMISSIONS);


    return 1;
}

int llclose_transmitter()
{
    if (!send_supervision_frame(DISC))
        return 0;
    
    start_alarm(a);

    do
    {
        printf("Sending DISC frame.\n");
        if (receive_supervision_frame(TRANSMITTER, DISC))
        {
            printf("Received DISC frame.\n");
            stop_alarm();
            break;
        }
        else
            if (!send_supervision_frame(DISC))
                return 0;
        // In case of a timeout when reading the UA frame, a new alarm is setted up
        // and a->counter is incremented. It pretty much works like calling the handler
        // at the end of a send/receive pair
    } while(a->counter < MAXTRANSMISSIONS);

    if(!send_supervision_frame(UA))
        return 0;
	printf("Sending UA frame.");
    return 1;
    
}

int llclose(int fd, Device device)
{    
    int ret;

    //set_alarm(a);

    ret = device == TRANSMITTER ? llclose_transmitter() : llclose_receiver();

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