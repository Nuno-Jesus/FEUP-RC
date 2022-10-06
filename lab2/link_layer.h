#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#define MAX_SIZE 256

struct linkLayer {
    char port[20];                  /* Dispositivo /dev/ttySx, x = 0, 1 */
    int baudRate;                   /* Velocidade de transmissão */
    unsigned int sequenceNumber;    /* Número da sequência da trama: 0, 1 */
    unsigned int timeout;           /* Valor do temporizador: 1 s */
    unsigned int numTransmissions;  /* Número de tentativas em caso de falha */

    char frame[MAX_SIZE]            /* Trama */
}