#include "exchange.h"


unsigned char set[5] = {FLAG, A_R, C_SET, BCC(A_R, C_SET), FLAG};
unsigned char ua[5] = {FLAG, A_R, C_UA, BCC(A_R, C_UA), FLAG};
unsigned char ua_last[5] = {FLAG, A_T, C_UA, BCC(A_T, C_UA), FLAG};
unsigned char disc[5] = {FLAG, A_R, C_DISC, BCC(A_R, C_DISC), FLAG};
unsigned char rr0[5] = {FLAG, A_R, C_RR0, BCC(A_R, C_RR0), FLAG};
unsigned char rr1[5] = {FLAG, A_R, C_RR1, BCC(A_R, C_RR1), FLAG};
unsigned char rej0[5] = {FLAG, A_R, C_REJ0, BCC(A_R, C_REJ0), FLAG};
unsigned char rej1[5] = {FLAG, A_R, C_REJ1, BCC(A_R, C_REJ1), FLAG};

/*unsigned char createBCC(unsigned char a, unsigned char c)
{
    return a ^ c;   
}*/

int enviaSET(int fd) // META 2
{
    return write(fd, set, 5);
}

int enviaUA(int fd) // META 2
{
    return write(fd, ua, 5);
}

int enviaDISC(int fd)
{
    return write(fd, disc, 5);
}

int envia_ultimo_UA(int fd)
{
    return write(fd, ua_last, 5);
}

int enviaRR0(int fd) //META 6
{
    return write(fd, rr0, 5);
}

int enviaRR1(int fd) //META 6
{
    return write(fd, rr1, 5);
}

int enviaREJ0(int fd) // META 9
{
    return write(fd, rej0, 5);
}

int enviaREJ1(int fd) // META 9
{
    return write(fd, rej1, 5);
}


unsigned char EstadosSupervisao(enum estados *estado, unsigned char byte) // META 3
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
            if (byte == C_SET || byte == C_DISC || byte == C_UA || byte == C_RR0 || byte == C_RR1 || byte == C_REJ0 || byte == C_REJ1)
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

void EstadosInformacao(enum estados *estado, unsigned char byte) //maquina de estados que processa as tramas do tipo I
{
    static unsigned char aux = 0;
    switch (*estado)
    {
        case START:
            if (byte == FLAG) 
            {
                *estado = FLAG_RCV;
            }
            break;

        case FLAG_RCV:
            if (byte == A_R)
            {
                *estado = A_RCV;
            }
            else if (byte!= FLAG)
            {
                *estado = START;
            }
            break;

        case A_RCV:
            if ((byte == C_I0 && linklayer.sequenceNumber==0) || (byte == C_I1 && linklayer.sequenceNumber==1))
            {
                *estado = C_RCV;
                aux = byte;
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
            if (byte == BCC(A_R,aux))
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
            if (byte != FLAG)
            {
                *estado = DATA;
            }
            else
            {
                *estado = START;
            }
            break;
        case DATA:
            if (byte == FLAG)
            {
                *estado = STOP;
            }
        case STOP:
            break;
        
        default:
            break;
    }
}

int lerComando(int fd)
{

    unsigned char byte;
    enum estados estado = START;
	
    while (estado != STOP)
    {
        read(fd,&byte,1);
        EstadosSupervisao(&estado, byte);
    }

    return 0;
}

int lerResposta(int fd)
{

    unsigned char byte;
    enum estados estado = START;

    while(estado!=STOP)
    {
        read(fd, &byte, 1);
        EstadosSupervisao(&estado, byte);
    }
    return 0;
}

int lerTramasI(int fd, unsigned char *frame)
{
    enum estados estado = START;
    unsigned char byte = 0;
    int tamanho = 0;

    while(estado != STOP)
    {
        read(fd,&byte,1);
        EstadosInformacao(&estado, byte);
        if(tamanho != 0 && estado == FLAG_RCV)
        {
            tamanho = 0;
        }
        frame[tamanho++] = byte;
    }

    // PARA DEPOIS: linklayer.stats.numReceivedFramesI++;

    return tamanho;
}

int lerTramasRR_REJ(int fd)
{
    unsigned char byte, campodecontrolo;
    enum estados estado = START;

    while(estado != STOP)
    {
        read(fd, &byte, 1);
        campodecontrolo = EstadosSupervisao(&estado, byte);
    }

    if(linklayer.sequenceNumber == 1 && campodecontrolo == C_RR0)
    {
        printf("A trama RR0 foi recebida!\n");
        linklayer.sequenceNumber = 0;
        return 0;
    }

    else if(linklayer.sequenceNumber == 0 && campodecontrolo == C_RR1)
    {
        printf("A trama RR1 foi recebida!\n");
        linklayer.sequenceNumber = 1;
        return 0;
    }

    else if(campodecontrolo == C_REJ0)
    {
        printf("A trama REJ0 foi recebida!\n");
        linklayer.numTransmissions++;
        return -1;
    }

    else if(campodecontrolo == C_REJ1)
    {
        printf("A trama REJ1 foi recebida!\n");
        linklayer.numTransmissions++;
        return -1;
    }

    return -1;
}

int TramaStuffing(int fd, unsigned char *buffer, int length) // esta funcao constroi o cabecalho da trama, calcula bcc2, stuffing dos dados e envia a trama pela porta serie
{
    unsigned char frame[2 * length + 6];

    //linklayer.sequenceNumber==0 ? C_I0 : C_I1

    // Frame Header
    frame[0] = FLAG;
    frame[1] = A_R;
    
    if(linklayer.sequenceNumber==0)
    {
        frame[2] = C_I0;
    }
    else
    {
        frame[2] = C_I1;
    }

    frame[3] = BCC(A_R, frame[2]);

    // BCC2 antes de ocorrer o byte stuffing 
    unsigned char bcc2 = buffer[0];
    int i;
    for(i = 1; i<length; i++)
    {
        bcc2 ^= buffer[i];
    }

    // Process data
    int indiceData=0, indiceTrama=4;
    unsigned char temp_buffer;

    while(indiceData < length)
    {
        temp_buffer = buffer[indiceData++];

        // Byte Stuffing
        if(temp_buffer == FLAG || temp_buffer == ESCAPE)
        {
            frame[indiceTrama++] = ESCAPE;
            frame[indiceTrama++] = temp_buffer ^ STUFFING;
        }
        else
        {
            frame[indiceTrama++] = temp_buffer;
        }
    }

    // Frame Footer
    if(bcc2 == FLAG || bcc2 == ESCAPE)
    {
        frame[indiceTrama++] = ESCAPE;
        frame[indiceTrama++] = bcc2 ^ STUFFING;
    }
    else
    {
        frame[indiceTrama++] = bcc2;
    }

    frame[indiceTrama++] = FLAG;
    write(fd, frame, indiceTrama);
    //linklayer.stats.numSentFramesI++;

    return indiceTrama;
}

int TramaDestuffing(unsigned char* trama, int tamanho, unsigned char* trama_destuffed)
{
    // Parametros da trama: 0 -> FLAG; 1 -> A; 2 -> C; 3 -> BCC1

    trama_destuffed[0] = trama[0];
    trama_destuffed[1] = trama[1];
    trama_destuffed[2] = trama[2];
    trama_destuffed[3] = trama[3];

    // Process data
    int indiceTramaDestuffed = 4;
    int indiceTrama;

    for(indiceTrama=4; indiceTrama < tamanho - 1; indiceTrama++)
    {
        if(trama[indiceTrama] == ESCAPE)
        {
            indiceTrama++;
            trama_destuffed[indiceTramaDestuffed] = trama[indiceTrama] ^ STUFFING;
        }
        else
        {
            trama_destuffed[indiceTramaDestuffed] = trama[indiceTrama];
        }
        indiceTramaDestuffed++;
    }

    // Frame Footer
    trama_destuffed[indiceTramaDestuffed++] = trama[indiceTrama++]; // FLAG

    return indiceTramaDestuffed;
}

void generateBCC2Error(unsigned char *trama, int percentagem)
{
    int probabilidade = (rand() % 100) + 1;

    if (probabilidade <= percentagem)
    {
        unsigned char randomByte = (unsigned char)((rand() % 177));
        trama[4] = randomByte;
        printf("BCC2 gerado com erros\n\n");
    }
}