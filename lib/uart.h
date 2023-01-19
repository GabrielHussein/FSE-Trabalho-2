#ifndef UART_H
#define UART_H

#define GET_INTERNAL_TEMP 0xC1
#define GET_REF_TEMP 0xC2
#define GET_USER_CMD 0xC3
#define SEND_CTRL_SIGNAL 0xD1
#define SEND_REF_SIGNAL 0xD2
#define SEND_SYS_STATE 0xD3
#define TEMP_CTRL_MODE 0xD4
#define SEND_FUNC_STATE 0xD5
#define SEND_AMBIENT_TEMP 0xD6

typedef struct Number_type {
  int int_value;
  float float_value;
} Number_type;

int initUart();
void requestToUart(int uart_filestream, unsigned char code);
void sendToUart(int uart_filestream, unsigned char code, int value);
void sendToUartByte(int uart_filestream, unsigned char code, char value);
Number_type readFromUart(int uart_filestream, unsigned char code);
void closeUart(int uart_filestream);

#endif