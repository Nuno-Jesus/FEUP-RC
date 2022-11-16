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

/**
 * @brief Creates a new state machine to a given device and expected frame 
 * 
 * @param device TRANSMITTER or RECEIVER
 * @param control The expected frame to receive
 * @param isInfoFrame If we are expecting an information frame or not
 * @return A new StateMachine struct
 */
StateMachine *new_state_machine(Device, FrameControl, int);

/**
 * @brief Delete and free the memory of a state machine
 * @param machine State Machine
 */
void delete_state_machine(StateMachine *);

/**
 * @brief Redirects the program flow to the correct handler, depending on the current state
 * @param machine The State Machine to use
 */
void state_machine_multiplexer(StateMachine *);

/**
 * @brief Handles the Start State
 * @param machine The State Machine to use
 */
void start_handler(StateMachine *);

/**
 * @brief Handles the Wait Address State
 * @param machine The State Machine to use
 */
void wait_address_handler(StateMachine *);

/**
 * @brief Handles the Control State
 * @param machine The State Machine to use
 */
void wait_control_handler(StateMachine *);

/**
 * @brief Handles the BCC State
 * @param machine The State Machine to use
 */
void wait_bcc_handler(StateMachine *);

/**
 * @brief Handles the End Flag State
 * @param machine The State Machine to use
 */
void wait_end_flag_handler(StateMachine *);

#endif