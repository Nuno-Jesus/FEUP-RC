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

/**
 * @brief Creates a port struct with a given filename and file descriptor
 * 
 * @param portname The portname
 * @param fd The file descriptor of the serial port
 * @return A new Port struct
 */
Port *new_port(char *, int);

/**
 * @brief Deletes a Port struct pointer
 * @param port The Port struct to delete
 */
void delete_port(Port *);

/**
 * @brief Opens the serial port to later establish the serial connection 
 * 
 * @param portname The serial port to open
 * @return 1 on success, 0 otherwise
 */
int canonical_open(char *);

/**
 * @brief Closes the serial port after serial communication has ended
 * 
 * @param portname The serial port to close
 * @return 1 on sucess, 0 otherwise 
 */
int canonical_close(int);

/**
 * @brief Assembles and sends through the serial port one of the supervision frames,
 * specified in the received parameter.
 * 
 * @param field The supervision frame to send 
 * @return 1 on success, 0 otherwise
 */
int send_supervision_frame(FrameControl);

/**
 * @brief Given an expected supervision frame, it creates a State Machine to ensure the 
 * received frame is the correct one
 * 
 * @param device TRANSMITTER or RECEIVER
 * @param field The supervision frame to expect
 * @return 1 on success, 0 otherwise
 */
int receive_supervision_frame(Device, FrameControl);

/**
 * @brief Sends an information frame of a given size
 * 
 * @param frame The information frame to send
 * @param size The size of the frame
 * @return 1 on sucess, 0 otherwise 
 */
int send_information_frame(unsigned char *, int);

/**
 * @brief Receives an information frame and retrieves the data within the packet
 
 * @param device TRANSMITTER or RECEIVER
 * @return 1 on sucess, 0 otherwise 
 */
int receive_information_frame(Device);

/**
 * @brief Assembles a new supervision frame
 * 
 * @param control A enum stating which type of frame to build
 * @return The assembled supervision frame
 */
unsigned char *assemble_supervision_frame(FrameControl);

/**
 * @brief Assembles a new information frame
 * 
 * @param buffer The data packet to use
 * @param length Used as both the length of the data packet and the length of 
 * the new information frame
 * @return The new information frame buffer
 */
unsigned char *assemble_information_frame(unsigned char *buffer, int *length);

#endif