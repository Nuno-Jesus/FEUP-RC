#ifndef CONNECTION_H
#define CONNECTION_H

#include "macros.h"
#include "link.h"
#include "utils.h"
#include "str.h"

int		socket_open(Link *url, int port);

int		socket_close(int fd);

void	send_command(char *command, char *arg, int fd, int port);

char	*read_response(int fd);

int		login(int fd, Link* url);

int		passive_mode(int fd, Link *link);

int	request_file(int fd, Link *link);

int		download(Link *link, char *filename, int ftp_fd);

#endif