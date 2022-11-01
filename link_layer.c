#include "link_layer.h"

LinkLayer *ll;
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

	if (!(ll = new_link_layer()))
		return -1;

	return fd;
}

int llclose(int fd, Device device)
{
	device == TRANSMITTER ? llclose_transmitter() : llclose_receiver();
	return canonical_close(fd);
}

void get_possible_responses(unsigned char responses[])
{
	if (ll->sequenceNumber == 0)
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

unsigned char *assemble_information_frame(unsigned char *buffer, int length)
{
	unsigned char *frame;
	int size;

	size = 6 + length;
	if (!(frame = (unsigned char *)malloc(size * sizeof(char))))
		return NULL;

	frame[0] = FLAG;
	frame[1] = ADDRESS;
	frame[2] = SEQ(ll->sequenceNumber);
	frame[3] = BCC(frame[1], frame[2]);
	memcpy(frame + 4, buffer, length);
	frame[4 + length] = get_bcc2(frame + 4, length);
	frame[4 + length + 1] = FLAG;

	return frame;
}

int llwrite(int fd, char *buffer, int length)
{
	unsigned char responses[2];
	int bytes;
	int newLength;

	if (!(newLength = stuff_information_frame(buffer, length)))
		return -1;

	if (!(ll->frame = assemble_information_frame(buffer, newLength)))
		return -1;

	ll->frameSize = 6 + newLength;
	// print_frame(ll->frame, ll->framsdeSize);
	start_alarm(a);
	get_possible_responses(responses);

	if (!send_information_frame(ll->frame, ll->frameSize))
		return -1;
	
	#ifdef DEBUG
		print_frame(ll->frame, ll->frameSize);
	#endif
	
	do
	{
		if (!a->isActive)
		{
			a->isActive = TRUE;
			printf("Sending data frame (length = %d).\n", length);
			if (receive_supervision_frame(TRANSMITTER, responses[0]))
			{	
				printf("RR %d received.\n", responses[0]);
				stop_alarm();
				break;
			}
			else if (receive_supervision_frame(TRANSMITTER, responses[1]))
				printf("REJ received. Attempts: %d\n", a->counter++);
			else if (!(bytes = send_information_frame(ll->frame, ll->frameSize)))
				return -1;
		}

		if(a->counter >= MAXTRANSMISSIONS)
			return -1;
	} while (1);

	ll->sequenceNumber = !ll->sequenceNumber;

	return bytes;
}

int llread(int fd, char *buffer)
{
	int bytesRead, responseByte;

	bool bufferFull = false;

	while (!bufferFull)
	{
		// Read the frame
		if (!receive_information_frame(RECEIVER))
			return 0;

		#ifdef DEBUG
			printf("\n\tBEFORE DESTUFFING\n\n");
			print_frame(ll->frame, ll->frameSize);
		#endif

		// Unstuff it
		if ((bytesRead = unstuff_information_frame(ll->frame, ll->frameSize)) < 0)
			return 0;

			// printf("Size after destuffing: %d\n", bytesRead);

		#ifdef DEBUG
			printf("\n\tAFTER DESTUFFING\n\n");
			print_frame(ll->frame, ll->frameSize);
		#endif

		// Parse the control byte (Sequence Number)
		int controlByte = ll->frame[2] == 0 ? 0 : 1;

		// Get the bbc2 field
		int bcc2 = ll->frame[bytesRead - 2];

		// printf("bbc2 is %d\n", ll->frame[bytesRead - 2]);
		// printf("get_bcc2 is %d\n", get_bcc2(&ll->frame[4], bytesRead - 6));

		// Check if bcc2 is correct
		if (bcc2 == get_bcc2(&ll->frame[4], bytesRead - 6))
		{
			if (controlByte != ll->sequenceNumber)
			{
				responseByte = (controlByte == 1) ? RR00 : RR01;
				ll->sequenceNumber = (controlByte == 1) ? 0 : 1;
			}

			else
			{
				bufferFull = true;
				memcpy(buffer, ll->frame + 4, bytesRead);

				responseByte = (controlByte == 1) ? RR00 : RR01;
				ll->sequenceNumber = (controlByte == 1) ? 0 : 1;
			}
		}
		// error in the data field
		else
		{
			// If this is a duplicated frame, do nothing with the data, and send RR(N)
			if (controlByte != ll->sequenceNumber)
			{
				responseByte = (controlByte == 1) ? RR00 : RR01;
				ll->sequenceNumber = (controlByte == 1) ? 0 : 1;
			}
			// If this is a new frame, request a retrasmission with REJ(N)
			else
			{
				responseByte = (controlByte == 1) ? REJ01 : REJ00;
				ll->sequenceNumber = (controlByte == 1) ? 1 : 0;
			}
		}
	}

	if (!send_supervision_frame(responseByte))
		return 0;

	return (bytesRead - 6);
}

int llopen_transmitter()
{
	start_alarm(a);

	do
	{
		if (!a->isActive)
		{
			a->isActive = TRUE;
			if (!send_supervision_frame(SET))
				return 0;

			printf("Sending SET frame.\n");
			if (receive_supervision_frame(TRANSMITTER, UA))
			{
				printf("Received UA frame.\n");
				stop_alarm();
				break;
			}
		}
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while (a->counter < MAXTRANSMISSIONS);

	return 1;
}

int llopen_receiver()
{
	start_alarm(a);

	do
	{
		if (receive_supervision_frame(RECEIVER, SET))
		{
			printf("Received SET frame.\n");
			if (!send_supervision_frame(UA))
				return 0;

			printf("Sending UA frame.\n");
			stop_alarm();
			break;
		}
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
	if (!(ll = (LinkLayer *)malloc(sizeof(LinkLayer))))
		return NULL;

	ll->sequenceNumber = 0;
	ll->frameSize = 0;
	ll->frame = (unsigned char *)malloc(0);

	return ll;
}