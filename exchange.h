#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "funcoes_ll.h"

#define BCC(a,c) (a ^ c)

#define FLAG 0x07e
#define A_T 0x01
#define A_R 0x03
#define C_SET 0x03
#define C_UA 0x07
#define C_DISC 0x0b

#define C_I0 0x00
#define C_I1 0x40

#define C_RR0 0x05
#define C_RR1 0x85
#define C_REJ0 0x01
#define C_REJ1 0x81

#define ESCAPE 0x7d // Octeto de escape
#define STUFFING 0x20 // Octeto para stuffing

enum estados // estados presentes na maquina de estado
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    DATA,
    STOP,
};

/*-------------funcoes a serem utilizadas no programa------------------------------*/

int enviaSET(int fd); // envio da trama SET pelo Transmissor 
int enviaUA(int fd); // envio da trama UA pelo Recetor
int enviaDISC(int fd);
int envia_ultimo_UA(int fd);
int enviaRR0(int fd);
int enviaRR1(int fd);
int enviaREJ0(int fd);
int enviaREJ1(int fd);


unsigned char EstadosSupervisao(enum estados *estado, unsigned char byte); // funcao que processa as tramas SET e UA *META 3*

void EstadosInformacao(enum estados *estado, unsigned char byte); //funcao que processa as tramas do tipo I

int lerComando(int fd); // funcao tem como objetivo ler as tramas SET e DISC 

int lerResposta(int fd); // funcao tem como objetivo ler a trama UA

int lerTramasI(int fd, unsigned char *frame); // funcao que tem como objetivo ler tramas I *META 5*

int lerTramasRR_REJ(int fd); // funcao que tem como objetivo ler tramas do tipo RR e REJ *META 6*

int TramaDestuffing(unsigned char* trama, int tamanho, unsigned char* trama_destuffed); //funcao que trata do destuffing do receptor *META 8*

int TramaStuffing(int fd, unsigned char *buffer, int length); // funcao que trata do stuffing *META 8*

void generateBCC2Error(unsigned char *trama, int percentagem);