/**
 *****************************************************************************
 * File Name    : _24LCXXX.cpp
 *
 * Title        : I2C EEPROM 24LCXXX Claass Source File
 * Revision     : 0.1
 * Notes        :
 * Target Board : mbed NXP LPC1768, mbed LPC1114FN28  etc
 * Tool Chain   : ????
 *
 * Revision History:
 * When         Who         Description of change
 * -----------  ----------- -----------------------
 * 2012/12/06   Hiroshi M   init
 *****************************************************************************
 *
 * Copyright (C) 2013 Hiroshi M, MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **/

/* Includes ----------------------------------------------------------------- */
#include "_24LCXXX.h"
#include "mbed.h"

/* Private typedef ---------------------------------------------------------- */
/* Private define ----------------------------------------------------------- */
/* Private macro ------------------------------------------------------------ */
/* Private variables -------------------------------------------------------- */

/* member fanctions --------------------------------------------------------- */

// Constractor
_24LCXXX::_24LCXXX(I2C *i2c, const int address):
    _i2c_address(address<<1), _i2c(i2c), _pc(NULL), _debug(false)
{
}

_24LCXXX::_24LCXXX(I2C *i2c, Serial *pc, const int address):
    _i2c_address(address<<1), _i2c(i2c), _pc(pc), _debug(true)
{
}

int _24LCXXX::byte_write( int mem_addr, char data )
{
    int res;
    char buf[3];

    buf[0] = (0xff00 & mem_addr)>>8;        // Write Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Write Address Low  byte set
    buf[2] = data;

    res = _i2c->write(_i2c_address, buf, sizeof(buf), false);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! Byte Data Write. \n");
        }
        else
        {
            _pc->printf("Failed! Byte Data Write %d.\n", res);
        }
    }

    wait_ms(5);         // 5mS

    return res;
}

int _24LCXXX::nbyte_write( int mem_addr, void *data, int size )
{
    int  i;
    int  res;
    char buf[3];
    char *p;

    p = (char *)data;
    res = -1;
    for ( i = 0; i < size; i++ )
    {
        buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
        buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set
        buf[2] = *p++;

        res = _i2c->write(_i2c_address, buf, sizeof(buf), false);
        if(_debug)
        {
            if(res==0)
            {
                _pc->printf("Success! N-Byte Data Write. \n");
            }
            else
            {
                _pc->printf("Failed! N-Byte Data Write %d.\n", res);
            }
        }

        if(res!=0)
        {
            return res;
        }

        wait_ms(5);         // 5mS

        if( ++mem_addr >= MAXADR_24LCXXX )      // Address counter +1
        {
            return -1;                          // Address range over
        }
    }

    return res;
}

int _24LCXXX::page_write( int mem_addr, char *data )
{
    int i;
    int res;
    char buf[PAGE_SIZE_24LCXXX+2];

    buf[0] = (0xff00 & mem_addr)>>8;        // Write Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Write Address Low  byte set

    for (i=0; i<PAGE_SIZE_24LCXXX; i++)
    {
        buf[i+2] = data[i];
    }
    res = _i2c->write(_i2c_address, buf, sizeof(buf), false);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! Page Data Write. \n");
        }
        else
        {
            _pc->printf("Failed! Page Data Write %d.\n", res);
        }
    }

    return res;
}


int _24LCXXX::nbyte_read( int mem_addr, void *data, int size )
{
    int res;
    char buf[2];

    buf[0] = (0xff00 & mem_addr)>>8;        // Read Address High byte set
    buf[1] = (0x00ff & mem_addr);           // Read Address Low  byte set

    res = _i2c->write(_i2c_address, buf, sizeof(buf), true);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! nbyte read address send. \n");
        }
        else
        {
            _pc->printf("Failed! nbyte read address send %d.\n", res);
        }
    }

    //
    res = _i2c->read(_i2c_address, (char *)data, size, false);
    if(_debug)
    {
        if(res==0)
        {
            _pc->printf("Success! nbyte read address send. \n");
        }
        else
        {
            _pc->printf("Failed! nbyte read address send %d.\n", res);
        }
    }

    return res;
}
