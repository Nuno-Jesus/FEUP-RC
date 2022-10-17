#ifndef FRAME_H
#define FRAME_H

#include "utils.h"
#include "packet.h"

typedef struct frame_t
{
	unsigned char bcc1;
	unsigned char bcc2;
	unsigned char control;
	Packet *packet;
} Frame;


/**
 * @brief Creates a new frame with the given control field and packet 
 * 
 * @param control The control field of the new frame 
 * @param packet The packet to encapsulate
 * @return A pointer to a new frame, NULL in case of error
 */
Frame *new_frame(unsigned char control, Packet *packet);

void delete_frame(Frame *frame);

unsigned char *assemble_information_frame(Frame *frame);

#endif