#include "protocol.h"

PortInfo *port;

PortInfo *new_port(const char *name)
{
	PortInfo port = (PortInfo *)malloc(sizeof(PortInfo));
	if (!port)
		return NULL;

	port->name = name;
	memset(port->oldtio, 0, sizeof(port->oldtio));
	memset(port->newtio, 0, sizeof(port->newtio));
	
	return port;
}

void delete_port(PortInfo *port)
{
	free(port->name);
	free(port);
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
   		print_error("tcgetattr()");

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
    if (tcsetattr(fd, TCSANOW, &port->oldtio) == -1)
   		print_error("tcsetattr()");

    close(fd);
}

int llopen(const char *port, Device device)
{
	int fd;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	/**
	 * Set an alarm to define a timeout
	 * Create the SET frame
	 * Send the SET frame
	 * Wait for the UA frame
	 * Read for the UA frame
	 * If timeout occurs, retransmit
	 * If the number of retransmissions is reached, abort
	 */

    
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