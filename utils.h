#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

/**
 * @brief Prints an error message if the usage of the program was incorrect
 * 
 * @param command The name of the executable to print in the console
 */
void print_usage(char *);

/**
 * @brief Prints an error on the console and aborts the program
 * 
 * @param message The message to display on the console
 */
void print_error(char *);

/**
 * @brief Prints an hexadecimal buffer of a given size
 * 
 * @param data The buffer to print 
 * @param n The data size
 * 
 */
void print_frame(unsigned char *, size_t);

/**
 * @brief Converts a number into a string by consecutively by dividing by 256
 * 
 * @param long The number to convert
 * @return The size of the newly string
 */
int tobytes(unsigned long, unsigned char *);

/**
 * @brief Reverses a string
 * 
 * @param str The string to revert
 * @param len The length of str
 * @return Returns its own parameter, str
 */
unsigned char *strrev(unsigned char *, int);

/**
 * @brief Compute the bcc2 of the data field
 * 
 * @param data Data field to perform the calculation on 
 * @param size Size of the data field
 * @return The BCC2 result 
 */
unsigned char get_bcc2(unsigned char *, unsigned long);

/**
 * @brief Applies byte stuffing to the data packet of the frame
 * @param frame Frame to undergo byte stuffing
 * @param size Size of the data packet to process
 * @return Size of the frame after suffering byte stuffing
 */
unsigned char* stuff_information_frame(unsigned char *, int *);

/**
 * @brief Applies byte de-stuffing to the data packet of the frame
 * @param frame Frame to undergo byte de-stuffing
 * @param size Size of the data packet to process
 * @return Size of the frame after suffering byte de-stuffing
 */
unsigned char* unstuff_information_frame(unsigned char *, int *);

#endif