#include "utils.h"

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

char *get_filename(char *path)
{
	char *filename;

	filename = strrchr(path, '/');
	filename = filename == NULL ? path : filename + 1;

	return filename;
}	