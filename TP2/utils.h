#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char	**split(char const *s, char c);

char	*substr(char const *s, unsigned int start, size_t len);

char	*strmap(char const *s, char (*f)(unsigned int, char));

#endif