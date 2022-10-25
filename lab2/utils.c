#include "utils.h"

void print_usage(char *command)
{
	printf(
		"Incorrect program usage\n Usage: %s <SerialPort>\n"
		"Example: %s /dev/ttyS1\n",
		command, command);
	exit(1);
}

void print_error(char *message)
{
	perror(message);
	exit(-1);
}

void print_frame(unsigned char *data, size_t n)
{
	for (size_t i = 0; i < n; i++)
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

	len = strlen((char *)str);
	for (int i = 0; i < len / 2; i++)
	{
		aux = str[i];
		str[i] = str[len - 1 - i];
		str[len - 1 - i] = aux;
	}
	return (str);
}

unsigned char get_bcc2(unsigned char *data, unsigned long size)
{
	if (!data || !size)
		return 0;

	unsigned char res = data[0];

	for (unsigned long i = 1; i < size; i++)
		res = res ^ data[i];

	return res;
}

int stuff_information_frame(unsigned char *frame, int size)
{
	if (!frame || !size)
		return 0;

	unsigned char buf[size + 6]; // 4 bytes for the header + size of the data packet + 2 bytes for the tail

	for (int i = 0; i < size; i++)
	{
		buf[i] = frame[i];
	}

	int dataPosition = 4;

	for (int j = dataPosition; j < (size + 6); j++)
	{
		// If, inside the tram, 0x7e occurs, replace it with 0x7d 0x5e
		if (buf[j] == FLAG && j != (size + 5))
		{
			frame[dataPosition] = ESCAPE;
			frame[dataPosition + 1] = FLAG_STUFFED;
			dataPosition+=2;
		}

		// If, inside the tram, 0x7d occurs, replace it with 0x7d 0x5d
		else if (buf[j] == ESCAPE && j != (size + 5))
		{
			frame[dataPosition] = ESCAPE;
			frame[dataPosition + 1] = ESCAPE_STUFFED;
			dataPosition += 2;
		}
		else
		{
			frame[dataPosition] = buf[j];
			dataPosition++;
		}
	}

	return dataPosition;
}

int unstuff_information_frame(unsigned char *frame, int size)
{
	unsigned char buf[size + 5];	// (data packet + bcc2) + 5 bytes for header and trail

	for (int i = 0; i < (size + 5); i++)
		buf[i] = frame[i];
	
	int dataPosition = 4;

	for (int j = dataPosition; j < (size + 5); j++)
	{
		if (buf[j] == ESCAPE)
		{
			if (buf[j+1] == ESCAPE_STUFFED)
				frame[dataPosition] = ESCAPE;
			
			else if (buf[j+1] == FLAG_STUFFED)
				frame[dataPosition] = FLAG;

			j++;
			dataPosition++;
		}
		else
		{
			frame[dataPosition] = buf[j];
			dataPosition++;
		}
	}

	return dataPosition;
}