#include "utils.h"
#include "link.h"

#define DELIMITER '|'

void print_usage(char *command)
{
	printf("Usage: %s URL\n", command);
	exit(EXIT_FAILURE);
}

void print_error(char *func, char *message)
{
	printf("%s(): %s\n", func, message);
	exit(EXIT_FAILURE);
}

char to_bar(unsigned int i, char c)
{
	(void)i;

	if (c == '@' || c == ':')
		return DELIMITER;
	return c;
}

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

int	socket_open(Link *url)
{
	int fd;
    struct sockaddr_in server_addr;

    /*server address handling*/
    memset((char *) &server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(url->ip); 
    server_addr.sin_port = htons(21);     

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

	while ((line = get_line(fd)))
	{	
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

int send_command(char *command, int fd)
{
	if (write(fd, command, strlen(command)) == -1)
		return 0;
	return 1;
}

int request_login(int fd, Link* url)
{
	int	code;
	char command[64];

	memset(command, 0, 64);
	sprintf(command, "user %s\n", url->user);
	send_command(command, fd);
	code = read_response(fd);

	memset(command, 0, 64);
	sprintf(command, "pass %s\n", url->password);
	send_command(command, fd);
	code = read_response(fd);

	return code;
}

int request_passive_mode(int fd, Link *link)
{	
	(void)link;
	char command[64];
	int code;

	memset(command, 0, 64);
	sprintf(command, "pasv\n");
	send_command(command, fd);
	code = read_response(fd);

	return code;
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

	if ((fd = socket_open(link)) < 0)
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
	#ifdef DEBUG
		printf("Code response: %d\n", code);
		printf("File descriptor = %d\n", fd);
	#endif

	request_login(fd, link);
	request_passive_mode(fd, link);

	if ((fd = socket_close(fd)) < 0)
	{
		link_delete(link);
		print_error("socket_close", "Couldn't close connection");
	}

	printf("\n\t_/=\\_/=\\_/=\\_ Connection closed. _/=\\_/=\\_/=\\_\n\n");
	link_delete(link);
}