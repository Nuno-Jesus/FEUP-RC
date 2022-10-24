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

unsigned char get_bcc2(unsigned char *data, unsigned long size)
{
	if (!data || !size)
		return 0;

	unsigned char res = data[0];
	
	for(unsigned long i = 1; i < size; i++)
		res = res ^ data[i];

	return res;
}

unsigned char *stuff_information_frame(unsigned char *frame)
{
	return NULL;
}

unsigned char *unstuff_information_frame(unsigned char *frame)
{
	return NULL;
}

/* 
unsigned long get_packet_size(Packet *packet)
{	
	if (packet->control == 1)
		return 4 + packet->dataSize;
	else
		return 1 + packet->dataSize;
}

unsigned char *assemble_data_packet(unsigned char *data, int size)
{
	return NULL;
}

unsigned char *assemble_control_packet(char *filename, PacketControl control)
{
	int framesize;
	Packet *packet;
	unsigned long filesize;
	unsigned char *fileSizeBytes;

	if (!(packet = new_packet(control)))
		return NULL;

	filesize = get_file_size(filename);
	fileSizeBytes = tobytes(filesize);
	framesize = 2 + strlen((char *)fileSizeBytes) + 2 + strlen(filename);

	if (!(packet->frame = (char *)malloc(framesize)))
	{
		delete_packet(packet);
		return NULL;
	}

	packet->frame[0] = control;
	packet->frame[1] = strlen((char *)fileSizeBytes);
	

	return packet;

	return NULL;
}


unsigned char *assemble_information_frame()
{
	unsigned char *str;
	unsigned long framesize;
	unsigned long packetsize;

	packetsize = get_packet_size();
	framesize = 6 + packetsize;

	if(!(str = (char *)malloc(framesize)))
		return NULL;
	
	str[0] = FLAG;
	str[1] = ADDRESS;
	str[2] = frame->control;

	for(unsigned long i = 3; i < packetsize; i++)
		str[i] = frame->packet->data[i - 3];
	
	str[3 + packetsize] = frame->bcc2;
	str[4 + packetsize] = FLAG; 

	return str;
}

unsigned long get_frame_size()
{
	return 6 + get_packet_size();
}

int send_frame()
{
	unsigned char *array;
	int bytes;

	if(!(array = assemble_information_frame()))
		return -1;

	if((bytes = write(port->fd, array, get_frame_size(frame))) == -1)
		return -1;

	return bytes;
}
 */