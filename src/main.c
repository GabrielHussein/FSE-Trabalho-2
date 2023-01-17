#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <gpio.h>
#include <softPwm.h>

#include "pid.h"
#include "bme280.h"

int main () {
    signal(SIGINT, closeComponents);
    wirintPiSetup();
    if (wiringPiSetup () == -1) { 
        exit (1);
    }
    //setup de resistor de ventoinha (checar pinos correspondentes)
    //conectar uart (dev/serial0)
    //conectar bme/i2c (dev/i2c-1)
    initMenu();
    return 0;
}

void initMenu() {
    int command;
    while(1) {
        //le da uart e atribui o comando
        readCommand(command);
        delay(500);
    };
}

void readCommand(int command) {
    switch(command) {
        case 1:
            //ligar o forno
            break;
        case 2:
            //desligar o forno
            break;
        case 3:
            //iniciar aquecimento
            break;
        case 4:
            //cancela o processo
            break;
        case 5:
            //alterar o tipo entre referencia e curva
            break;
        default:
            break;
    }
}

void closeComponents() {
    system("clear");
    printf("Fechando todos os processos e componentes pendentes\n");
    //fechar todos os componentes (uart gpio e i2c) e desligar ventoinha e resistor de potência
    exit(0);
}
