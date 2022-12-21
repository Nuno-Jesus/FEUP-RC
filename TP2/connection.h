#ifndef CONNECTION_H
#define CONNECTION_H

#include "macros.h"
#include "entity.h"
#include "utils.h"
#include "str.h"


/**
 * @brief Opens a socket based on an IP and port
 * @param ip The ip to connect to
 * @param port The port to enter on
 * @return 1 on success, 0 otherwise
 */
int		socket_open(char *ip, int port);

/**
 * @brief Closes a previously open socket
 * @param fd The file descriptor of the socket to close
 * @return 1 in success, 0 otherwise
 */
int		socket_close(int fd);

/**
 * @brief Sends an FTP command with the given argument to the 
 * 
 * @param command The command to send
 * @param arg The argument to send attatched to the command
 * @param fd The file descriptor of the open socket
 * @param port (Debugging purposes)
 */
void	send_command(char *command, char *arg, int fd, int port);

/**
 * @brief Reads line by line the response coming from the server, printing it on the
 * screen. It returns a string with the response code and additional information
 * @param fd The file descriptor of the socket to read from 
 * @return A string containing the response code and response string
 */
char	*read_response(int fd);

/**
 * @brief Given a username and password, the login() function sends commands that allow the user to login
 * in the FTP server
 * @param fd The file descriptor of the socket to write the commands on
 * @param url The Entity structure containing the arguments sent by terminal
 * @return 1 on success, 0 otherwise
 */
int		login(int fd, Entity* url);

/**
 * @brief Requests passive mode (connection from client to server) to the server
 * @param fd The file descriptor of the socket to write the commands on
 * @param url The Entity structure containing the arguments sent by terminal
 * @return 1 on success, 0 otherwise 
 */
int		passive_mode(int fd, Entity *link);

/**
 * @brief Sends the necessary commands to start the download process of the file. It won't
 * fetch the file data.
 * @param fd The file descriptor of the socket to write the commands on
 * @param url The Entity structure containing the arguments sent by terminal * 
 * @return 1 on success, 0 otherwise 
 */
int		request_file(int fd, Entity *link);

/**
 * @brief This function fetches the file after requesting its download and creates a new file with the same name of 
 * requested file.
 * @param link The Entity structure containing the arguments sent by terminal
 * @param filename The filename to be downloaded
 * @param ftp_fd The file descriptor of the socket to fetch the file from 
 * @return 1 on success, 0 otherwise
 */
int		download(Entity *link, char *filename, int ftp_fd);

#endif