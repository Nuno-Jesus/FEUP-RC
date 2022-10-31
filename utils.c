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

int tobytes(unsigned long n, unsigned char *buf)
{
	int size = 0;

	while (n > 0)
	{
		buf = realloc(buf, ++size);
		buf[size - 1] = n % 256;
		n /= 256;
	}

	buf = realloc(buf, ++size);
	buf[size - 1] = '\0';
	buf = strrev(buf, size - 1);
	return size;
}

unsigned char *strrev(unsigned char *str, int len)
{
	char aux;

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

	print_frame(data, size);

	unsigned char res = data[0];

	for (unsigned long i = 1; i < size; i++)
		res = res ^ data[i];

	return res;
}

int stuff_information_frame(unsigned char *frame, int size)
{
	if (!frame || !size)
		return 0;

	unsigned char *buf; // 4 bytes for the header + size of the data packet + 2 bytes for the tail
	int newSize = size;

	if (!(buf = (unsigned char *)malloc(size * sizeof(unsigned char))))
		return 0;

	for (int i = 0, j = 0; i < size; i++)
	{
		if (frame[i] == FLAG)
		{
			buf = realloc(buf, ++newSize);
			buf[j] = ESCAPE;
			buf[j + 1] = FLAG_STUFFED;
			j += 2;
		}
		else if (frame[i] == ESCAPE)
		{
			buf = realloc(buf, ++newSize);
			buf[j] = ESCAPE;
			buf[j + 1] = ESCAPE_STUFFED;
			j += 2;
		}
		else
		{
			buf[j] = frame[i];
			j++;
		}
	}

	frame = realloc(frame, newSize);
	memcpy(frame, buf, newSize);
	free(buf);

	return newSize;
}

int unstuff_information_frame(unsigned char *frame, int size)
{
	unsigned char *buf;
	int newSize = size;

	if (!(buf = (unsigned char *)malloc(size)))
		return 0;

	for (int j = 0, i = 0; i < size; j++)
	{
		if (frame[i] == ESCAPE)
		{
			buf = realloc(buf, --newSize);
			if (i + 1 < size && frame[i + 1] == ESCAPE_STUFFED)
			{
				buf[j] = ESCAPE;
				i += 2;
			}
			else if (i + 1 < size && frame[i + 1] == FLAG_STUFFED)
			{
				buf[i] = FLAG;
				i += 2;
			}
			i++;
		}
		else
		{
			buf[j] = frame[i];
			i++;
		}
	}

	frame = realloc(frame, newSize);
	memcpy(frame, buf, newSize);
	free(buf);

	return newSize;
}