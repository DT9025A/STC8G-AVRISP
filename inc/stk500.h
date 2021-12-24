#ifndef __STK500_H__
#define __STK500_H__

#include "command.h"
#include "delay.h"
#include "spi.h"
#include "uart.h"

#define Hardware_Version 1        //硬件版本
#define Software_Major_Version 1  //软件主要版本
#define Software_Minor_Version 20 //软件次要版本

#define EECHUNK (32)                                    //EEPROM块大小
#define GET_WORD_DATA(addr) (*addr * 256 + *(addr + 1)) //从当前地址的两字节合成一字(16位)的数据, 高字节在前

sbit MOSI = P5 ^ 4;
sbit RESET = P5 ^ 5;
sbit MISO = P3 ^ 3;
sbit SCLK = P3 ^ 2;

//器件设置, 参照 <AVR061: STK500 Communication Protocol> 的第五页
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

void ISP_Process_Data(); //处理数据

#endif