#include "link.h"

Link *link_new (char *user, char *password, char *hostname, char *path)
{
	Link* link = malloc(sizeof(Link));
	if (!link)
		return NULL;
	link->user = user;
	link->password = password;
	link->hostname = hostname;
	link->path = path;
	link->port = 0;

	return link;
}

void link_delete(Link *url)
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

void link_print(Link *url)
{
	if (!url)
		return ;

	printf("User: %s\n", url->user);
	printf("Password: %s\n", url->password);
	printf("Hostname: %s\n", url->hostname);
	printf("Path: %s\n", url->path);
	printf("IP: %s\n", url->ip);
}