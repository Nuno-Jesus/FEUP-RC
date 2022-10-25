#include "link_layer.h"

LinkLayer *link;
extern Alarm *a;

int llopen(char *port, Device device)
{
	int fd;
	int ret;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	if (!(a = new_alarm(NULL, TIMEOUT)))
		return -1;

	set_alarm(a);

	ret = device == TRANSMITTER ? llopen_transmitter() : llopen_receiver();
	if (ret == 0)
	{
		canonical_close(fd);
		return -1;
	}

	if (!(link = new_link_layer()))
		return -1;

	return fd;
}

int llclose(int fd, Device device)
{
	int ret;

	ret = device == TRANSMITTER ? llclose_transmitter() : llclose_receiver();
	return canonical_close(fd);
}

void get_possible_responses(unsigned char responses[])
{
	if (link->sequenceNumber == 0)
	{
		responses[0] = RR01;
		responses[1] = REJ00;
	}
	else
	{
		responses[0] = RR00;
		responses[1] = REJ01;
	}
}

int assemble_information_frame(unsigned char *buffer, int length)
{
	unsigned char *frame;
	int size;

	size = 6 + length;
	if (!(frame = (unsigned char *)malloc(size * sizeof(char))))
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	frame[2] = SEQ(link->sequenceNumber);
	frame[3] = BCC(frame[1], frame[2]);
	memcpy(frame + 4, buffer, length);
	frame[4 + length] = FLAG;

	return frame;
}

int llwrite(int fd, char *buffer, int length)
{
	unsigned char *frame;
	unsigned char responses[2];
	int bytes;

	if (!(frame = assemble_information_frame(buffer, length)))
		return -1;

	if (!(frame = stuff_information_frame(frame, length)))
		return -1;

	link->frame = frame;
	link->frameSize = 6 + length;

	start_alarm(a);
	get_possible_responses(responses);

	if (!send_information_frame(frame, link->frameSize))
		return -1;

	do
	{
		printf("Sending data frame (length = %d).\n", length);
		if (receive_supervision_frame(TRANSMITTER, responses[0]))
		{
			stop_alarm();
			break;
		}
		else if (receive_supervision_frame(TRANSMITTER, responses[1]))
			printf("REJ received. Attempts: %d\n", a->counter++);
		else if (!(bytes = send_information_frame(frame, link->frameSize)))
			return -1;

	} while (a->counter < MAXTRANSMISSIONS);

	link->sequenceNumber = !link->sequenceNumber;

	return bytes;
}

int llread(int fd, char *buffer)
{
	int bytesRead, responseByte;

	bool bufferFull = false;

	while (!bufferFull)
	{
		if(!receive_information_frame(RECEIVER, SET))
			return 0;

		if ((bytesRead = unstuff_information_frame(link->frame, link->frameSize - 6)) < 0)
			return 0;

		int controlByte;

		if (link->frame[2] == SEQ(0))
			controlByte = 0;

		else
			controlByte = 1;

		int bcc2 = link->frame[bytesRead - 2];

		// Check if bcc2 is correct
		if (bcc2 == get_bcc2(&link->frame[4], bytesRead - 6))
		{
			if (controlByte != link->sequenceNumber)
			{
				if (!controlByte)
				{
					responseByte = RR01;
					link->sequenceNumber = 1;
				}
				else
				{
					responseByte = RR00;
					link->sequenceNumber = 0;
				}
			}
			else
			{
				for (int i = 0; i < bytesRead; i++){
					buffer[i] = link->frame[4 + i];
				}

				bufferFull = true;

				if(!controlByte){
					responseByte = RR01; 
					link->sequenceNumber = 1; 
				}
				else{
					responseByte = RR00;
					link->sequenceNumber = 0;
				}
			}
		}
		// bcc2 was incorrect
		else {
			if (controlByte != link->sequenceNumber)
			{
				if(!controlByte){
					responseByte = RR01; 
					link->sequenceNumber = 1; 
				}
				else{
					responseByte = RR00;
					link->sequenceNumber = 0;
				}
			}
			else
			{
				if (!controlByte) 
				{
					responseByte = REJ01; 
					link->sequenceNumber = 1;
				}
				else
				{
					responseByte = REJ00;
					link->sequenceNumber = 0;
				}
			}
		}
	}

	if(!send_supervision_frame(responseByte))
		return 0;

	return (bytesRead - 6);
}

int llopen_transmitter()
{
	if (!send_supervision_frame(SET))
		return 0;

	start_alarm(a);

	do
	{
		printf("Sending SET frame.\n");
		if (receive_supervision_frame(TRANSMITTER, UA))
		{
			printf("REceived UA frame.\n");
			stop_alarm();
			break;
		}
		else if (!send_supervision_frame(SET))
			return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while (a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen_receiver()
{
	if (!receive_supervision_frame(RECEIVER, SET))
		return 0;

	start_alarm(a);

	do
	{
		printf("Received SET frame.\n");
		if (send_supervision_frame(UA))
		{
			printf("Sending UA frame.\n");
			stop_alarm();
			break;
		}
		else if (!receive_supervision_frame(RECEIVER, SET))
			return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while (a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llclose_receiver()
{
	if (!receive_supervision_frame(RECEIVER, DISC))
		return 0;

	start_alarm(a);

	do
	{
		printf("Received DISC frame.\n");
		if (send_supervision_frame(DISC))
		{
			printf("Sending DISC frame.\n");
			stop_alarm();
			break;
		}
		else if (!receive_supervision_frame(RECEIVER, DISC))
			return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while (a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llclose_transmitter()
{
	if (!send_supervision_frame(DISC))
		return 0;

	start_alarm(a);

	do
	{
		printf("Sending DISC frame.\n");
		if (receive_supervision_frame(TRANSMITTER, DISC))
		{
			printf("Received DISC frame.\n");
			stop_alarm();
			break;
		}
		else if (!send_supervision_frame(DISC))
			return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while (a->counter < MAXTRANSMISSIONS);

	if (!send_supervision_frame(UA))
		return 0;

	printf("Sending UA frame.\n");
	return 1;
}

LinkLayer *new_link_layer()
{
	if (!(link = (LinkLayer *)malloc(sizeof(LinkLayer))))
		return NULL;

	link->sequenceNumber = 0;
	if (!(link->frame = new_frame(1, NULL)))
	{
		free(link);
		return NULL;
	}

	return link;
}