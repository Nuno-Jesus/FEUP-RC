#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "macros.h"

typedef enum state_t
{
	START,
	WAIT_ADDRESS,
	WAIT_CONTROL,
	WAIT_BCC,
	WAIT_END_FLAG,
	END
} State;

typedef enum device_t
{
	RECEIVER,
    TRANSMITTER
} Device;

typedef struct state_machine_t
{
	State state;
	Device device;
	unsigned char *frame;
	unsigned char *expectedFrame;
	unsigned char currentByte;
} State_Machine;


State_Machine *new_state_machine(Device);

unsigned char *create_expected_frame(Device);

void delete_state_machine(State_Machine *);

//! Handlers
void state_machine_multiplexer(State_Machine *);

void start_handler(State_Machine *);

void wait_address_handler(State_Machine *);

void wait_control_handler(State_Machine *);

void wait_bcc_handler(State_Machine *);

void wait_end_flag_handler(State_Machine *);

#endif