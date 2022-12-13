#include "download.h"
#include "URL.h"

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

char	*parse_hostname(char *hostname)
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

int	socket_open(URL *url)
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

URL *parse_url(char *link)
{
	if (strstr(link, "ftp://") != link)
		print_error("parse_url", "missing \"ftp://\" at the beggining of the url");

	char *dup = strmap(link + 6, &to_bar);
	char *temp = strchr(dup , '/');
	temp[0] = DELIMITER;
	char **tokens = split(dup, DELIMITER);
	URL *url;

	if (strchr(link + 6, '@') && strchr(link + 6, ':'))
		url = url_new(strdup(tokens[0]), strdup(tokens[1]), strdup(tokens[2]), strdup(tokens[3]));
	else
		url = url_new(strdup("anonymous"), strdup("dummy"), strdup(tokens[0]), strdup(tokens[1]));

	delete_matrix(tokens);
	free(dup);

	return url;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	URL *url;
	int sockfd;

	if (!(url = parse_url(argv[1])))
	{
		perror("malloc()");
		exit(EXIT_FAILURE);
	}
	
	if (!(url->ip = parse_hostname(url->hostname)))
	{
		url_delete(url);
		print_error("parse_hostname", "unknown hostname");
	}

	#ifdef DEBUG
		url_print(url); 
	#endif

	if ((sockfd = socket_open(url)) < 0)
	{
		url_delete(url);
		print_error("socket_open", "Couldn't establish connection");
	}

	printf("File descriptor = %d\n", sockfd);

	if ((sockfd = socket_close(sockfd)) < 0)
	{
		url_delete(url);
		print_error("socket_close", "Couldn't close connection");
	}
	
}