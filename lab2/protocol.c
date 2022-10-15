#include "protocol.h"

extern Alarm* a;

int llopen(char *port, Device device)
{
	int fd;
	int ret;

	if ((fd = canonical_open(port)) == -1)
		return -1;

	if (!(a = new_alarm(NULL, TIMEOUT)))
		return 0;

	set_alarm(a);

	ret = device == TRANSMITTER ? llopen_transmitter() : llopen_receiver();
	if (ret == 0)
	{
		canonical_close(fd);
		return -1;
	}
	return fd;
}

int llclose(int fd, Device device)
{    
    int ret;
    
	ret = device == TRANSMITTER ? llclose_transmitter() : llclose_receiver();
    return canonical_close(fd);
}

int llwrite(int fd, char *buffer, int length)
{
	return -1;
}

int llread(int fd, char *buffer)
{
	return -1;
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
		else
			if (!send_supervision_frame(SET))
				return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

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
		else
			if (!receive_supervision_frame(RECEIVER, SET))
				return 0;
		// In case of a timeout when reading the UA frame, a new alarm is setted up
		// and a->counter is incremented. It pretty much works like calling the handler
		// at the end of a send/receive pair
	} while(a->counter < MAXTRANSMISSIONS);

	return 1;
}


int llclose_receiver()
{
    if (!receive_supervision_frame(RECEIVER,DISC))
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
        else
            if (!receive_supervision_frame(RECEIVER,DISC))
                return 0;
        // In case of a timeout when reading the UA frame, a new alarm is setted up
        // and a->counter is incremented. It pretty much works like calling the handler
        // at the end of a send/receive pair
    } while(a->counter < MAXTRANSMISSIONS);

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
        else
            if (!send_supervision_frame(DISC))
                return 0;
        // In case of a timeout when reading the UA frame, a new alarm is setted up
        // and a->counter is incremented. It pretty much works like calling the handler
        // at the end of a send/receive pair
    } while(a->counter < MAXTRANSMISSIONS);

    if(!send_supervision_frame(UA))
        return 0;

	printf("Sending UA frame.\n");
    return 1;
}