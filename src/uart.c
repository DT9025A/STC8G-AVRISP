#include "uart.h"

unsigned char xdata URAT_RECV_BUF[UART_RECV_BUFFER_SIZE]; //���λ������
unsigned char UART_RECV_PTR;                              //������н���ָ��
unsigned char UART_RECV_READ_PTR;                         //��������Ѷ�ȡָ��

void Uart_Init(unsigned char isArduinoISP) //115200bps@5.5296MHz
{
    SCON = 0x50;  //8λ����,�ɱ䲨����
    AUXR |= 0x40; //��ʱ��1ʱ��ΪFosc,��1T
    AUXR &= 0xFE; //����1ѡ��ʱ��1Ϊ�����ʷ�����
    TMOD &= 0x0F; //�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
    if (isArduinoISP)
        TL1 = 0xB8; //19200bps@5.5296MHz
    else
        TL1 = 0xF4; //115200bps@5.5296MHz
    TH1 = 0xFF;     //�趨��ʱ��ֵ
    ET1 = 0;        //��ֹ��ʱ��1�ж�
    TR1 = 1;        //������ʱ��1
    ES = 1;         //�������ж�

    UART_RECV_PTR = 0;
    UART_RECV_READ_PTR = 0;
}

void Uart_ISR() interrupt 4
{
    if (RI) //��������
    {
        RI = 0;                                     //���жϱ�־
        if (UART_RECV_PTR == UART_RECV_BUFFER_SIZE) //������
            UART_RECV_PTR = 0;                      //���û��ζ���
        URAT_RECV_BUF[UART_RECV_PTR++] = SBUF;      //��������
    }
}

void Uart_Send(unsigned char dat)
{
    SBUF = dat;
    while (TI == 0)
        ;
    TI = 0;
}

void Uart_SendString(char *s)
{
    while (*s)
    {
        Uart_Send(*s++);
    }
}

bit Uart_Available()
{
    //�������ָ����ڶ�ȡָ��, ����һ����δ��ȡ����
    if (UART_RECV_PTR > UART_RECV_READ_PTR)
        return 1;
    //�������ָ����ڶ�ȡָ��, �������ݶ�ȡ���
    else if (UART_RECV_PTR == UART_RECV_READ_PTR)
        return 0;
    //�������ָ��С�ڶ�ȡָ��, �������ζ�������, ��������δ��ȡ����
    else
        return 1;
}

unsigned char Uart_Getch()
{
    while (!Uart_Available()) //�ȴ���Ч����
        ;
    if (UART_RECV_READ_PTR == UART_RECV_BUFFER_SIZE) //��ȡ������β��
        UART_RECV_READ_PTR = 0;                      //���ö�ȡָ��
    return URAT_RECV_BUF[UART_RECV_READ_PTR++];      //��ȡ����
}
