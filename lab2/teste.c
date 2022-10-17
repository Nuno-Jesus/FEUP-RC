#include "file.h"
#include "utils.h"

int main()
{
	unsigned long filesize = get_file_size("transmitter.c");
	unsigned char *res = tobytes(filesize);
	printf("Filesize: %lu\n", filesize);
	
	for(size_t i = 0; i < strlen((char*) res); i++)
		printf("%lu - 0x%X\n", i, res[i]);
}