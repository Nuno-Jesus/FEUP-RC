#include <stdio.h>

/**
 * @brief Get the File Size
 *
 * @param filename The file's name
 * @return Number of bytes in the file
 */
unsigned long get_file_size(unsigned char *);

/**
 * @brief Given a filename, gets its content
 *
 * @param filename The file's name
 * @return buffer with file content
 */
char* get_file_content(unsigned char* );