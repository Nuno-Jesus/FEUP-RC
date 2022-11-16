#include <stdio.h>
#include <stdlib.h>
#include "file.h"

char *get_file_content(char *filename)
{
    char *buffer;

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error opening file.\n");
        return NULL;
    }

    unsigned long fileSize = get_file_size(filename);

    buffer = (char *)malloc(fileSize * sizeof(char));
    if (buffer == NULL)
    {
        printf("Error while allocating memory for file content.\n");
        return NULL;
    }

    fread(buffer, sizeof(char), fileSize, fp);
    fclose(fp);

    return buffer;
}

unsigned long get_file_size(char *filename)
{
	struct stat buf;

    if ((stat(filename, &buf)) == -1)
		return 0;

	return (unsigned long) buf.st_size;
}