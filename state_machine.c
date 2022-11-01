#include "state_machine.h"

extern LinkLayer *ll;

StateMachine *new_state_machine(Device device, FrameControl frame, int isInfoFrame)
{
	StateMachine *machine = (StateMachine *)malloc(sizeof(StateMachine));
	if (machine == NULL)
		return NULL;

	machine->state = START;
	machine->device = device;
	machine->controlField = frame;
	machine->isInfoFrame = isInfoFrame;
	machine->byte = 0x00;
	machine->frame = (unsigned char *)malloc(5 * sizeof(unsigned char));
	if (!machine->frame)
	{
		delete_state_machine(machine);
		return NULL;
	}

	return machine;
}

void delete_state_machine(StateMachine *machine)
{
	if (!machine)
		return;

	if (machine->frame)
		free(machine->frame);

	free(machine);
}

void state_machine_multiplexer(StateMachine *machine)
{
	switch (machine->state)
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
	switch (machine->byte)
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
	switch (machine->byte)
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
	if (machine->isInfoFrame)
	{
		//printf("Machine Byte: %d\n", machine->byte);
		//printf("Sequence Number: %d\n", ll->sequenceNumber);

		if (ll->sequenceNumber == SEQ(0) && machine->byte == SEQ(0))
			machine->state = WAIT_BCC;

		else if (ll->sequenceNumber == SEQ(1) && machine->byte == SEQ(1))
			machine->state = WAIT_BCC;

		else
			machine->state = START;

		return;
	}

	switch (machine->byte)
	{
		case FLAG:
			machine->state = WAIT_ADDRESS;
			break;
		case CONTROL_SET:
			if (machine->controlField == SET)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_UA:
			if (machine->controlField == UA)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_DISC:
			if (machine->controlField == DISC)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_RR(0):
			if (machine->controlField == RR00)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_RR(1):
			if (machine->controlField == RR01)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_REJ(0):
			if (machine->controlField == REJ00)
				machine->state = WAIT_BCC;
			else
				machine->state = START;
			break;
		case CONTROL_REJ(1):
			if (machine->controlField == REJ01)
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

	switch (machine->byte)
	{
	case FLAG:
		machine->state = WAIT_ADDRESS;
		break;
	case BCC_SET:
		if (machine->controlField == SET)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_UA:
		if (machine->controlField == UA)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_DISC:
		if (machine->controlField == DISC)
			machine->state = WAIT_END_FLAG;
		else
			machine->controlField == START;
		break;
	case BCC_RR(0):
		if (machine->controlField == RR00)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_RR(1):
		if (machine->controlField == RR01)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_REJ(0):
		if (machine->controlField == REJ01)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_REJ(1):
		if (machine->controlField == REJ01)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	case BCC_SEQ(0):
		if (!ll->sequenceNumber)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;

	case BCC_SEQ(1):
		if (ll->sequenceNumber)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;

	default:
		printf("Default state\n");
		machine->state = START;
		break;
	}
}

void wait_end_flag_handler(StateMachine *machine)
{
	switch (machine->byte)
	{
	case FLAG:
		machine->state = END;
		break;
	default:
		if (machine->isInfoFrame)
			machine->state = WAIT_END_FLAG;
		else
			machine->state = START;
		break;
	}
}