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

int request_file(int fd, Link *link)
{
	char *line;
	int code;
	
	send_command("TYPE ", "I", fd, link->port);

	line = read_response(fd);
	code = atoi(line);
	free(line);

	if (code == CODE_FAILURE)
	{
		printf("\n\t%s TYPE: unrecognized mode.%s\n\n", BRED, RESET);
		return 0;
	}

	send_command("RETR ", link->path, fd, link->port);

	line = read_response(fd);
	code = atoi(line);
	free(line);

	if (code == CODE_WRONG_FILE)
	{
		printf("\n\t%sRETR: no such file.%s\n\n", BRED, RESET);
		return 0;
	}

	return 1;
}

int download(Link *link, char *filename, int ftp_fd)
{
	int filefd;
	int sockfd;
	char c;

	//######################### OPEN THE SOCKET TO TRANSFER THE FILE #########################

	if ((sockfd = socket_open(link, link->port)) < 0)
		return 0;

	if (!request_file(ftp_fd, link))
		return 0;

	if ((filefd = open(filename, O_WRONLY | O_CREAT, 0666)) < 0)
	{
		printf("\n\t%sFailed to create '%s' file.%s\n\n", BCYAN, filename, RESET);
		return 0;
	}

	printf("\n> Dowloading %s\'%s\'%s...\n", BYELLOW, filename, RESET);
	
	#ifdef DEBUG
		size_t i = 0;
	#endif

	while (read(sockfd, &c, 1) > 0)
	{
		#ifdef DEBUG
			printf("%ld = '%c' (%#02X)\n", i++, c, c);
		#endif
		write(filefd, &c, 1);
	}
	printf("\n> %sDownload complete.%s\n", BCYAN, RESET);

	//################################### CLOSE THE SOCKET ###################################

	close(filefd);
	if ((sockfd = socket_close(sockfd)) < 0)
		return 0;

	return 1;
}