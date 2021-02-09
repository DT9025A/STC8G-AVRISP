#include "uart.h"

unsigned char xdata URAT_RECV_BUF[UART_RECV_BUFFER_SIZE]; //环形缓冲队列
unsigned char UART_RECV_PTR;                              //缓冲队列接收指针
unsigned char UART_RECV_READ_PTR;                         //缓冲队列已读取指针

void Uart_Init(void) //19200bps@5.5296MHz
{
    SCON = 0x50;  //8位数据,可变波特率
    AUXR |= 0x40; //定时器1时钟为Fosc,即1T
    AUXR &= 0xFE; //串口1选择定时器1为波特率发生器
    TMOD &= 0x0F; //设定定时器1为16位自动重装方式
    TL1 = 0xB8;   //设定定时初值
    TH1 = 0xFF;   //设定定时初值
    ET1 = 0;      //禁止定时器1中断
    TR1 = 1;      //启动定时器1
    ES = 1;       //允许串口中断

    UART_RECV_PTR = 0;
    UART_RECV_READ_PTR = 0;
}

void Uart_ISR() interrupt 4
{
    if (RI) //接受数据
    {
        RI = 0;                                     //清中断标志
        if (UART_RECV_PTR == UART_RECV_BUFFER_SIZE) //队列满
            UART_RECV_PTR = 0;                      //重置环形队列
        URAT_RECV_BUF[UART_RECV_PTR++] = SBUF;      //放入数据
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
    //如果接收指针大于读取指针, 表明一定有未读取数据
    if (UART_RECV_PTR > UART_RECV_READ_PTR)
        return 1;
    //如果接收指针等于读取指针, 表明数据读取完成
    else if (UART_RECV_PTR == UART_RECV_READ_PTR)
        return 0;
    //如果接收指针小于读取指针, 表明环形队列重置, 队列首有未读取数据
    else
        return 1;
}

unsigned char Uart_Getch()
{
    while (!Uart_Available()) //等待有效数据
        ;
    if (UART_RECV_READ_PTR == UART_RECV_BUFFER_SIZE) //读取到队列尾部
        UART_RECV_READ_PTR = 0;                      //重置读取指针
    return URAT_RECV_BUF[UART_RECV_READ_PTR++];      //读取数据
}
