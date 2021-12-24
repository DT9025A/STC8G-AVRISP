#include "spi.h"

void SPI_Init()
{
    SPCTL = 0xD0 | SPI_CLK_DIV; //ʹ��SPI����ģʽ SSIG+SPEN+MSTR
    SPSTAT = 0xc0;              //���жϱ�־
}

unsigned char SPI_SendByte(unsigned char dat)
{
    SPDAT = dat;   //��������
    SPI_WAIT_BUSY; //�ȴ����ݴ������
    SPSTAT = 0xc0; //�����־λ
    return SPDAT;  //���شӻ���������
}
