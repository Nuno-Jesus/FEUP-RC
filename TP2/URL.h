#ifndef URL_H
#define URL_H

#include "utils.h"

typedef struct url_t
{
	char *user;
	char *password;
	char *hostname;
	char *path;
}URL;

URL *url_new (char *user, char *password, char *hostname, char *path);

#endif