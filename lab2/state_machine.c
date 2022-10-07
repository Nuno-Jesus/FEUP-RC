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
	machine->expectedFrame = create_expected_frame(device);
	if(!machine->expectedFrame || !machine->frame){
		delete_state_machine(machine);
		return NULL;
	}
		
	return machine;
}

unsigned char *create_expected_frame(Device device)
{
	unsigned char *frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!frame)
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	frame[2] = device == RECEIVER ? CONTROL_SET : CONTROL_UA;
	frame[3] = device == RECEIVER ? BCC_SET : BCC_UA;
	frame[4] = FLAG;

	return frame;
}

void delete_state_machine(StateMachine *machine)
{
	if(!machine)
		return;
	
	if(machine->frame)
		free(machine->frame);
	if(machine->expectedFrame)
		free(machine->expectedFrame);
	
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