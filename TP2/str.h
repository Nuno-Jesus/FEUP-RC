#ifndef STR_H
#define STR_H

#include "macros.h"
#include "utils.h"

char	*strtrim(char const *s1, char const *set);

char	*get_line(int fd);

char	*strmap(char const *s, char (*f)(unsigned int, char));

void	delete_matrix(char **mat);

char	**split(char const *s, char c);

char	*substr(char const *s, unsigned int start, size_t len);


#endif