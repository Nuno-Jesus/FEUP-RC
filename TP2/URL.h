#ifndef URL_H
#define URL_H

#include "utils.h"

typedef struct url_t
{
	char *user;
	char *password;
	char *hostname;
	char *path;
	char *ip;
}URL;

URL *url_new (char *user, char *password, char *hostname, char *path);

void url_delete(URL *url);

void url_print(URL *url);

#endif