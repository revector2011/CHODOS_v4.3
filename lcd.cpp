#include "mbed.h"
#include "lcd.h"

LCD::LCD(I2C & in_cI2C)
    : Serial(SERIAL_TX,SERIAL_RX)
    , m_cI2C(in_cI2C)
{
}
