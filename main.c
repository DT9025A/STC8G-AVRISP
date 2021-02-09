/**
 *          STC8G1K08 AVRISP
 * 
 *            License: BSD
 *   Copyright (c) 2008-2011 Randall Bohn
 *        Modified by: DT9025A
 *           Date: 2021/2/9
 * ---------------------------------------
 * 
 * 系统时钟 = 5.5296MHz
 * SPI速度 = 系统时钟 / 32 = 172.8KHz
 * 参照: ArduinoISP.ino(44-52)
 * 
 * UART速率:19200bps
 * 参照: ArduinoISP.ino(138)
 * 
 * ---------------------------------------
 *     STC8G1K08 引脚分配
 *        _________
 *  MOSI-| P54 P33 |-MISO
 *   VCC-|     P32 |-SCLK
 *   RST-| P55 P31 |-TXD
 *   GND-|     P30 |-RXD
 *        ---------
 */

#include "ioconfig.h"
#include "stk500.h"
#include "uart.h"

void main()
{
    //配置引脚模式
    PIN_MODE_CONFIG(P3, PIN_ALL, PIN_MODE_STANDARD);
    PIN_MODE_CONFIG(P5, PIN_ALL, PIN_MODE_STANDARD);

    Uart_Init();

    EA = 1; //开总中断

    while (1)
    {
        if (Uart_Available())
        {
            ISP_Process_Data(); //处理数据
        }
    }
}
