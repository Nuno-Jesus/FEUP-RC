#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

void	print_usage(char *command);

void	print_error(char *func, char *message);

char	to_bar(unsigned int i, char c);

char	*get_filename(char *path);

#endif