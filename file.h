#ifndef FILE_H
#define FILE_H

#include "utils.h"

/**
 * @brief Get the File Size
 *
 * @param filename The file's name
 * @return Number of bytes in the file
 */
unsigned long get_file_size(char *);

/**
 * @brief Given a filename, gets its content
 *
 * @param filename The file's name
 * @return buffer with file content
 */
char* get_file_content(char *);

#endif