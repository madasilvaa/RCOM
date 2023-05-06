
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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

int enviaSET(int fd); // envio da trama SET pelo Tranmissor 
int enviaUA(int fd); // envio da trama UA pelo Recetor


unsigned char EstadosSupervisao(enum estados *estado, unsigned char byte) // funcao que processa as tramas SET e UA

int readCommand(int fd) // funcao tem como objetivo ler a trama SET 

int readResponse(int fd); // funcao tem como objetivo ler a trma UA