#include "state_machine.h"

StateMachine *new_state_machine(Device device)
{
	StateMachine *machine = (StateMachine *)malloc(sizeof(StateMachine));
	if (machine == NULL)
		return NULL;

	machine->state = START;
	machine->device = device;
	machine->currentByte = 0x00;
	machine->frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if(!machine->frame){
		delete_state_machine(machine);
		return NULL;
	}
		
	return machine;
}

void delete_state_machine(StateMachine *machine)
{
	if(!machine)
		return;
	
	if(machine->frame)
		free(machine->frame);
	
	free(machine);
}

void state_machine_multiplexer(StateMachine *machine)
{
	switch(machine->state)
	{
		case START:
			start_handler(machine);
			break;
		case WAIT_ADDRESS:
			wait_address_handler(machine);
			break;
		case WAIT_CONTROL:
			wait_control_handler(machine);
			break;
		case WAIT_BCC:
			wait_bcc_handler(machine);
			break;
		case WAIT_END_FLAG:
			wait_end_flag_handler(machine);
			break;
		default:
			return;
	}
}

void start_handler(StateMachine *machine)
{
	switch(machine->currentByte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_address_handler(StateMachine *machine)
{
	switch(machine->currentByte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case ADDRESS:
			machine->state = WAIT_CONTROL;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_control_handler(StateMachine *machine)
{
	unsigned char control;	

	control = machine->device == RECEIVER ? CONTROL_SET : CONTROL_UA;
	switch(machine->currentByte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case control:
			machine->state = WAIT_BCC;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_bcc_handler(StateMachine *machine)
{
	unsigned char bcc;

	bcc = machine->device == RECEIVER ? BCC_SET : BCC_UA;
	switch(machine->currentByte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case bcc:
			machine->state = WAIT_END_FLAG;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_end_flag_handler(StateMachine *machine)
{
	switch(machine->currentByte)
	{
		case FLAG:
			machine->state = END;
			break;
		default:
			machine->start = START;
			break;
	}
}