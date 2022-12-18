#include "connection.h"

//! URL Regular Expression: ftp://[<user>:<password>@]<host>/<url-path>

char *getip(char *hostname)
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

	if (args)
		args->ip = getip(args->hostname);
	delete_matrix(tokens);
	free(dup);

	return args;
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
	
	link_print(link); 

	if ((fd = socket_open(link, FTP_PORT)) < 0)
	{
		link_delete(link);
		print_error("socket_open", "Couldn't establish connection");
	}
	
	printf("\n\t%s_/=\\_/=\\_/=\\_ Connection established. _/=\\_/=\\_/=\\_%s\n\n", BYELLOW, RESET);
	
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
	char *filename = get_filename(link->path);
	
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

	printf("\n\t%s_/=\\_/=\\_/=\\_ Connection closed. _/=\\_/=\\_/=\\_%s\n\n", BYELLOW, RESET);
	link_delete(link);
}