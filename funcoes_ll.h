#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "exchange.h"
#include "alarm.h"

#define BAUDRATE B38400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define TRANSMITTER 0
#define RECEIVER 1

#define TIMEOUT 3
#define MAX_TRANSMISSIONS 3

typedef struct
{
    char port[20]; // /dev/ttySx
    int flag; //TRANSMITTER/RECEIVER
    unsigned int sequenceNumber; //numero de sequencia da trama
    unsigned int timeout;
    unsigned int numTransmissions; //numero de tentativas em caso de erro
    unsigned int alarm;
} linkLayer;

extern linkLayer linklayer;

void dadosLinkLayer(int port, int flag); // inicia os dados da linklayer

int openNonCanonical();
int closeNonCanonical();

/*FUNCOES LL: llopen, llclose, llread, llwrite*/

int llopen(int port, int flag);
int llclose(int fd);
