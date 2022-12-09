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

void parse_url(char *link)
{
	char *temp;
	char *dup;
	char **tokens;
	
	if (strstr(link, "ftp://") != link)
		print_error("parse_url", "missing \"ftp://\" at the beggining of the url");

	dup = strmap(link + 6, &to_bar);
	temp = strchr(dup , '/');
	temp[0] = DELIMITER;
	tokens = split(dup, DELIMITER);

	URL *url;

	if (strchr(link + 6, '@') && strchr(link + 6, ':'))
		url = url_new(strdup(tokens[0]), strdup(tokens[1]), strdup(tokens[2]), strdup(tokens[3]));
	else
		url = url_new(strdup("anonymous"), strdup("dummy"), strdup(tokens[0]), strdup(tokens[1]));

	#ifdef DEBUG
		url_print(url); 
	#endif

	delete_matrix(tokens);
	url_delete(url);
	free(dup);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	parse_url(argv[1]);
}