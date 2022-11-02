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

int check_file_diff(char *filename1, char *filename2){
    char *file1 = get_file_content("pinguim.gif");
	char *file2 = get_file_content("received_pinguim.gif");

    for (int i = 0; i < strlen(file1); i++){
        if (file1[i]!= file2[i]){
            printf("*** Found a different byte at position %d\n", i);
            printf("\t\t Byte at file 1 is %c n", file1[i]);
            printf("\t\t Byte at file 2 is %c n", file2[i]);
            return 0;
        }
    }

    printf("Files '%s' and '%s' are an exact match.\n", filename1, filename2);

    return 1;
}