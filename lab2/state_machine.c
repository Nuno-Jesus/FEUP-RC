#include "state_machine.h"

State_Machine *new_state_machine(Device device)
{
	State_Machine *machine = (State_Machine *)malloc(sizeof(State_Machine));
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
	frame[2] = device == RECEIVER ? SET_CONTROL : UA_CONTROL;
	frame[3] = device == RECEIVER ? BCC_SET : BCC_UA;
	frame[4] = FLAG;

	return frame;
}

void delete_state_machine(State_Machine *machine)
{
	if(!machine)
		return;
	
	if(machine->frame)
		free(machine->frame);

	if(machine->expectedFrame)
		free(machine->expectedFrame);
	
	free(machine);
}