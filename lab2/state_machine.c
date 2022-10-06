#include "state_machine.h"
unsigned char set_tram[5] = {0};
int correctBytes = 0;

void state_machine_multiplexer(unsigned char byte, Role role, State s){
	switch(s){
		case S_START:
			start_handler(byte);
			break;
		case S_FLAG:
			FLAG_handler(byte);
			break;
		case S_ADDRESS:
			address_handler(byte, role);
			break;
		case S_CONTROL:
			control_handler(byte);
			break;
		case S_BCC:
			bcc_handler(byte);
			break;
		case S_END_FLAG:
			end_FLAG_handler(byte, role);
			break;
		default:
			break;
	}
}

State start_handler(unsigned char byte){
	if (byte == S_FLAG){
		set_tram[correctBytes] = byte;
		return S_FLAG;
	}
	return S_START;
}

State FLAG_handler(unsigned char byte){
	switch(byte){
		case S_FLAG:
			return S_FLAG;
		case S_ADDRESS: 
			return S_ADDRESS;
		default:
			return S_START;
	}
}

State a_rcv_handler(unsigned char byte, Role role){
	if (byte == RECEIVER_CONTROL && role == RECEIVER)
		return S_CONTROL;
	else if (byte == TRANSMITTER_CONTROL && role == TRANSMITTER)
		return S_CONTROL;
	else if (byte == S_FLAG)
		return S_FLAG;
	else
		return S_START;
}

State c_rcv_handler(unsigned char byte, Role role){
	if (role == RECEIVER){
		if (BCC_READ == BCC(frame[1], frame[2]) && (frame[2] == RECEIVER_CONTROL)){
			return S_END_FLAG;
		}
		else{
			correctBytes = 0;
			return S_START;
		}
	}
	
	else {
		if (BCC_TRANSMITTER == BCC(frame[1], frame[2]) && (frame[2] == TRANSMITTER_CONTROL))
			return S_END_FLAG;
		else{
			correctBytes = 0;
			return S_START;
		}
	}
}


State bcc_handler(unsigned char frame[], Role role){
	
}
