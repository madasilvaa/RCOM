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

//ROLE
#define NOT_DEFINED -1
#define TRANSMITTER 0
#define RECEIVER 1
#define BAUDRATE B38400


//SIZE of maximum acceptable payload; maximum number of bytes that application layer should send to link layer
#define MAX_PAYLOAD_SIZE 1000

//CONNECTION deafault values
#define BAUDRATE_DEFAULT B38400
#define MAX_RETRANSMISSIONS_DEFAULT 3
#define TIMEOUT_DEFAULT 4
#define _POSIX_SOURCE 1 /* POSIX compliant source */

//MISC
#define FALSE 0
#define TRUE 1

#define TIMEOUT 3
#define MAX_TRANSMISSIONS 3

typedef struct
{
    unsigned int numSentFramesI;
    unsigned int numReceivedFramesI;
    unsigned int numTimeouts;
    unsigned int numSentRR;
    unsigned int numReceivedRR;
    unsigned int numSentREJ;
    unsigned int numReceivedREJ;
} statistics;

typedef struct
{
    char port[20]; // /dev/ttySx
    int flag; //TRANSMITTER/RECEIVER
    unsigned int sequenceNumber; //numero de sequencia da trama
    unsigned int timeout;
    int baudRate;
    unsigned int numTransmissions; //numero de tentativas em caso de erro
    unsigned int alarm;
    statistics stats;
} linkLayer;

extern linkLayer linklayer;

void dadosLinkLayer(int port, int flag); // inicia os dados da linklayer

statistics initStatistics();

int openNonCanonical();
int closeNonCanonical();

/*FUNCOES LL: llopen, llclose, llread, llwrite*/

int llopen(int port, int flag);
int llclose(int fd);
int llread(int fd, char *buffer);
int llwrite(int fd, char* buffer, int length);

void displayStats();
