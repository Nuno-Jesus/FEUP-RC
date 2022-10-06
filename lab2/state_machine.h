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
#define READ_CONTROL 0x03
#define TRANSMITTER_CONTROL 0x07
#define BCC_READ READ_CONTROL ^ ADDRESS
#define BCC_TRANSMITTER TRANSMITTER_CONTROL ^ ADDRESS
#define BCC(c, a) c ^ a

#define BAUDRATE 38400
#define _POSIX_SOURCE 1 // POSIX compliant source

#define FALSE 0
#define TRUE 1

#define BUF_SIZE 256

volatile int STOP = FALSE;

typedef enum States{
	START,
	FLAG_RCV,
	READ_BYTES,
	ANALYSE_TRAM,
	SEND_UA,
	END
}State;

typedef enum Roles{
	RECEIVER,
    TRANSMITTER
}Role;


void state_machine_multiplexer(unsigned char, Role, State);

State start_handler(unsigned char);

State flag_rcv_handler(unsigned char);

State read_bytes_handler(unsigned char);

State analyse_tram_handler(unsigned char[], Role);

State send_ua_handler(int);
