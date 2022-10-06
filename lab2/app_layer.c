#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#include "state_machine.h"

State s = START;

int llopen(char *port, Role role){
    // Open serial port device for reading and writing, and not as controlling tty
    // because we don't want to get killed if linenoise sends CTRL-C.
    int fd = open(port, O_RDWR | O_NOCTTY);

    if (fd < 0)
    {
        perror(port);
        exit(-1);
    }

    struct termios oldtio;
    struct termios newtio;

    // Save current port settings
    if (tcgetattr(fd, &oldtio) == -1)
    {
        perror("tcgetattr");
        exit(-1);
    }

    // Clear struct for new port settings
    memset(&newtio, 0, sizeof(newtio));

    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    // Set input mode (non-canonical, no echo,...)
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 1; // Inter-character timer unused
    newtio.c_cc[VMIN] = 0;  // Blocking read until 5 chars received

    // VTIME e VMIN should be changed in order to protect with a
    // timeout the reception of the following character(s)

    // Now clean the line and activate the settings for the port
    // tcflush() discards data written to the object referred to
    // by fd but not transmitted, or data received but not read,
    // depending on the value of queue_selector:
    //   TCIFLUSH - flushes data received but not read.
    tcflush(fd, TCIOFLUSH);

    // Set new port settings
    if (tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("tcsetattr");
        exit(-1);
    }

    printf("New termios structure set\n");

    if(role == TRANSMITTER)
        return llopenTransmitter(fd);
    else
        return llopenReceiver(fd);
}

/**
 * @brief 
 * Pseudo code to write llopen (TRANSMITTER):
 * 	1 - Open the port connection;
 * 	2 - Send the SET tram;
 * 	3 - Set an alarm with TIMEOUT;
 * 	4 - Wait for the UA tram to be sent back;
 * 	5 - If the UA tram wasn't received before the alarm went off, do something
 */

int assembleFrame(char *frame, char control)
{
    frame[0] = FLAG;
    frame[1] = ADDRESS;
    frame[2] = control;
    frame[3] = BCC(frame[2], frame[1]);
    frame[4] = FLAG;
}

int llopenReceiver(int fd){
    return 0;
}

int llopenTransmitter(int fd){
    char setFrame[5];
    char uaFrame[5];
    
    /* Assmeble and send the SET frame */
    assembleFrame(setFrame, TRANSMITTER_CONTROL);
    if (writeSupervisionFrame(fd, setFrame, 5) == -1)
        return -1;
    
    // TODO: Activate the alarm

    if (readSupervisionFrame(fd, uaFrame, 5) == -1) // ! Also check if the alarm went off
        return -1;
    return 0;
}

int writeSupervisionFrame(int fd, char *frame)
{
    
}

int readSupervisionFrame(int fd, char *frame)
{
    unsigned char byte;
    while(s != END)
    {
        if(read(fd, &byte, 1) == -1)
            return -1;
        if(parseSupervisionField(byte) == -1)
            return -1;
    }
}