# ArduinoISP-STC8G

使用STC8G1K08实现的AVRISP/ArduinoISP

STK500协议部分参照了[ArduinoISP@rsbohn](https://github.com/rsbohn/ArduinoISP)

使用avrdudess实测两种模式均可用

---
#### 一些杂乱的说明

```
      STC8G1K08 引脚分配
         _________
   MOSI-| P54 P33 |-MISO
    VCC-|     P32 |-SCLK
    RST-| P55 P31 |-TXD
    GND-|     P30 |-RXD
         ---------
```

Program Size: data=54.0 xdata=403 code=2671

程序文件编码都是GBK

**烧写时振荡器频率选择5.5296MHz**

---

#### 文件夹结构

bin：存放一些编译出的BIN文件，但请到release页面进行下载

inc：头文件

src：程序文件

project：Keil UV4 工程文件（使用了STC提供的MCU Database - STC8G1K08 Series）

---

硬件实现：[立创开源广场](https://oshwhub.com/DT9025A/stc8-arduinoisp)

