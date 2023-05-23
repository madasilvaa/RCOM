#include "alarm.h"

void gerirAlarme(int signal)
{
    if(signal != SIGALRM)
    {
        return;
    }

    printf("Alarm: %d\n", linklayer.numTransmissions + 1);

    linklayer.alarm = 1;
    linklayer.numTransmissions++;
}

// Set sigaction struct and linkLayer struct
void preparaAlarme()
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = &gerirAlarme;
    sa.sa_flags = 0;

    sigaction(SIGALRM, &sa, NULL);

    linklayer.alarm = 0;
}

void comecaAlarme()
{
    linklayer.alarm = 0;
    alarm(linklayer.timeout);
}

void paraAlarme()
{
    linklayer.alarm = 0;
    alarm(0);
}


void tiraAlarme()
{
    struct sigaction sa;
    sa.sa_handler = NULL;

    sigaction(SIGALRM, &sa, NULL);

    linklayer.alarm = 0;
}