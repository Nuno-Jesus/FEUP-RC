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

typedef enum state_t
{
	START,
	WAIT_ADDRESS,
	WAIT_CONTROL,
	WAIT_BCC,
	WAIT_END_FLAG,
	END
}State;

typedef struct state_machine_t
{
	State state;
	Device device;
	unsigned char *frame;
	unsigned char *expectedFrame;
	unsigned char currentByte;
} State_Machine;

typedef enum device_t
{
	RECEIVER,
    TRANSMITTER
}Device;

State_Machine *new_state_machine(Device);

void state_machine_multiplexer(State_Machine *);

void start_handler(State_Machine *);

void wait_address_handler(State_Machine *);

void wait_control_handler(State_Machine *);

void wait_bcc_handler(State_Machine);

void wait_end_flag_handler(State_Machine *);