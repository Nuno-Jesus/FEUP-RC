#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

void print_usage(char *);

void print_error(char *);

void print_frame(unsigned char *, size_t);

int tobytes(unsigned long, unsigned char *);

unsigned char *strrev(unsigned char *, int);

unsigned char get_bcc2(unsigned char *, unsigned long);

/**
 * @brief Applies byte stuffing to the data packet of the frame
 * @param frame Frame to undergo byte stuffing
 * @param size Size of the data packet to process
 * @return Size of the frame after suffering byte stuffing
 */
int stuff_information_frame(unsigned char *, int);

/**
 * @brief Applies byte de-stuffing to the data packet of the frame
 * @param frame Frame to undergo byte de-stuffing
 * @param size Size of the data packet to process
 * @return Size of the frame after suffering byte de-stuffing
 */
int unstuff_information_frame(unsigned char *, int);

#endif