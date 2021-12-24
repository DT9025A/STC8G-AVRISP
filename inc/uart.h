#ifndef __UART_H__
#define __UART_H__

#include <STC8G.h>
#include "ioconfig.h"

//UART 接收缓冲区大小
#define UART_RECV_BUFFER_SIZE 128

void Uart_Init(unsigned char isArduinoISP); //初始化UART
void Uart_Send(unsigned char dat);          //发送一字节
void Uart_SendString(char *dat);            //发送字符串
unsigned char Uart_Getch();                 //从缓冲区读取一字节, 缓冲区无内容则阻塞
bit Uart_Available();                       //缓冲区是否有待读取内容

#endif
