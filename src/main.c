#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <gpio.h>
#include <softPwm.h>
#include <time.h>

#include "pid.h"
#include "bme280.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"

int uart0_filestream;
float internalTemp, externalTemp;
float userTemp = 0;
int valueFan = 0;
int valueResistor = 0;
long millisAux = 0;
long millisCounter = 0;
// 1 ligado 0 desligado
int systemState = 0;
// 1 funcionando 0 parado
int funcState = 0;

int main () {
    signal(SIGINT, closeComponents);

    FILE *fp = fopen("report.csv", "w");
    fprintf(fp, "DATE,TEMP_INT,TEMP_EXT,TEMP_USR,VAL_FAN,VAL_RES;\n");
    fclose(fp);

    wirintPiSetup();
    if (wiringPiSetup () == -1) { 
        exit (1);
    }
    turnOffResistor();
    turnOffFan();
    initUart();
    bme_start();
    initMenu();
    return 0;
}

void initMenu() {
    int command;
    while(1) {
        requestToUart(uart0_filestream, GET_USER_CMD);
        command = readFromUart(uart0_filestream, GET_USER_CMD).int_value;
        readCommand(command);
        millisCounter = millis();
        if(millisCounter - millisAux > 1000){
            time_t rawtime;
            struct tm *timeinfo;
            time(&rawtime);
            timeinfo = localtime(&rawtime);
            FILE *fp = fopen("report.csv", "a");
            fprintf(fp, "%s,%.2f,%.2f,%.2f,%d,%d\n", asctime(timeinfo), internalTemp, externalTemp, userTemp, valueFan, valueResistor);
            fclose(fp);
            millisAux = millisCounter;
        }
        delay(500);
    };
}

void readCommand(int command) {
    switch(command) {
        case 1:
            printf("Ligando o forno");
            sendToUartByte(uart0_filestream, SEND_SYS_STATE, 1);
            systemState = 1;
            break;
        case 2:
            printf("Desligando o forno");
            sendToUartByte(uart0_filestream, SEND_SYS_STATE, 0);
            systemState = 0;
            break;
        case 3:
            printf("Iniciando aquecimento");
            sendToUartByte(uart0_filestream, SEND_FUNC_STATE, 1);
            funcState = 1;
            //calculo do pid em paralelo com acionamento de ventoinha e resistor
            break;
        case 4:
            printf("Cancelando aquecimento");
            sendToUartByte(uart0_filestream, SEND_FUNC_STATE, 0);
            funcState = 0;
            //calculo do pid em paralelo com acionamento de ventoinha e resistor
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
    turnOffResistor();
    turnOffFan();
    closeUart(uart0_filestream);
    exit(0);
}
