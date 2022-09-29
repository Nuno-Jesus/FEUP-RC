#include "state_machine.h"
unsigned char set_tram[5] = {0};
int correctBytes = 0;

void state_machine_multiplexer(unsigned char byte, State s){
	switch(s){
		case START:
			start_handler(byte);
			break;
		case FLAG_RCV:
			flag_rcv_handler(byte);
			break;
		case READ_BYTES:
			read_bytes_handler(byte);
			break;
		case ANALYSE_TRAM:
			analyse_tram_handler(byte);
			break;
		case SEND_UA:
			send_ua_handler(byte);
		default:
			break;
	}
}

State start_handler(unsigned char byte){
	if (byte == FLAG){
		set_tram[correctBytes] = byte;
		return FLAG_RCV;
	}
	return START;
}

State flag_rcv_handler(unsigned char byte){
	switch(byte){
		case FLAG:
			return FLAG_RCV;
		default: 
			return READ_BYTES;
	}
}

State read_bytes_handler(unsigned char byte){
	switch(byte){
		case FLAG:
			return ANALYSE_TRAM;
		default:
			return READ_BYTES;
	}
}

State analyse_tram_handler(unsigned char frame[], bool is_reading){
	if (is_reading){
		if (BCC_READ == BCC(frame[1], frame[2]) && (frame[2] == READ_CONTROL)){
			return SEND_UA;
		}
		else{
			correctBytes = 0;
			return START;
		}
	}
	
	else {
		if (BCC_TRANSMITTER == BCC(frame[1], frame[2]) && (frame[2] == TRANSMITTER_CONTROL)){
			return SEND_UA;
		}
		else{
			correctBytes = 0;
			return START;
		}
	}
}


State send_ua_handler(int fd){
	unsigned char ua_tram[5] = {
		FLAG, 
		ADDRESS, 
		TRANSMITTER_CONTROL, 
		BCC_TRANSMITTER, 
		FLAG
	};


	int bytes = write(fd, ua_tram, sizeof ua_tram);

}