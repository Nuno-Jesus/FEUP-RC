#ifndef APP_LAYER_H
#define APP_LAYER_H

#include "utils.h"
#include "file.h"
#include "link_layer.h"
#include "macros.h"
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>


typedef struct app_layer_t {
    int fd;
    Device device;
} AppLayer;

/**
 * @brief Sends a given file through a serial port
 *
 * @param portname The serial port to be used
 * @param filename The file to be opened and sent
 * @return Returns 0 if an error occurred, 1 otherwise
 */
int send_file(char *, char *);

/**
 * @brief Reads a file through the specified serial port
 * @param portname The serial port to be used
 * @return 1 on success, 0 otherwise
 */
int receive_file(char* );

/**
 * @brief Given a data packet, resolves its content
 * @param packet Contains the data packet to be resolved
 * @param sequence_number Will contain sequence number of the packet.
 * @param data Will contain packet data field
 * @return 1 on sucess, 0 otherwise
 */
int resolve_data_packet(unsigned char *, int *, unsigned char *);


/**
 * @brief Given a control packet, resolves its content
 * @param packet Contains the control packet to be resolved
 * @param fileSize Will contain the size of the file
 * @param filename Will contain the name of the file.
 * @return 1 on sucess, 0 otherwise
 */
int resolve_control_packet(unsigned char *packet, int*, char*);

/**
 * @brief Assembles a control packet given the used parameters to be used both at the end
 * and start of the serial communication. 
 * 
 * @param packetSize A pointer that returns the packet final size
 * @param control A enum stating which control packet to build (START or END)
 * @param filename Name of the file
 * @param filesize Size of the file
 * @return Control packet 
 */
unsigned char *assemble_control_packet(int *packetSize, PacketControl control, char *filename, int filesize);

/**
 * @brief Assembles an information packet to encapsulate the file data.
 * @param packetSize A pointer that returns the packet final size
 * @param data The data chunk to encapsulate
 * @param dataSize The data size
 * @param sequenceNumber The number of the packet (mod 256)
 * @return Data packet
 */
unsigned char *assemble_data_packet(int *packetSize, unsigned char *data, int dataSize, int sequenceNumber)

/**
 * @brief Creates a new AppLayer to store either Transmitter or Receiver information
 * @param device The device the application layer is about
 * @return A pointer to the new AppLayer structure
 */
AppLayer *new_app_layer(Device device);


#endif