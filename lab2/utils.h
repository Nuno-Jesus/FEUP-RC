#ifndef UTILS_H
#define UTILS_H

#include "macros.h"

void print_usage(char *);

void print_error(char *);

void print_frame(unsigned char *, size_t);

unsigned char *tobytes(unsigned long n);

unsigned char *strrev(unsigned char *str);

#endif