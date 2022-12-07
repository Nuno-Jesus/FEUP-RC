#include "URL.h"

URL *url_new (char *user, char *password, char *hostname, char *path)
{
	URL* url = malloc(sizeof(URL));
	if (!url)
		return NULL;
	url->user = user;
	url->password = password;
	url->hostname = hostname;
	url->path = path;

	return url;
}