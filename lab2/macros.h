#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FLAG 0x7E
#define ADDRESS 0x03
#define CONTROL_SET 0x03
#define CONTROL_UA 0x07
#define BCC_SET CONTROL_SET ^ ADDRESS
#define BCC_UA CONTROL_UA ^ ADDRESS
#define BCC(c, a) c ^ a

#define TIMEOUT 5
#define ATTEMPTS 3

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

#endif