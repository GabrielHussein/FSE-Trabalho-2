#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wiringPi.h>
#include <softPwm.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <unistd.h>

#include "pid.h"
#include "bme280.h"
#include "gpio.h"
#include "uart.h"
#include "i2c.h"
#include "displaylcd.h"

int uart0_filestream;
float internalTemp = 0;
float externalTemp = 0;
float userTemp = 0;
int valueFan = 0;
int valueResistor = 0;
long millisAux = 0;
long millisCounter = 0;
// 1 ligado 0 desligado
int systemState = 0;
// 1 funcionando 0 parado
int funcState = 0;
float ki = 0;
float kp = 0;
float kd = 0;
pthread_t ovenThread;
pthread_t reportThread;
struct bme280_dev bme;


void *writeReport(void *arg) {
    while(1){
        time_t rawtime;
        struct tm *timeinfo;
        time(&rawtime);
        timeinfo = localtime(&rawtime);
        FILE *fp = fopen("report.csv", "a");
        fprintf(fp, "%s,%.2f,%.2f,%.2f,%d,%d\n", asctime(timeinfo), internalTemp, externalTemp, userTemp, valueFan, valueResistor);
        fclose(fp);
        millisAux = millisCounter;
        sleep(1);
    }
}

void *controlTemp(void *arg) {
    system("clear");
    float TI, TR, TE;
    pidSetupConstants(30.0, 0.2, 400.0);
    do {
        requestToUart(uart0_filestream, GET_INTERNAL_TEMP);
        TI = readFromUart(uart0_filestream, GET_INTERNAL_TEMP).float_value;
        internalTemp = TI;
        double value = pidControl(TI);
        pwmControl(value);
        if (value > 0) {
            valueFan = 0;
            valueResistor = value;
        } else {
            if (value <= -40) {
                valueResistor = 0;
                valueFan = value * -1;
            } else {
                valueFan = 0;
            }
        }

        requestToUart(uart0_filestream, GET_REF_TEMP);
        TR = readFromUart(uart0_filestream, GET_REF_TEMP).float_value;
        userTemp = TR;
        pidUpdateReferences(TR);

        TE = stream_sensor_data_normal_mode(&bme);
        externalTemp = TE;
        printf("\nTemperaturas\nInterna: %.2f\nReferencia: %.2f\nExterna(I2C): %.2f\n", TI, TR, TE);

        printTemp(TI, TE, TR);

        if(TR > TI){
            turnOnResistor(100);
            turnOffFan();
            value = 100;
            sendToUart(uart0_filestream, SEND_CTRL_SIGNAL, value);
        } else if(TR <= TI) {
            turnOffResistor();
            turnOnFan(100);
            value = -100;
            sendToUart(uart0_filestream, SEND_CTRL_SIGNAL, value);
        }
    } while (funcState == 1);
    pthread_exit(0);
}

void initMenu() {
    printf("\nIniciando menu!\n");
    int command;
    pthread_create(&reportThread, NULL, writeReport, NULL);
    while(1) {
        requestToUart(uart0_filestream, GET_USER_CMD);
        command = readFromUart(uart0_filestream, GET_USER_CMD).int_value;
	printf("comando %d\n", command);
        readCommand(command);
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
            pthread_create(&ovenThread, NULL, controlTemp, NULL);
            break;
        case 4:
            printf("Cancelando aquecimento");
            sendToUartByte(uart0_filestream, SEND_FUNC_STATE, 0);
            funcState = 0;
            break;
        case 5:
            //alterar o tipo entre referencia e curva
            break;
        default:
            break;
    }
}

void closeComponents() {
    printf("\n\n\n\nFechando todos os processos e componentes pendentes\n");
    turnOffResistor();
    turnOffFan();
    closeUart(uart0_filestream);
    pthread_cancel(reportThread);
    exit(0);
}

int main () {

    FILE *fp = fopen("report.csv", "w+");
    fprintf(fp, "DATE,TEMP_INT,TEMP_EXT,TEMP_USR,VAL_FAN,VAL_RES;\n");
    fclose(fp);
    if (wiringPiSetup () == -1) { 
        exit (1);
    }
    turnOffResistor();
    turnOffFan();
    initUart();
    bme = bme_start();
    signal(SIGINT, closeComponents);
    initMenu();
    return 0;
}
