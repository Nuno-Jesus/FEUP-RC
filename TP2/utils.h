#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

/**
 * @brief In case of wrong inputted parameters, it prints an error message
 * @param command The executable to print the command
 */
void	print_usage(char *command);

/**
 * @brief It outputs an error message, based on an error that ocurred inside a function
 * @param func The function where the error occured on
 * @param message The message to print
 */
void	print_error(char *func, char *message);

/**
 * @brief A utilitary function used on a strmap function that returns a '/' if the char passed
 * as argument is either a '@' or a ':'
 * @param i The index of the character
 * @param c The char of the original string to be analysed
 * @return A '/' if the condition is met, c otherwise
 */
char	to_bar(unsigned int i, char c);

/**
 * @brief Given a path to the file to be transfered, this function returns a pointer to the 
 * filename contained on the path.
 * @param path The path to be parsed
 * @return A pointer to the filename, NULL otherwise
 */
char	*get_filename(char *path);

#endif