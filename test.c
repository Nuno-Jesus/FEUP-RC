#include "utils.h"

int main(int argc, char **argv)
{
	int num = atoi(argv[argc - 1]);
	unsigned char *res = (unsigned char *)malloc(0);
	int size = tobytes(num, res);

	printf("Number\n");
	for(int i = 0; i < size - 1; i++)
		printf("%d - 0x%02X\n", i, res[i]);
}