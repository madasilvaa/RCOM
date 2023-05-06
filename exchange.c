#include "exchange.h"


unsigned char set[5] = {FLAG, A_T, C_SET, BCC(A_T, C_SET), FLAG};
unsigned char ua[5] = {FLAG, A_R, C_UA, BCC(A_R, C_UA), FLAG};

unsigned char createBCC(unsigned char a, unsigned char c) {

    return a ^ c;
    
}



int enviaSET(int fd)
{
    return write(fd, set, 5);
}

int enviaUA(int fd)
{
    return write(fd, ua, 5);
}



unsigned char EstadosSupervisao(enum estados *estado, unsigned char byte)
{
    static unsigned char a = 0;
    static unsigned char c = 0;
    switch (*estado) 
    {
        case START:
            if (byte == FLAG) 
            {
                *estado = FLAG_RCV;
            }
            break;

        case FLAG_RCV:
            if (byte == A_R || byte == A_T)
            {
                *estado = A_RCV;
                a = byte;
            }
            else if (byte != FLAG)
            {
                *estado = START;
            }
            break;

        case A_RCV:
            if (byte == C_SET || byte == C_UA)
            {
                *estado = C_RCV;
                c = byte;
            }
            else if (byte == FLAG)
            {
                *estado = FLAG_RCV;
            }
            else
            {
                *estado = START;
            }
            break;

        case C_RCV:
            if (byte == BCC(a,c)) // ou exclusivo
            {
                *estado = BCC_OK;
            }
            else if (byte == FLAG)
            {
                *estado = FLAG_RCV;
            }
            else
            {
                *estado = START;
            }
            break;

        case BCC_OK:
            if (byte == FLAG)
            {
                *estado = STOP;
            }
            else
            {
                *estado = START;
            }
            break;

        case STOP:
            break;
        
        default:
            break;
    }
    return c;

}

int readCommand(int fd){

    unsigned char byte;
    enum estados estado = START;
	
    while (estado != STOP) {

        read(fd,&byte,1);
        processFrameSU(&estado, byte);

    }

    return 0;
}

int readResponse(int fd){

    unsigned char byte;
    enum estados estado = START;

    while(estado!=STOP) {

        read(fd, &byte, 1);
        processFrameSU(&state, byte);

    }

    return 0;
}