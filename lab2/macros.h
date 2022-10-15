#ifndef MACROS_H
#define MACROS_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>

typedef enum state_t
{
	START,
	WAIT_ADDRESS,
	WAIT_CONTROL,
	WAIT_BCC,
	WAIT_END_FLAG,
	END
} State;

typedef enum frame_control_t
{
	SET,
	UA,
	RR00,
	RR01,
	REJ00,
	REJ01,
	DISC
} FrameControl;

typedef enum packet_control_t
{
	DATA,
	START,
	END
} PacketControl;

typedef enum device_t
{
	RECEIVER,
    TRANSMITTER
} Device;

//#define DEBUG

#define FLAG 0x7E
#define ADDRESS 0x03
#define CONTROL_SET 0x03
#define BCC_SET CONTROL_SET ^ ADDRESS

#define CONTROL_UA 0x07
#define BCC_UA CONTROL_UA ^ ADDRESS

#define CONTROL_DISC 0x11
#define BCC_DISC CONTROL_DISC ^ ADDRESS

#define CONTROL_RR 0x05
#define BCC_RR CONTROL_RR ^ ADDRESS

#define CONTROL_REJ 0x01
#define BCC_REJ CONTROL_REJ ^ ADDRESS

#define BCC(c, a) c ^ a

#define TRANSMITTER_PORT "/dev/ttyS5"
#define RECEIVER_PORT "/dev/ttyS0"

#define TIMEOUT 3
#define MAXTRANSMISSIONS 3

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

//volatile int STOP = FALSE;

#endif