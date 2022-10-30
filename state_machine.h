#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include "utils.h"
#include "link_layer.h"

typedef struct state_machine_t
{
	State state;
	FrameControl controlField;
	Device device;
	int isInfoFrame;
	unsigned char *frame;
	unsigned char byte;
} StateMachine;

StateMachine *new_state_machine(Device, FrameControl, int);

void delete_state_machine(StateMachine *);
//! Handlers
void state_machine_multiplexer(StateMachine *);

void start_handler(StateMachine *);

void wait_address_handler(StateMachine *);

void wait_control_handler(StateMachine *);

void wait_bcc_handler(StateMachine *);

void wait_end_flag_handler(StateMachine *);

#endif