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
	DATA_PACKET = 1,
	START_PACKET,
	END_PACKET
} PacketControl;

typedef enum packet_parameter_t
{
	FILESIZE,
	FILENAME
} PacketParameter;

typedef enum device_t
{
	RECEIVER,
    TRANSMITTER
} Device;

//#define DEBUG

#define FLAG 0x7E
#define ADDRESS 0x03
#define ESCAPE 0x7D
#define FLAG_STUFFED 0x5E
#define ESCAPE_STUFFED 0x5D

#define CONTROL_SET 0x03
#define BCC_SET CONTROL_SET ^ ADDRESS

#define CONTROL_UA 0x07
#define BCC_UA CONTROL_UA ^ ADDRESS

#define CONTROL_DISC 0x11
#define BCC_DISC CONTROL_DISC ^ ADDRESS

#define CONTROL_RR(s) (s << 8 | 0x05)
#define BCC_RR(s) CONTROL_RR(s) ^ ADDRESS

#define CONTROL_REJ(s) (s << 8 | 0x01)
#define BCC_REJ(s) CONTROL_REJ(s) ^ ADDRESS

#define BCC(a, c) a ^ c
#define SEQ(s) s << 7

#define TRANSMITTER_PORT "/dev/ttyS5"
#define RECEIVER_PORT "/dev/ttyS0"

#define TIMEOUT 3
#define MAXTRANSMISSIONS 3

#define BAUDRATE B38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define CONTROL_DATA_FIELD_1 0x00
#define CONTROL_DATA_FIELD_2 0x40

#define MAX_FILENAME_SIZE 255
#define MAX_DATA 1024
#define MAX_DATA_PACKET (MAX_DATA + 4)	// 4 extra bytes for the packet header


#endif