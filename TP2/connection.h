#ifndef CONNECTION_H
#define CONNECTION_H

#include "macros.h"
#include "entity.h"
#include "utils.h"
#include "str.h"

int		socket_open(Entity *url, int port);

int		socket_close(int fd);

void	send_command(char *command, char *arg, int fd, int port);

char	*read_response(int fd);

int		login(int fd, Entity* url);

int		passive_mode(int fd, Entity *link);

int	request_file(int fd, Entity *link);

int		download(Entity *link, char *filename, int ftp_fd);

#endif