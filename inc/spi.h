#ifndef __SPI_H__
#define __SPI_H__

#include <STC8G.H>

#define SPI_CLK_DIV 3                          //SPI时钟32分频
#define SPI_WAIT_BUSY while (!(SPSTAT & 0x80)) //等待发送完成

void SPI_Init();                               //初始化SPI
unsigned char SPI_SendByte(unsigned char dat); //SPI发送字节

#endif