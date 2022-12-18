#ifndef CONNECTION_H
#define CONNECTION_H

#include "macros.h"
#include "link.h"
#include "utils.h"
#include "str.h"

int		socket_open(Link *url, int port);

int		socket_close(int fd);

void	send_command(char *command, char *arg, int fd, int port);

int		read_response(int fd);

int		request_login(int fd, Link* url);

int		request_passive_mode(int fd, Link *link);

size_t	request_file(int fd, Link *link);

int		receive_file(int fd, char *filename, size_t filesize);

#endif