#include "state_machine.h"

StateMachine *new_state_machine(Device device, Frame frame)
{
	StateMachine *machine = (StateMachine *)malloc(sizeof(StateMachine));
	if (machine == NULL)
		return NULL;

	machine->state = START;
	machine->device = device;
	machine->frame = frame;
	machine->byte = 0x00;
	machine->currentFrame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if(!machine->currentFrame){
		delete_state_machine(machine);
		return NULL;
	}
		
	return machine;
}

void delete_state_machine(StateMachine *machine)
{
	if(!machine)
		return;
	
	if(machine->currentFrame)
		free(machine->currentFrame);
	
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
	switch(machine->byte)
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
	switch(machine->byte)
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
	switch(machine->byte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case CONTROL_SET:
			if(machine->device == RECEIVER)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_UA:
			if(machine->device == TRANSMITTER)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_bcc_handler(StateMachine *machine)
{
	switch(machine->byte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case BCC_SET:
			if(machine->device == RECEIVER)
				machine->state = WAIT_END_FLAG;
			else
				machine->state = START;
			break;
		case BCC_UA:
			if(machine->device == TRANSMITTER)
				machine->state = WAIT_END_FLAG;
			else
				machine->state = START;
			break;
		default:
			machine->state = START;
			break;
	}
}

void wait_end_flag_handler(StateMachine *machine)
{
	switch(machine->byte)
	{
		case FLAG:
			machine->state = END;
			break;
		default:
			machine->state = START;
			break;
	}
}