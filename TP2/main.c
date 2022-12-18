#include "utils.h"
#include "str.h"
#include "link.h"

//! URL Regular Expression: ftp://[<user>:<password>@]<host>/<url-path>

char	*getip(char *hostname)
{
	struct hostent *h;
	char *ip;

	if (!(h = gethostbyname(hostname))) 
	{
        perror("gethostbyname()");
        return NULL;
    }
	
	ip = inet_ntoa(*((struct in_addr *) h->h_addr));
	return ip;
}

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

Link *parse_url(char *link)
{
	if (strstr(link, "ftp://") != link)
		print_error("parse_url", "missing \"ftp://\" at the beggining of the url");

	char *dup = strmap(link + 6, &to_bar);
	char *temp = strchr(dup , '/');
	temp[0] = DELIMITER;
	char **tokens = split(dup, DELIMITER);
	Link *args;

	if (strchr(link + 6, '@') && strchr(link + 6, ':'))
		args = link_new(strdup(tokens[0]), strdup(tokens[1]), strdup(tokens[2]), strdup(tokens[3]));
	else
		args = link_new(strdup("anonymous"), strdup("dummy"), strdup(tokens[0]), strdup(tokens[1]));

	delete_matrix(tokens);
	free(dup);

	return args;
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

void send_command(char *command, char *arg, int fd)
{
	char buffer[64];

	memset(buffer, 0, 64);
	sprintf(buffer, "%s %s\n", command, arg);
	printf("> Sending command \'%s %s\'\n", command, arg);

	write(fd, buffer, strlen(buffer));
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
	
	#ifdef DEBUG
		printf("Line: %s", line);
		printf("Filesize: %ld\n", filesize);
	#endif

	return filesize;
}

char *get_filename(Link *link)
{
	char *filename;

	filename = strrchr(link->path, '/');
	filename = filename == NULL ? link->path : filename + 1;

	return filename;
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

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	Link *link;
	int fd;

	if (!(link = parse_url(argv[1])))
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	
	if (!(link->ip = getip(link->hostname)))
	{
		link_delete(link);
		print_error("parse_hostname", "unknown hostname");
	}

	link_print(link); 

	if ((fd = socket_open(link, 21)) < 0)
	{
		link_delete(link);
		print_error("socket_open", "Couldn't establish connection");
	}
	

	printf("\n\t_/=\\_/=\\_/=\\_ Connection established. _/=\\_/=\\_/=\\_\n\n");
	
	int code = read_response(fd);
	if (code != CODE_SERVICE_READY)
	{
		link_delete(link);
		print_error("read_response", "Response code was not 220 (success code).");
	}

	request_login(fd, link);
	request_passive_mode(fd, link);

	//############# OPEN THE SOCKET TO TRANSFER THE FILE #########################

	int fd2;

	if ((fd2 = socket_open(link, link->port)) < 0)
	{
		link_delete(link);
		print_error("socket_open", "Couldn't establish connection");
	}

	size_t filesize = request_file(fd, link);
	char *filename = get_filename(link);
	
	#ifdef DEBUG
		puts(filename);
	#endif
	receive_file(fd2, filename, filesize);

	if ((fd2 = socket_close(fd2)) < 0)
	{
		link_delete(link);
		print_error("socket_close", "Couldn't close connection");
	}

	//#######################################################

	if ((fd = socket_close(fd)) < 0)
	{
		link_delete(link);
		print_error("socket_close", "Couldn't close connection");
	}

	printf("\n\t_/=\\_/=\\_/=\\_ Connection closed. _/=\\_/=\\_/=\\_\n\n");
	link_delete(link);
}