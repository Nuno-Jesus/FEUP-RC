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

Entity *parse_url(char *link)
{
	regex_t regex;
	regcomp(&regex, "ftp://.+:.+@.+/.+", REG_EXTENDED);

	int status = regexec(&regex, link, 0, NULL, 0);
	if (status == REG_NOMATCH)
	{
		printf("\n\t%sValid URL format: "
			"ftp://<user>:<password>@<host>/<url-path>%s\n\n", BRED, RESET);
		regfree(&regex);
		return (NULL);
	}
	
	char *dup = strmap(link + 6, &to_bar);
	char *temp = strchr(dup , '/');
	temp[0] = DELIMITER;
	char **tokens = split(dup, DELIMITER);
	Entity *args;

	args = entity_new(strdup(tokens[0]), strdup(tokens[1]), strdup(tokens[2]), strdup(tokens[3]));
	if (args)
		args->ip = getip(args->hostname);
	
	regfree(&regex);
	delete_matrix(tokens);
	free(dup);

	return args;
}

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	int fd;
	Entity *link;
	char *response;

	if (!(link = parse_url(argv[1])))
		exit(EXIT_FAILURE);
	
	entity_print(link); 

	if ((fd = socket_open(link->ip, FTP_PORT)) < 0)
	{
		entity_delete(link);
		exit(EXIT_FAILURE);
	}
	
	printf("\n\t%s_/=\\_/=\\_/=\\_ Connection established. _/=\\_/=\\_/=\\_%s\n\n", BYELLOW, RESET);
	
	response = read_response(fd);
	int code = atoi(response);
	free(response);

	if (code != CODE_SERVICE_READY)
	{
		entity_delete(link);
		exit(EXIT_FAILURE);
	}

	if (!login(fd, link))
	{
		entity_delete(link);
		exit(EXIT_FAILURE);
	}

	if (!passive_mode(fd, link))
	{
		entity_delete(link);
		exit(EXIT_FAILURE);
	}

	download(link, get_filename(link->path), fd);

	entity_delete(link);

	if ((fd = socket_close(fd)) < 0)
		exit(EXIT_FAILURE);

	printf("\n\t%s_/=\\_/=\\_/=\\_ Connection closed. _/=\\_/=\\_/=\\_%s\n\n", BYELLOW, RESET);
}