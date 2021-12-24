#ifndef __UART_H__
#define __UART_H__

#include <STC8G.h>
#include "ioconfig.h"

//UART ���ջ�������С
#define UART_RECV_BUFFER_SIZE 128

void Uart_Init(unsigned char isArduinoISP); //��ʼ��UART
void Uart_Send(unsigned char dat);          //����һ�ֽ�
void Uart_SendString(char *dat);            //�����ַ���
unsigned char Uart_Getch();                 //�ӻ�������ȡһ�ֽ�, ������������������
bit Uart_Available();                       //�������Ƿ��д���ȡ����

#endif
