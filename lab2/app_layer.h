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

#endif