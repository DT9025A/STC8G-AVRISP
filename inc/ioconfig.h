/************************
 * STC8系列 IO配置
 * @Author  DT9025A
 * @Date    21/1/15
 * @License Apache License 2.0
 * 
 * 本文件基于STC8H的文档编写, 根据目标平台自行适配
 ***********************/
#ifndef __STC8_IO_CONFIG_H__
#define __STC8_IO_CONFIG_H__

#include <STC8G.h>

#ifdef __STC8H_H_
#warning 默认头文件未更改, 目标平台为STC8H
#endif

//IO定义, 使用时将其加和
#define PIN_0 0x01
#define PIN_1 0x02
#define PIN_2 0x04
#define PIN_3 0x08
#define PIN_4 0x10
#define PIN_5 0x20
#define PIN_6 0x40
#define PIN_7 0x80
#define PIN_ALL 0xFF

//模式定义
#define PIN_MODE_STANDARD 0, 0
#define PIN_MODE_PUSHPULL 1, 0
#define PIN_MODE_HIRGRESIN 0, 1
#define PIN_MODE_OPENDRAIN 1, 1

//PULLUP状态
#define PIN_PULLUP_ENABLE 1
#define PIN_PULLUP_DISABLE 0

//NCS状态
#define PIN_NCS_ENABLE 0
#define PIN_NCS_DISABLE 1

//SR状态
#define PIN_SWITCHRATE_HIGH 0
#define PIN_SWITCHRATE_LOW 1

//DR状态
#define PIN_DRAINRATE_HIGH 0
#define PIN_DRAINRATE_LOW 1

//IE状态
#define PIN_DIGITALSIGINPUTE_DISABLE 0
#define PIN_DIGITALSIGINPUTE_ENABLE 1

//IO模式
#define PIN_MODE_CONFIG(port, pin, mode) _PINMODE_CONFIG(port, pin, mode)
#define _PINMODE_CONFIG(port, pin, stat0, stat1)      \
    stat0 ? (port##M0 |= pin) : (port##M0 &= (~pin)); \
    stat1 ? (port##M1 |= pin) : (port##M1 &= (~pin));

//上拉电阻设置, 进行设置前需先将P_SW2的最高位置1
#define PIN_PULLUP_CONFIG(port, pin, stat) \
    stat ? (port##PU |= pin) : (port##PU &= (~pin));

//施密特触发器设置, 进行设置前需先将P_SW2的最高位置1
#define PIN_NCS_CONFIG(port, pin, stat) \
    stat ? (port##NCS |= pin) : (port##NCS &= (~pin));

//端口转换速率设置, 进行设置前需先将P_SW2的最高位置1
#define PIN_SWITCHRATE_CONFIG(port, pin, stat) \
    stat ? (port##SR |= pin) : (port##SR &= (~pin));

//端口驱动电流设置, 进行设置前需先将P_SW2的最高位置1
#define PIN_DRAINRATE_CONFIG(port, pin, stat) \
    stat ? (port##DR |= pin) : (port##DR &= (~pin));

//数字信号使能设置, 进行设置前需先将P_SW2的最高位置1
#define PIN_DIGITALSIGINPUTENABLE_CONFIG(port, pin, stat) \
    stat ? (port##IE |= pin) : (port##IE &= (~pin));

#endif
