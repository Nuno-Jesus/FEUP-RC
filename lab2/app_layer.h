#ifndef APP_LAYER_H
#define APP_LAYER_H

#include "utils.h"
#include "file.h"
#include "link_layer.h"

/**
 * @brief Sends a given file through a serial port
 * 
 * @param portname The serial port to be used 
 * @param filename The file to be opened and sent
 * @return Returns 0 if an error occurred, 1 otherwise
 */
int send_file(char *, char *);


/**
 * @brief 
 * @param packet Contains the data packet to be resolved
 * @param sequence_number Will contain sequence number of the packet.
 * @param data Will contain packet data field
 * @return 1 on sucess, 0 otherwise
 */
int resolve_data_packet(unsigned char *, int *, unsigned char *);


/**
 * @brief 
 * 
 * @param packet Contains the control packet to be resolved
 * @param filename Will contain the name of the file.
 * @param fileSize Will contain the size of the file
 * @return 1 on sucess, 0 otherwise 
 */
int resolve_control_packet(unsigned char *, unsigned char *, unsigned char *);

#endif