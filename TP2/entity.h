#ifndef LINK_H
#define LINK_H

#include "utils.h"

typedef struct entity_t
{
	char *user;
	char *password;
	char *hostname;
	char *path;
	char *ip;
	size_t port;
}Entity;

Entity *entity_new (char *user, char *password, char *hostname, char *path);

void entity_delete(Entity *args);

void entity_print(Entity *args);

#endif