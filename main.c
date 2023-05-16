#include "main.h"

int main(int argc, char** argv) {
	// Forma de compilar o nosso programa.
    if(argc != 4){
        printf("Usage: ./nome_compilar <receiver/transmitter> <destination/filename> <numero da porta: 0,1,10,11>\n");
	    exit(1);
    }

    int porta = atoi(argv[3]);
	if (porta!=0 && porta!=1 && porta!=10 && porta!=11)
	{
		printf("O numero da porta tera que ser 0, 1, 10 ou 11!\n");
		exit(1);
	}

	int status;
	if(strcmp("transmitter",argv[1])==0)
	{
		status = TRANSMITTER;
	}
	else if (strcmp("receiver",argv[1])==0)
	{
		status = RECEIVER;
	}
	else 
	{
		printf("Ocorreu um erro! Deveria estar no comando receiver ou transmitter. Tente de novo.\n");
		exit(1);
	}

	// Momento em que a ligação foi estabelecida.
    int fd;
	fd = llopen(porta, status);
	if(fd == -1)
	{
		printf("Ocorreu um erro na funcao llopen (falha no estabelecimento da ligacao).\n");
		return -1;
	}

	// Terminar a ligacao
	if(llclose(fd) == -1)
	{
		printf("Ocorreu um erro na funcao llclose (falha no termino da ligacao).\n");
		return -1;
	}

	
	return 0;
}