#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#define FLAG 0x7E
#define ADDRESS 0x03
#define SET_CONTROL 0x03
#define UA_CONTROL 0x07
#define BCC_SET SET_CONTROL ^ ADDRESS
#define BCC_UA UA_CONTROL ^ ADDRESS
#define BCC(c, a) c ^ a

#define BAUDRATE 38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

// CHANGE THIS NAMES 
typedef enum state_t{
	START,
	PARSE_ADDRESS,
	PARSE_CONTROL,
	PARSE_BCC,
	PARSE_END_FLAG,
	END
}State;

typedef enum device_t{
	RECEIVER,
    TRANSMITTER
}Device;

void state_machine_multiplexer(unsigned char, Device, State);

/* State start_handler(unsigned char);

State parse_address_handler(unsigned char);

State parse_control_handler(unsigned char);

State parse_bcc_handler(unsigned char[], Device);

State end_handler(int); */
