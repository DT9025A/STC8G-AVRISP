#include "delay.h"

void delay_() //1ms@5.5296MHz
{
    unsigned char i, j;

    _nop_();
    _nop_();
    i = 8;
    j = 43;
    do
    {
        while (--j)
            ;
    } while (--i);
}

void delay(unsigned int r) {
    while (r--)
    {
        delay_();
    }
}
