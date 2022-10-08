#include "protocol.h"

PortInfo *port;

PortInfo *new_port(const char *name)
{
	PortInfo *port = (PortInfo *)malloc(sizeof(PortInfo));
	if (!port)
		return NULL;

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
	frame[2] = device == RECEIVER ? CONTROL_SET : CONTROL_UA;
	frame[3] = device == RECEIVER ? BCC_SET : BCC_UA;
	frame[4] = FLAG;

	return frame;
}

int canonical_open(const char *portname)
{
	int fd;
    PortInfo *port;

	if ((fd = open(portname, O_RDWR | O_NOCTTY)) < 0)
		print_error(portname);

	port = new_port(portname);
	if (port == NULL)
		print_error("malloc()");

    // Save current port settings
    if (tcgetattr(fd, &port->oldtio) == -1)
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
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &port->newtio) == -1)
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

int sendFrame(int fd, unsigned char *frame, size_t n)
{
	return write(fd, frame, n);
}

/**
	 * @brief 
	 * 1 - Send the SET frame
	 * 2 - Wait for the UA frame
	 * 3 - Check for timeouts when receiveing or sending the frames
	 * 4 - Check for maximum number of attempts
	 */
int llopenTransmitter(int fd)
{
	unsigned char *setFrame;

	if (!(setFrame = get_open_frame(TRANSMITTER)))
		return -1;

	if (sendFrame(fd, setFrame, 5) == -1)
		return -1;

	/**
	 * @brief 
	 * Install alarm with TIMEOUT seconds
	 * Pass an alarm handler to increase the number of attempts to send the frame to the port
	 * While the number of attempts isn't reached:
	 * 		Attempt to receive the response (UA);
	 * 		If the number of attempts increases:
	 * 			Initiate SET retransmission
	 * When reaching ATTEMPTS, return -1;
	 */

	
}

int llopenReceiver(int fd)
{
	return -1;
}

int llopen(const char *port, Device device)
{
	int fd;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	//NEED TO: Check for errors in each llopen helper
    if (device == RECEIVER)
		llopenReceiver(fd);
	else 
		llopenTransmitter(fd);

	return fd;
}

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