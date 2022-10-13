#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

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

typedef enum frame_t
{
	SET,
	UA,
	RR00,
	RR01,
	REJ00,
	REJ01,
	DISC
} Frame;

typedef enum device_t
{
	RECEIVER,
    TRANSMITTER
} Device;

typedef struct state_machine_t
{
	State state;
	Frame frame;
	Device device;
	unsigned char *currentFrame;
	unsigned char byte;
} StateMachine;

StateMachine *new_state_machine(Device, Frame);

void delete_state_machine(StateMachine *);
//! Handlers
void state_machine_multiplexer(StateMachine *);

void start_handler(StateMachine *);

void wait_address_handler(StateMachine *);

void wait_control_handler(StateMachine *);

void wait_bcc_handler(StateMachine *);

void wait_end_flag_handler(StateMachine *);

#endif