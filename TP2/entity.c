#include "entity.h"

Entity *entity_new (char *user, char *password, char *hostname, char *path)
{
	Entity* entity = malloc(sizeof(Entity));
	if (!entity)
		return NULL;
	entity->user = user;
	entity->password = password;
	entity->hostname = hostname;
	entity->path = path;
	entity->port = 0;

	return entity;
}

void entity_delete(Entity *url)
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

void entity_print(Entity *url)
{
	if (!url)
		return ;

	printf("User: %s\n", url->user);
	printf("Password: %s\n", url->password);
	printf("Hostname: %s\n", url->hostname);
	printf("Path: %s\n", url->path);
	printf("IP: %s\n", url->ip);
}