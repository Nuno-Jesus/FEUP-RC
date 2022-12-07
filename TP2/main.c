#include "download.h"

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

//! URL Regular Expression: ftp://[<user>:<password>@]<host>/<url-path>

void parse_url(char *url)
{
	if (strstr(url, "ftp://") != url)
		print_error("parse_url", "missing \"ftp://\" at the beggining of the url");
	//puts("Got it.");
}

int main(int argc, char **argv)
{
	if (argc != 2)
		print_usage(argv[0]);
	
	parse_url(argv[1]);
}