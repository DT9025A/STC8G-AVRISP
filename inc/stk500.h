#ifndef __STK500_H__
#define __STK500_H__

#include "command.h"
#include "delay.h"
#include "spi.h"
#include "uart.h"

#define Hardware_Version 1        //Ӳ���汾
#define Software_Major_Version 1  //�����Ҫ�汾
#define Software_Minor_Version 20 //�����Ҫ�汾

#define EECHUNK (32)                                    //EEPROM���С
#define GET_WORD_DATA(addr) (*addr * 256 + *(addr + 1)) //�ӵ�ǰ��ַ�����ֽںϳ�һ��(16λ)������, ���ֽ���ǰ

sbit MOSI = P5 ^ 4;
sbit RESET = P5 ^ 5;
sbit MISO = P3 ^ 3;
sbit SCLK = P3 ^ 2;

//��������, ���� <AVR061: STK500 Communication Protocol> �ĵ���ҳ
typedef struct
{
    unsigned char devicecode;
    unsigned char revision;
    unsigned char progtype;
    unsigned char parmode;
    unsigned char polling;
    unsigned char selftimed;
    unsigned char lockbytes;
    unsigned char fusebytes;
    unsigned char flashpoll;
    unsigned int eeprompoll;
    unsigned int pagesize;
    unsigned int eepromsize;
    unsigned long flashsize;
} ISP_Parameter;

void ISP_Process_Data(); //��������

#endif