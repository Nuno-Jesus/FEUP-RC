#ifndef LINK_H
#define LINK_H

#include "utils.h"

typedef struct link_t
{
	char *user;
	char *password;
	char *hostname;
	char *path;
	char *ip;
	size_t port;
}Link;

Link *link_new (char *user, char *password, char *hostname, char *path);

void link_delete(Link *args);

void link_print(Link *args);

#endif