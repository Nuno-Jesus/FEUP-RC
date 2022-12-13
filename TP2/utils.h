#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

char	**split(char const *s, char c);

char	*get_line(int fd);

char	*substr(char const *s, unsigned int start, size_t len);

char	*strmap(char const *s, char (*f)(unsigned int, char));

void	*delete_matrix(char **mat);

#endif