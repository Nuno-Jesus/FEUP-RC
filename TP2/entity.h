#ifndef LINK_H
#define LINK_H

#include "utils.h"

typedef struct entity_t
{
	char *user;		//Username
	char *password;	//User's password
	char *hostname;
	char *path;
	char *ip;		//Converted IP address of the host
	size_t port;	//Port to be calculated after requesting passive mode
}Entity;

/**
 * @brief Creates a new entity instance based on the parameters received
 * @param user The username
 * @param password The password of the user
 * @param hostname The hostname
 * @param path The path of the file to transfer
 * @return An entity pointer on success, NULL otherwise
 */
Entity *entity_new (char *user, char *password, char *hostname, char *path);

/**
 * @brief Deletes the memory of Entity structure
 * @param args The Entity structure to delete
 */
void entity_delete(Entity *args);

/**
 * @brief Prints the Entity strucures in a friendly way
 * @param args The Entity structure to print
 */
void entity_print(Entity *args);

#endif