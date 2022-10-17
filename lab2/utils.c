#include "utils.h"

void print_usage(char *command)
{
	printf(
		"Incorrect program usage\n Usage: %s <SerialPort>\n"
		"Example: %s /dev/ttyS1\n", command, command
	);
    exit(1);
}

void print_error(char *message)
{
	perror(message);
	exit(-1);
}

void print_frame(unsigned char *data, size_t n)
{
	for(size_t i = 0; i < n; i++)
		printf("%ld - 0x%x\n", i, data[i]);
	printf("\n");
}

unsigned char *tobytes(unsigned long n)
{	
	int size = 0;
	unsigned char *res = NULL;

	while (n > 0)
	{
		res = realloc(res, ++size);
		res[size - 1] = n % 256;
		n /= 256;
	}

	res = realloc(res, ++size);
	res[size - 1] = '\0';
	return strrev(res);
}

unsigned char *strrev(unsigned char *str)
{
	int len;
	char aux;

	len = strlen((char*)str);
	for(int i = 0; i < len / 2; i++)
	{
		aux = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = aux;
	}
	return (str);
}
