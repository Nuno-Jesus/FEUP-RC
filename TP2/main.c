#include "download.h"
#include "URL.h"

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
		return '/';
	return c;
}

//! URL Regular Expression: ftp://[<user>:<password>@]<host>/<url-path>

void parse_url(char *link)
{
	if (strstr(link, "ftp://") != link)
		print_error("parse_url", "missing \"ftp://\" at the beggining of the url");

	char *temp = strmap(link + 6, &to_bar);
	//puts(temp);
	//free(temp);

	char **tokens = split(temp, '/');
	for (int i = 0; tokens[i]; i++)
		puts(tokens[i]);
	
	URL *url;

	if (strchr(link + 6, '@') && strchr(link + 6, ':'))
		url = url_new(tokens[0], tokens[1], tokens[2], link + 6 + strlen(tokens[0]) + strlen(tokens[1]) + strlen(tokens[2]));
	else
		url = url_new("anonymous", "dummy", tokens[0], link + 6 + strlen(tokens[0]));

	/* IDEA>
		Replace the @ and : for / so that you can split all the tokens equally and retrieve 
		the right ones in the right place.
	  */
	//puts("Got it.");
}

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	parse_url(argv[1]);
}