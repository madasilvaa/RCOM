#include "funcoes_ll.h"

struct termios oldtio;
struct termios newtio;

linkLayer linklayer;

void dadosLinkLayer(int port, int flag) // iniciar os dados da linklayer.
{
    char porta[12];
    snprintf(porta, 12, "/dev/ttyS%d", port);
    strcpy(linklayer.port, porta);
    linklayer.flag = flag;
    linklayer.numTransmissions = 0;
    linklayer.alarm = 0;
    linklayer.timeout = TIMEOUT;
    linklayer.sequenceNumber = 0;
}

int openNonCanonical() //abrir a conexao (retirado do moodle!)
{
    /*
	Open serial port device for reading and writing and not as controlling tty
	because we don't want to get killed if linenoise sends CTRL-C.
    */
    int fd = open(linklayer.port, O_RDWR | O_NOCTTY);
    if(fd < 0)
    {
        perror(linklayer.port);
        exit(-1);
    }

    struct termios newtio;

    if(tcgetattr(fd, &oldtio) == -1)
    {
        perror("Erro em tcgetattr - verificar conexao do cabo ou numero da porta");
        exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;
    newtio.c_cc[VTIME] = 1;   /* inter-character timer unused */
    newtio.c_cc[VMIN] = 0;   /* blocking read until 5 chars received */

    tcflush(fd, TCIOFLUSH);

    if(tcsetattr(fd, TCSANOW, &newtio) == -1)
    {
        perror("\nOcorreu um erro.\n");
        exit(-1);
    }

    printf("\nConexao iniciada.\n\n");
    return fd;
}

int closeNonCanonical(int fd)
{
    tcflush(fd, TCIOFLUSH);

    if (tcsetattr(fd,TCSANOW,&oldtio) == -1)
    {
		perror("\nOcorreu um erro.\n");
		exit(-1);
	}

    printf("\nA conexao foi terminada!\n");

    return close(fd);
}

//------------------------------- FUNCOES LL ------------------------------//

int llopen(int port, int flag)
{
    dadosLinkLayer(port, flag); // os dados da linklayer

    int fd = openNonCanonical();


    if(linklayer.flag == TRANSMITTER) // caso emissor
    {
        do
        {
            if(enviaSET(fd) != -1)
            {
                printf("\nA trama SET foi enviada corretamente!\n");
            }
            else
            {
                printf("\nOcorreu um erro a enviar SET.\n");
                return -1;
            }


            if(lerResposta(fd) != -1)
            {
                printf("\nA trama UA foi recebida com sucesso!\n");
            }
            else
            {
                printf("\nOcorreu um erro na rececao da trama UA.\n");
                return -1;
            }
        } while(linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);


        if(linklayer.numTransmissions >= MAX_TRANSMISSIONS) //se o numero de transmissoes ultrapassar o maximo, saimos do programa.
        {
            return -1;
        }
        linklayer.numTransmissions = 0;

    }

    else if(linklayer.flag == RECEIVER) //caso receptor
    {
        if(lerComando(fd) != -1)
        {
            printf("\nA trama SET foi recebida com sucesso.\n");
        }
        else
        {
            printf("\nOcorreu um erro na rececao da trama SET.\n");
            return -1;
        }

        if(enviaUA(fd) != -1)
        {
            printf("\nA trama UA foi enviada com sucesso.\n");
        }
        else
        {
            printf("\nOcorreu um erro no envio da trama UA.\n");
            return -1;
        }
    }

    printf("\nSucesso! A conexao foi estabelecida.\n\n");
    return fd;
}

int llclose(int fd)
{
if(linklayer.flag == TRANSMITTER)
    {
    do 
    {
        if(enviaDISC(fd) != -1)
        {
            printf("\nA trama DISC foi enviada corretamente!\n");
        }
        else
        {
            printf("\nOcorreu um erro a enviar DISC.\n");
            return -1;
        }


        if(lerResposta(fd) != -1)
        {
            printf("\nA trama UA foi recebida com sucesso!\n");
        }
        else
        {
            printf("\nOcorreu um erro na rececao da trama DISC.\n");
            return -1;
        }
    } while(linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);


    if(linklayer.numTransmissions >= MAX_TRANSMISSIONS) //se o numero de transmissoes ultrapassar o maximo, saimos do programa.
    {
        return -1;
    }
    linklayer.numTransmissions = 0;

    if(envia_ultimo_UA(fd) != -1)
    {
        printf("\nUA foi enviado.\n");
    }
    else
    {
        printf("\nOcorreu um erro.\n");
        return -1;
    }
    }

    else if(linklayer.flag == RECEIVER)
    {
        if(lerComando(fd) != -1)
        {
            printf("\nA trama DISC foi recebida com sucesso.\n");
        }
        else
        {
            printf("\nOcorreu um erro na rececao da trama DISC.\n");
            return -1;
        }

        if(enviaDISC(fd) != -1)
        {
            printf("\nA trama DISC foi enviada com sucesso.\n");
        }
        else
        {
            printf("\nOcorreu um erro no envio da trama DISC.\n");
            return -1;
        }

        if(enviaUA(fd) != -1)
        {
            printf("\nA trama UA foi enviada com sucesso.\n");
        }
        else
        {
            printf("\nOcorreu um erro no envio da trama UA.\n");
            return -1;
        }
    }

    printf("\nA ligacao foi terminada!\n\n");

    sleep(1); // para evitar erros de transmissao, damos sleep.

    return closeNonCanonical(fd);
}

int llread(int fd, char *buffer)
{
    int TamanhoDaTrama = 0;
    int TamanhoDaTramaDestuffed = 0;
    int TamanhoDoPacote = 0;
    int recebido = 0;
    unsigned char trama[2 * 1024 + 6]; //1024: tamanho maximo
    unsigned char tramad[1024];
    unsigned char controlo;

    while(recebido != 0)
    {
        if((TamanhoDaTrama = lerTramasI(fd, trama)))
        {
            printf("\nA trama foi recebida!\n");	
            controlo = trama[2];

            // Efetuamos o destuffing da trama recebida
            TamanhoDaTramaDestuffed= TramaDestuffing(trama, TamanhoDaTrama, tramad);

// -------------------------- META 8: CALCULO DE BCC2 ---------------------------------------------------//

            unsigned char bcc2 = tramad[4];
            int i;
            for(i = 5; i<TamanhoDaTramaDestuffed -7 + 5; i++)
            {
                bcc2 ^= tramad[i];
            }

            // Verificacao do calculo de BCC2
            if(bcc2 != tramad[TamanhoDaTramaDestuffed-2])
            {
                printf("\nOcorreu um erro no calculo de BCC2!\n");
                
                // If not, send REJ
                if(controlo == C_I0)
                {
                    if(enviaREJ0(fd) == -1)
                    {
                        printf("Ocorreu um erro a enviar a trama REJ0!\n");
                    }
                    else
                    {
                        //linklayer.stats.numSentREJ++;
                        printf("A trama REJ0 foi enviada corretamente!\n");
                    }
                    
                }
                else if(controlo == C_I1)
                {
                    if(enviaREJ1(fd) == -1)
                    {
                       printf("Ocorreu um erro a enviar a trama REJ1!\n");
                    }
                    else
                    {
                        //linklayer.stats.numSentREJ++;
                        printf("A trama REJ1 foi enviada com sucesso!\n");
                    }
                }

                return -1;
            }

            else 
            {
                // Preenche o buffer com os dados que foram destuffed
                for(i = 4; i< TamanhoDaTramaDestuffed - 2; i++)
                {
                    buffer[TamanhoDoPacote++] = tramad[i];
                }

                // Enviar o RR correto
                if(controlo == C_I0)
                {
                    if(enviaRR1(fd) == -1)
                    {
                        printf("Ocorreu um erro a enviar a trama RR1!\n");
                    }
                    else
                    {
                        //linklayer.stats.numSentRR++;
                        printf("A trama RR1 foi enviada com sucesso!\n");
                    }
                }
                else if(controlo == C_I1)
                {
                    if(enviaRR0(fd) == -1)
                    {
                        printf("Ocorreu um erro a enviar a trama RR0!\n");
                    }
                    else
                    {
                        //linklayer.stats.numSentRR++;
                        printf("A trama RR0 foi enviada corretamente!\n");
                    }
                }

                recebido = 1;
            }
            
        }
        
    }

    // Apos isto tudo, muda-se o numero de sequencia!
    linklayer.sequenceNumber^=0x01;

    return TamanhoDoPacote;
}

int llwrite(int fd, char* buffer, int length)
{
    do
    {
        // Envia a trama
        if (TramaStuffing(fd, buffer, length))
        {
            printf("A trama foi enviada!\n");
        }


        if(lerTramasRR_REJ(fd) == -1)
        {
            linklayer.alarm = 1;
            continue;
        }

    } while(linklayer.numTransmissions < MAX_TRANSMISSIONS && linklayer.alarm);

    paraAlarme();

    if(linklayer.numTransmissions >= MAX_TRANSMISSIONS)
    {
        printf("Foi atingido o numero maximo de tentativas!\n");
        return -1;
    }

    linklayer.numTransmissions=0;
    
    return length;
}