#ifndef __SPI_H__
#define __SPI_H__

#include <STC8G.H>

#define SPI_CLK_DIV 3                          //SPIʱ��32��Ƶ
#define SPI_WAIT_BUSY while (!(SPSTAT & 0x80)) //�ȴ��������

void SPI_Init();                               //��ʼ��SPI
unsigned char SPI_SendByte(unsigned char dat); //SPI�����ֽ�

#endif