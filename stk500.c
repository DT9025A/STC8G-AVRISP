#include "stk500.h"

unsigned char ISP_Error_Count = 0;        //错误计数
unsigned char ISP_PROGMODE = 0;           //是否在编程模式
unsigned char ISP_Reset_Active_High = 0;  //是否高电平复位
unsigned char xdata ISP_Data_Buffer[256]; //数据缓冲区
unsigned long ISP_Operating_Addr = 0L;    //正在操作的地址
ISP_Parameter xdata ISP_Program_Param;    //器件选项

//给器件发送一个数据包/4字节
unsigned char ISP_Trans_Package(unsigned char a, unsigned char b, unsigned char c, unsigned char d)
{
    SPI_SendByte(a);
    SPI_SendByte(b);
    SPI_SendByte(c);
    return SPI_SendByte(d);
}

//发送空回复/INSYNC+OK/NOSYNC
void ISP_Send_Empty_Reply()
{
    if (Sync_CRC_EOP == Uart_Getch())
    {
        Uart_Send(Resp_STK_INSYNC);
        Uart_Send(Resp_STK_OK);
    }
    else
    {
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
    }
}

//发送一字节回复
void ISP_Send_Byte_Reply(unsigned char b)
{
    if (Sync_CRC_EOP == Uart_Getch())
    {
        Uart_Send(Resp_STK_INSYNC);
        Uart_Send(b);
        Uart_Send(Resp_STK_OK);
    }
    else
    {
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
    }
}

//STK编程器信息
void ISP_Get_Device_Parameter(unsigned char c)
{
    switch (c)
    {
    case Parm_STK_HW_VER:
        ISP_Send_Byte_Reply(Hardware_Version);
        break;
    case Parm_STK_SW_MAJOR:
        ISP_Send_Byte_Reply(Software_Major_Version);
        break;
    case Parm_STK_SW_MINOR:
        ISP_Send_Byte_Reply(Software_Minor_Version);
        break;
    case Parm_STK_PROGMODE:
        ISP_Send_Byte_Reply('S');
        break;
    default:
        ISP_Send_Byte_Reply(0x00);
    }
}

//从UART缓冲区读取数据并存储至数据缓冲区
void ISP_Fill_Buffer(unsigned char n)
{
    unsigned char x;
    for (x = 0; x < n; x++)
    {
        ISP_Data_Buffer[x] = Uart_Getch();
    }
}

//设置器件选项
void ISP_Set_Program_Parameters()
{
    ISP_Program_Param.devicecode = ISP_Data_Buffer[0];
    ISP_Program_Param.revision = ISP_Data_Buffer[1];
    ISP_Program_Param.progtype = ISP_Data_Buffer[2];
    ISP_Program_Param.parmode = ISP_Data_Buffer[3];
    ISP_Program_Param.polling = ISP_Data_Buffer[4];
    ISP_Program_Param.selftimed = ISP_Data_Buffer[5];
    ISP_Program_Param.lockbytes = ISP_Data_Buffer[6];
    ISP_Program_Param.fusebytes = ISP_Data_Buffer[7];
    ISP_Program_Param.flashpoll = ISP_Data_Buffer[8];
    // ignore buff[9] (= buff[8])

    // following are 16 bits (big endian)
    ISP_Program_Param.eeprompoll = GET_WORD_DATA(&ISP_Data_Buffer[10]);
    ISP_Program_Param.pagesize = GET_WORD_DATA(&ISP_Data_Buffer[12]);
    ISP_Program_Param.eepromsize = GET_WORD_DATA(&ISP_Data_Buffer[14]);

    // 32 bits flashsize (big endian)
    ISP_Program_Param.flashsize = ISP_Data_Buffer[16] * 0x01000000 + ISP_Data_Buffer[17] * 0x00010000 + ISP_Data_Buffer[18] * 0x00000100 + ISP_Data_Buffer[19];

    // AVR devices have active low reset, AT89Sx are active high
    ISP_Reset_Active_High = (ISP_Program_Param.devicecode >= 0xe0);
}

//复位器件
void ISP_Reset_Target(unsigned char reset)
{
    RESET = ((reset && ISP_Reset_Active_High) || (!reset && !ISP_Reset_Active_High)) ? 1 : 0;
}

//进入器件编程模式
void ISP_Start_Programming_Mode()
{
    // Reset target before driving PIN_SCK or PIN_MOSI

    // SPI.begin() will configure SS as output, so SPI master mode is selected.
    // We have defined RESET as pin 10, which for many Arduinos is not the SS pin.
    // So we have to configure RESET as output here,
    // (ISP_Reset_Target() first sets the correct level)
    ISP_Reset_Target(1);
    SPI_Init();

    // See AVR datasheets, chapter "SERIAL_PRG Programming Algorithm":

    // Pulse RESET after PIN_SCK is low:
    SCLK = 0;
    delay(20); // discharge PIN_SCK, value arbitrarily chosen
    ISP_Reset_Target(0);
    // Pulse must be minimum 2 target CPU clock cycles so 100 usec is ok for CPU
    // speeds above 20 KHz
    delay(1);
    ISP_Reset_Target(1);

    delay(50);

    ISP_Trans_Package(0xAC, 0x53, 0x00, 0x00);

    ISP_PROGMODE = 1;
}

//退出器件编程模式
void ISP_End_Programming_Mode()
{
    //SPI.end();
    // We're about to take the target out of reset so configure SPI pins as input
    //pinMode(PIN_MOSI, INPUT);
    //pinMode(PIN_SCK, INPUT);
    ISP_Reset_Target(0);

    //pinMode(RESET, INPUT);
    ISP_PROGMODE = 0;
}

//获取当前操作的内存页
unsigned long ISP_Get_Current_Page()
{
    if (ISP_Program_Param.pagesize == 32)
    {
        return ISP_Operating_Addr & 0xFFFFFFF0;
    }
    if (ISP_Program_Param.pagesize == 64)
    {
        return ISP_Operating_Addr & 0xFFFFFFE0;
    }
    if (ISP_Program_Param.pagesize == 128)
    {
        return ISP_Operating_Addr & 0xFFFFFFC0;
    }
    if (ISP_Program_Param.pagesize == 256)
    {
        return ISP_Operating_Addr & 0xFFFFFF80;
    }
    return ISP_Operating_Addr;
}

//生效更改
void ISP_Commit(unsigned long addr)
{
    ISP_Trans_Package(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

//编程一字节
void ISP_Flash_Byte(unsigned char hilo, unsigned int addr, unsigned char dat)
{
    ISP_Trans_Package(0x40 + 8 * hilo,
                      addr >> 8 & 0xFF,
                      addr & 0xFF,
                      dat);
}

//写数据到FLASH页
unsigned char ISP_Write_Flash_Page(unsigned long length)
{
    long x = 0;
    unsigned long page = ISP_Get_Current_Page();

    while (x < length)
    {
        if (page != ISP_Get_Current_Page())
        {
            ISP_Commit(page);
            page = ISP_Get_Current_Page();
        }
        ISP_Flash_Byte(0, ISP_Operating_Addr, ISP_Data_Buffer[x++]);
        ISP_Flash_Byte(1, ISP_Operating_Addr, ISP_Data_Buffer[x++]);
        ISP_Operating_Addr++;
    }

    ISP_Commit(page);

    return Resp_STK_OK;
}

//写数据到FLASH
void ISP_Write_Flash(unsigned int length)
{
    ISP_Fill_Buffer(length);
    if (Sync_CRC_EOP == Uart_Getch())
    {
        Uart_Send(Resp_STK_INSYNC);
        Uart_Send(ISP_Write_Flash_Page(length));
    }
    else
    {
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
    }
}

//写EEPROM区块
unsigned char ISP_Write_EEPROM_Chunk(unsigned long start, unsigned long length)
{
    // write (length) bytes, (start) is a byte address
    // this writes byte-by-byte, page writing may be faster (4 bytes at a time)
    unsigned long x, addr;

    ISP_Fill_Buffer(length);
    for (x = 0; x < length; x++)
    {
        addr = start + x;
        ISP_Trans_Package(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, ISP_Data_Buffer[x]);
        delay(45);
    }
    return Resp_STK_OK;
}

//写EEPROM
unsigned char ISP_Write_EEPROM(unsigned long length)
{
    // here is a word address, get the byte address
    unsigned long start = ISP_Operating_Addr * 2;
    unsigned long remaining = length;

    if (length > ISP_Program_Param.eepromsize)
    {
        ISP_Error_Count++;
        return Resp_STK_FAILED;
    }

    while (remaining > EECHUNK)
    {
        ISP_Write_EEPROM_Chunk(start, EECHUNK);
        start += EECHUNK;
        remaining -= EECHUNK;
    }

    ISP_Write_EEPROM_Chunk(start, remaining);

    return Resp_STK_OK;
}

//页编程
void ISP_Program_Page()
{
    unsigned char result;
    unsigned char memtype;
    unsigned int length;

    result = Resp_STK_FAILED;
    length = 256 * Uart_Getch();
    length += Uart_Getch();
    memtype = Uart_Getch();

    // flash memory @here, (length) bytes
    if (memtype == 'F')
    {
        ISP_Write_Flash(length);
        return;
    }
    if (memtype == 'E')
    {
        result = (char)ISP_Write_EEPROM(length);
        if (Sync_CRC_EOP == Uart_Getch())
        {
            Uart_Send(Resp_STK_INSYNC);
            Uart_Send(result);
        }
        else
        {
            ISP_Error_Count++;
            Uart_Send(Resp_STK_NOSYNC);
        }
        return;
    }
    Uart_Send(Resp_STK_FAILED);

    return;
}

//读取FLASH字节
unsigned char ISP_Read_Flash_Byte(unsigned char hilo, unsigned int addr)
{
    return ISP_Trans_Package(0x20 + hilo * 8,
                             (addr >> 8) & 0xFF,
                             addr & 0xFF,
                             0);
}

//读取FLASH页
unsigned char ISP_Read_Flash_Page(long length)
{
    long x;
    unsigned char low, high;

    for (x = 0; x < length; x += 2)
    {
        low = ISP_Read_Flash_Byte(0, ISP_Operating_Addr);
        Uart_Send(low);
        high = ISP_Read_Flash_Byte(1, ISP_Operating_Addr);
        Uart_Send(high);
        ISP_Operating_Addr++;
    }
    return Resp_STK_OK;
}

//读取EEPROM页
unsigned char ISP_Read_EEPROM_Page(long length)
{
    // here again we have a word address
    long start, x, addr;
    unsigned char ee;

    start = ISP_Operating_Addr * 2;
    for (x = 0; x < length; x++)
    {
        addr = start + x;

        ee = ISP_Trans_Package(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);

        Uart_Send(ee);
    }
    return Resp_STK_OK;
}

//读取页
void ISP_Read_Page()
{
    unsigned char result;
    unsigned char memtype;
    unsigned int length;

    result = Resp_STK_FAILED;
    length = 256 * Uart_Getch();
    length += Uart_Getch();
    memtype = Uart_Getch();

    if (Sync_CRC_EOP != Uart_Getch())
    {
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
        return;
    }
    Uart_Send(Resp_STK_INSYNC);
    if (memtype == 'F')
        result = ISP_Read_Flash_Page(length);
    if (memtype == 'E')
        result = ISP_Read_EEPROM_Page(length);
    Uart_Send(result);
}

//直接传输数据到器件
void ISP_Trans_Universal()
{
    unsigned char ch;

    ISP_Fill_Buffer(4);

    ch = ISP_Trans_Package(ISP_Data_Buffer[0], ISP_Data_Buffer[1], ISP_Data_Buffer[2], ISP_Data_Buffer[3]);

    ISP_Send_Byte_Reply(ch);
}

//读取器件签名
void ISP_Read_Signature()
{
    unsigned char high, middle, low;

    if (Sync_CRC_EOP != Uart_Getch())
    {
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
        return;
    }
    Uart_Send(Resp_STK_INSYNC);
    high = ISP_Trans_Package(0x30, 0x00, 0x00, 0x00);
    Uart_Send(high);
    middle = ISP_Trans_Package(0x30, 0x00, 0x01, 0x00);
    Uart_Send(middle);
    low = ISP_Trans_Package(0x30, 0x00, 0x02, 0x00);
    Uart_Send(low);
    Uart_Send(Resp_STK_OK);
}

//处理数据
void ISP_Process_Data()
{
    unsigned char ch;

    ch = Uart_Getch();
    switch (ch)
    {
    case Cmnd_STK_GET_SYNC:
        //'0'
        ISP_Error_Count = 0;
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_GET_SIGN_ON:
        //'1'
        if (Uart_Getch() == Sync_CRC_EOP)
        {
            Uart_Send(Resp_STK_INSYNC);
            Uart_SendString(STK_SIGN_ON_MESSAGE);
            Uart_Send(Resp_STK_OK);
        }
        else
        {
            ISP_Error_Count++;
            Uart_Send(Resp_STK_NOSYNC);
        }
        break;

    case Cmnd_STK_GET_PARAMETER:
        //'A'
        ISP_Get_Device_Parameter(Uart_Getch());
        break;

    case Cmnd_STK_SET_DEVICE:
        //'B'
        ISP_Fill_Buffer(20);
        ISP_Set_Program_Parameters();
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_SET_DEVICE_EXT:
        //'E'
        ISP_Fill_Buffer(5);
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_ENTER_PROGMODE:
        //'P'
        if (!ISP_PROGMODE)
            ISP_Start_Programming_Mode();
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_LOAD_ADDRESS:
        //'U'
        ISP_Operating_Addr = Uart_Getch();
        ISP_Operating_Addr += 256 * Uart_Getch();
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_PROG_FLASH:
        //0x60
        Uart_Getch(); // low addr
        Uart_Getch(); // high addr
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_PROG_DATA:
        //0x61
        Uart_Getch(); // data
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_PROG_PAGE:
        //0x64
        ISP_Program_Page();
        break;

    case Cmnd_STK_READ_PAGE:
        //'t'
        ISP_Read_Page();
        break;

    case Cmnd_STK_UNIVERSAL:
        //'V'
        ISP_Trans_Universal();
        break;

    case Cmnd_STK_LEAVE_PROGMODE:
        //'Q'
        ISP_Error_Count = 0;
        ISP_End_Programming_Mode();
        ISP_Send_Empty_Reply();
        break;

    case Cmnd_STK_READ_SIGN:
        //'u'
        ISP_Read_Signature();
        break;

    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case Sync_CRC_EOP:
        ISP_Error_Count++;
        Uart_Send(Resp_STK_NOSYNC);
        break;

    // anything else we will return STK_UNKNOWN
    default:
        ISP_Error_Count++;
        if (Sync_CRC_EOP == Uart_Getch())
            Uart_Send(Resp_STK_UNKNOWN);
        else
            Uart_Send(Resp_STK_NOSYNC);
    }
}
