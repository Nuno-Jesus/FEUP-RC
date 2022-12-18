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

void send_command(char *command, char *arg, int fd)
{
	char buffer[64];

	memset(buffer, 0, 64);
	sprintf(buffer, "%s %s\n", command, arg);
	printf("> Sending command \'%s %s\'\n", command, arg);

	write(fd, buffer, strlen(buffer));
}


int read_response(int fd)
{
	char	*line;
	int		code;

	while (1)
	{	
		line = get_line(fd);
		if (!line)
			break;
		#ifdef DEBUG
			printf("%s", line);
		#endif

		if (line[3] == ' ')
		{
			code = atoi(line);
			free(line);
			break;
		}
		free(line);
	}
	return code;
}


int request_login(int fd, Link* url)
{
	int	code;

	send_command("USER", url->user, fd);
	code = read_response(fd);

	send_command("PASS", url->password, fd);
	code = read_response(fd);

	return code;
}

int request_passive_mode(int fd, Link *link)
{	
	int code = 0;

	send_command("PASV", "", fd);

	//Get the response line
	char *line = get_line(fd);

	//Use a new string to retrive the (xxx, xxx, xxx,...) part of the string
	char *tmp = strtrim(strchr(line, '('), "().\n\r");

	//Split the numbers by the ','
	char **tokens = split(tmp, ',');
	
	link->port = atoi(tokens[4]) * 256 + atoi(tokens[5]);
	#ifdef DEBUG
		printf("Port: %ld\n", link->port);
	#endif

	delete_matrix(tokens);
	free(tmp);
	free(line);

	return code;
}

size_t request_file(int fd, Link *link)
{
	size_t filesize = 0;
	
	send_command("RETR", link->path, fd);

	char *line = get_line(fd);
	filesize = atoi(strchr(line, '(') + 1);

	return filesize;
}

int receive_file(int fd, char *filename, size_t filesize)
{
	char *line = malloc(filesize);
	int fd2 = open(filename, O_WRONLY | O_CREAT);

	read(fd, line, filesize);
	write(fd2, line, filesize);
	close(fd2);
	return 1;
}