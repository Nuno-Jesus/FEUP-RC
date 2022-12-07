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

int llwrite(int fd, char *buffer, int length)
{
	unsigned char expected;
	int bytes;
	int newLength = length;

	if (!(ll->frame = assemble_information_frame(buffer, &newLength)))
		return -1;

	ll->frameSize = newLength;
	expected = ll->sequenceNumber == 0 ? RR01 : RR00;

	start_alarm(a);
	do
	{
		if (!a->isActive)
		{
			a->isActive = TRUE;
			
			if (!send_information_frame(ll->frame, ll->frameSize))
				return -1;
			
			printf("Sending data frame (size = %d).\n", ll->frameSize);
			if (receive_supervision_frame(TRANSMITTER, expected))
			{
				printf("RR %d received.\n", expected);
				stop_alarm();
				break;
			}
			else
				printf("REJ received. Attempts: %d\n", a->counter++);
		}
		if (a->counter >= MAXTRANSMISSIONS)
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

		// Unstuff it
		if ((ll->frame = unstuff_information_frame(ll->frame, &ll->frameSize)) < 0)
			return 0;

		#ifdef DEBUG
			printf("Size after destuffing: %d\n", ll->frameSize);
		#endif

		// Parse the control byte (Sequence Number)
		int controlByte = ll->frame[2] == 0 ? 0 : 1;

		// Get the bbc2 field
		int bcc2 = ll->frame[ll->frameSize - 2];

		// Check if bcc2 is correct
		#ifdef DEBUG
			printf("bcc vs get_bcc2: 0x%02X, 0x%02X\n", bcc2, get_bcc2(&ll->frame[4], ll->frameSize - 6));
		#endif

		if (bcc2 == get_bcc2(&ll->frame[4], ll->frameSize - 6))
		{
			if (controlByte != ll->sequenceNumber)
			{
				responseByte = (controlByte == 1) ? RR00 : RR01;
				ll->sequenceNumber = (controlByte == 1) ? 0 : 1;
			}

			else
			{
				//printf("BCC is correct\n");
				bufferFull = true;
				memcpy(buffer, ll->frame + 4, ll->frameSize - 2);

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

	//If the frame was correct, free it to handle the next one
	if (ll->frame)
	{
		free(ll->frame);
		ll->frame = NULL;
	}

	bytesRead = ll->frameSize;
	ll->frameSize = 0;

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
		if (a->counter >= MAXTRANSMISSIONS)
			return 0;
	} while (1);

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
		if (a->counter >= MAXTRANSMISSIONS)
			return 0;
	} while (1);

	return 1;
}

int llclose_receiver()
{
	do
	{
		if (receive_supervision_frame(RECEIVER, DISC))
		{
			if (send_supervision_frame(DISC))
			{
				printf("Sending DISC frame.\n");
				stop_alarm();
				break;
			}
			else
				a->counter++;
		}
		if (a->counter >= MAXTRANSMISSIONS)
			return 0;
	} while (1);

	return 1;
}

int llclose_transmitter()
{
	start_alarm(a);

	do
	{
		if (!a->isActive)
		{
			a->isActive = TRUE;
			if (send_supervision_frame(DISC))
			{
				printf("Sending DISC frame.\n");
				if (receive_supervision_frame(TRANSMITTER, DISC))
				{
					printf("Received DISC frame.\n");
					send_supervision_frame(UA);
					stop_alarm();
					break;
				}
			}
		}
		if (a->counter >= MAXTRANSMISSIONS)
			return 0;
	} while (1);

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