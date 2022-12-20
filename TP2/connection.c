#include "connection.h"

int	socket_open(Link *url, int port)
{
	int fd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(url->ip); 
    server_addr.sin_port = htons(port);     

    /*open a TCP socket*/
    if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
        perror("socket()");
		return -1;
    }

    /*connect to the server*/
    if (connect(fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
	{
        perror("connect()");
		return -1;
    }

	return fd;
}

int socket_close(int fd)
{
	if (close(fd) < 0)
	{
        perror("close()");
		return -1;
    }

	return 1;
}

void send_command(char *command, char *arg, int fd, int port)
{
	char buffer[64];

	memset(buffer, 0, 64);
	sprintf(buffer, "%s%s\n", command, arg);
	printf("\n> Sending command %s\'%s%s\'%s to port %s%d%s\n", 
		BYELLOW, command, arg, RESET, BYELLOW, port, RESET);

	write(fd, buffer, strlen(buffer));
}


char *read_response(int fd)
{
	char	*line;

	while (1)
	{	
		line = get_line(fd);
		if (!line)
			break;

		printf("%s", line);
		if (line[3] == ' ')
			break;
		free(line);
	}
	return line;
}

int login(int fd, Link* url)
{
	char *response;

	send_command("USER ", url->user, fd, FTP_PORT);
	response = read_response(fd);
	if (!strstr(response, "Please specify the password"))
	{
		printf("\n\t%sUnknown user%s\n\n", BRED, RESET);
		free(response);
		return 0;
	}
	free(response);
	
	send_command("PASS ", url->password, fd, FTP_PORT);
	response = read_response(fd);
	if (atoi(response) != CODE_LOGIN_SUCCESSFUL)
	{
		printf("\n\t%sWrong password%s\n\n", BRED, RESET);
		free(response);
		return 0;
	}
	free(response);
	return 1;
}

int passive_mode(int fd, Link *link)
{	
	send_command("PASV", "", fd, FTP_PORT);

	//Get the response line
	char *line = get_line(fd);
	printf("%s", line);

	if (atoi(line) == CODE_WRONG_FILE)
	{
		free(line);
		printf("\n\t%sNo such file.%s\n\n", BRED, RESET);
		return 0;
	}

	//Use a new string to retrive the (xxx, xxx, xxx,...) part of the string
	char *tmp = strtrim(strchr(line, '('), "().\n\r");

	//Split the numbers by the ','
	char **tokens = split(tmp, ',');
	
	link->port = atoi(tokens[4]) * 256 + atoi(tokens[5]);

	delete_matrix(tokens);
	free(tmp);
	free(line);

	return 1;
}

size_t request_file(int fd, Link *link)
{
	size_t filesize = 0;
	char *line;
	
	send_command("TYPE ", "I", fd, link->port);

	line = get_line(fd);
	printf("%s", line);

	send_command("RETR ", link->path, fd, link->port);

	line = get_line(fd);
	printf("%s", line);
	
	filesize = atoi(strchr(line, '(') + 1);
	free(line);

	return filesize;
}

int download(Link *link, char *filename, int ftp_fd)
{
	int filefd;
	int sockfd;
	char *line;
	size_t filesize;

	//######################### OPEN THE SOCKET TO TRANSFER THE FILE #########################

	if ((sockfd = socket_open(link, link->port)) < 0)
	{
		link_delete(link);
		print_error("socket_open", "Couldn't establish connection");
	}

	filesize = request_file(ftp_fd, link);

	filefd = open(filename, O_WRONLY | O_CREAT);
	if (!(line = malloc(READ_MAX + 1)))
	{
		close(filefd);
		return (0);
	}

	printf("\n> Dowloading %s\'%s\'%s...", BYELLOW, filename, RESET);
	for (size_t i = 0; i < filesize; i += READ_MAX)
	{
		ssize_t bytes = read(sockfd, line, READ_MAX);
		printf("%s", line);
		if (bytes == -1)
			break;
		line[bytes] = 0;
		write(filefd, line, bytes);
	}
	printf("\n> %sDownload complete.%s\n", BCYAN, RESET);

	//################################### CLOSE THE SOCKET ###################################

	free(line);
	close(filefd);

	if ((sockfd = socket_close(sockfd)) < 0)
	{
		link_delete(link);
		print_error("socket_close", "Couldn't close connection");
	}

	return 1;
}