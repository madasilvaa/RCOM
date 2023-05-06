#include "statemachine.h"


unsigned char set[5] = {FLAG, A_T, C_SET, BCC(A_T, C_SET), FLAG};
unsigned char ua[5] = {FLAG, A_R, C_UA, BCC(A_R, C_UA), FLAG};

unsigned char createBCC(unsigned char a, unsigned char c) {
    return a ^ c;
}

/*
unsigned char disc[5] = {FLAG, A_RE, C_DISC, BCC(A_RE, C_DISC), FLAG};
unsigned char rr0[5] = {FLAG, A_ER, C_RR0, BCC(A_ER, C_RR0), FLAG};
unsigned char rr1[5] = {FLAG, A_ER, C_RR1, BCC(A_ER, C_RR1), FLAG};
unsigned char rej0[5] = {FLAG, A_ER, C_REJ0, BCC(A_ER, C_REJ0), FLAG};
unsigned char rej1[5] = {FLAG, A_ER, C_REJ1, BCC(A_ER, C_REJ1), FLAG};
*/


int enviaSET(int fd)
{
    return write(fd, set, 5);
}

int enviaUA(int fd)
{
    return write(fd, ua, 5);
}

/*
int enviaDISC(int fd)
{
    return write(fd, disc, 5);
}

int enviaRR0(int fd)
{
    return write(fd, rr0, 5);
}

int enviaRR1(int fd)
{
    return write(fd, rr1, 5);
}

int enviaREJ0(int fd)
{
    return write(fd, rej0, 5);
}

int enviaREJ1(int fd)
{
    return write(fd, rej1, 5);
}

*/

unsigned char StateMachineSupervisao(enum Estado_StateMachine *estado, unsigned char byte)
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