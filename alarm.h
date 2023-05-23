#pragma once
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include "funcoes_ll.h"

void gerirAlarme(int signal);
void preparaAlarme();
void comecaAlarme();
void paraAlarme();
void tiraAlarme();