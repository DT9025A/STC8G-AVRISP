# ArduinoISP-STC8G

使用STC8G1K08A实现的[ArduinoISP@rsbohn](https://github.com/rsbohn/ArduinoISP)

实测可用

---
一些杂乱的说明：

```
      STC8G1K08 引脚分配
         _________
   MOSI-| P54 P33 |-MISO
    VCC-|     P32 |-SCLK
    RST-| P55 P31 |-TXD
    GND-|     P30 |-RXD
         ---------
```

Program Size: data=54.0 xdata=403 const=0 code=2629

程序文件编码都是GBK

---

文件内容：

command：STK500协议常量定义

delay：延时函数

ioconfig：IO端口模式设置

main：主逻辑

spi：SPI接口操作相关

stk500：STK500协议实现

uart：串口相关

