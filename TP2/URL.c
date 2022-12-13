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

void url_delete(URL *url)
{
	if (!url)
		return ;
	if (url->user)
		free(url->user);
	if (url->password)
		free(url->password);
	if (url->hostname)
		free(url->hostname);
	if (url->path)
		free(url->path);
	free(url);
}

void url_print(URL *url)
{
	if (!url)
		return ;

	printf("User: %s\n", url->user);
	printf("Password: %s\n", url->password);
	printf("Hostname: %s\n", url->hostname);
	printf("Path: %s\n", url->path);
	printf("IP: %s\n", url->ip);
}