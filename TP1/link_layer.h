#ifndef LINK_LAYER_H
#define LINK_LAYER_H

#include "utils.h"
#include "alarm.h"
#include "port.h"

// To create a global variable
typedef struct link_layer_t
{
	unsigned char *frame;
	int frameSize;
	unsigned char sequenceNumber;
} LinkLayer;

/**
 * @brief Given a device and a port, redirects the program flow to open the serial communication
 * on the transmitter or receiver side
 * @param portname The port to be used
 * @param device The device where the port should be opened at
 * @return 1 on success, 0 otherwise
 */
int llopen(char *, Device);

/**
 * @brief Closes the serial communication on a given device
 * @param fd The file descriptor of the serial port to close
 * @param device The device where the port should be closed at
 * @return 1 on success, 0 otherwise
 */
int llclose(int, Device);

/**
 * @brief Function that sends data to the serial port
 * @param fd Connection Identifier
 * @param buffer Array of characters to send
 * @return If the writing was sucessful, return the number of characters written, otherwise return a negative value
 */
int llwrite(int, char *, int);

/**
 * @brief Function that reads data from the serial port
 * @param fd Connection Identifier
 * @param buffer Array of characters where received data will be stored
 * @return If the read was sucessful, return the number of characters read, otherwise return a negative value
 */
int llread(int, char *);

/**
 * @brief Establishes the serial communication on the transmitter side 
 * @return 1 on sucess, 0 otherwise 
  */
int llopen_transmitter();

/**
 * @brief Establishes the serial communication on the receiver side
 * @return 1 on sucess, 0 otherwise 
 */
int llopen_receiver();

/**
 * @brief Closes the serial communication on the transmitter side
 * @return 1 on sucess, 0 otherwise  
 */
int llclose_transmitter();

/**
 * @brief Closes the serial communication on the receiver side
 * @return 1 on sucess, 0 otherwise 
 */
int llclose_receiver();

/**
 * @brief Creates a new LinkLayer struct
 * @return On sucess, the new Link Layer struct, NULL otherwise
 */
LinkLayer *new_link_layer();

#endif