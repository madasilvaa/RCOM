#pragma once
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "macros.h"
#include "parte2.h"

#define FLAG 0x07E
#define A_T 0x01
#define A_R 0x03
#define C_SET 0x03
#define C_UA 0x07



enum estados // estados presentes na maquina de estado
{
    START,
    FLAG_RCV,
    A_RCV,
    C_RCV,
    BCC_OK,
    STOP,
};

/*-------------funcoes a serem utilizadas no programa------------------------------*/

int enviaSET(int fd); // envio da trama SET
int enviaUA(int fd); // envio da trama UA


/*int enviaDISC(int fd); //envio da trama DISC
int enviaRR0(int fd);
int enviaRR1(int fd);
int enviaREJ0(int fd);
int enviaREJ1(int fd);*/

unsigned char StateMachineSupervisao(enum estados *estado, unsigned char byte) // funcao que processa uma trama de supervisao