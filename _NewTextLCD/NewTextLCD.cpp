/* mbed TextLCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, sford, http://mbed.org
 * Changes by Erik Kerger
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "NewTextLCD.h"
#include "mbed.h"

TextLCD::TextLCD(PinName rs,PinName e, PinName d4, PinName d5,
                 PinName d6, PinName d7, LCDType type) : _rs(rs), _e(e), 
                    _d(d4, d5, d6, d7), _type(type) {
    _rs = 0;            // command mode
    _e  = 0;            // 0 --> inactive (most important change to original TextLCD)
    
    if (_type != LCDuser)
        setLCDparam(_type); // otherwise rows, colums, comdelay, adresses must be set before

    wait(0.050f);       // Wait 50ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    // this sets controler into 8 bit mode, so we have a defined state
    for (int i=0; i<3; i++) {
        _e = 1;
        __nop();
        __nop();
        _d = 0x3;
        __nop();        // data setup time at least 60 ns
        _e = 0;
        wait(0.005f);   // 5ms
       }
    _e = 1;
    __nop();
    __nop();
    _d = 0x2;           // 4 Bit mode (after this command, we have to send 2 nibbles)
    __nop(); 
    _e = 0;
    wait_us(LCDparam.delay);
    
    writeCommand(0x28); // Function set 4 Bit, 2Line, 5*7
    writeCommand(0x08); // Display off
    cls();              // clear display, reset _column and _row
    writeCommand(0x04); // cursor right, Display is not shifted
    writeCommand(0x0C); // Display on , Cursor off 
}

void TextLCD::character(int column, int row, int c) {
    int a = 0x80 | (LCDparam.adresses[row & 3] + column);
    writeCommand(a);    // set cursor address
    writeData(c);       // write char
}

void TextLCD::cls() {
    writeCommand(0x01); // cls, and set cursor to 0
    locate(0, 0);       // set internal position
    wait_us(45 * LCDparam.delay);   // CLS need much time
}

void TextLCD::locate(int column, int row) {
    _column = column;   // set position for next char
    _row = row;         // note: cursor is not set yet
}



int TextLCD::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row++;
        if (_row >= LCDparam.rows) {
            _row = 0;
        }
    } else {
        character(_column, _row, value);
        _column++;
        if (_column >= LCDparam.columns) {
            _column = 0;
            _row++;
            if (_row >= LCDparam.rows) {
                _row = 0;
            }
        }
    }
    return value;
}

// Dummy function - read not supported
int TextLCD::_getc() {
    return -1;
}

void TextLCD::writeByte(int value) {
    _e = 1;
    __nop();    
    _d = value >> 4;        // higher nibble first
    __nop();
    _e = 0;
    __nop();
    __nop();
    _e = 1;
    __nop();
    _d = value >> 0;        // then lower
    __nop();
    _e = 0;
}

void TextLCD::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
    wait_us(LCDparam.delay);
}


void TextLCD::writeData(int data) {
    _rs = 1;
    writeByte(data);
    wait_us(LCDparam.delay);
}


// set user defined char 
void  TextLCD::writeCGRAM(int address, int pattern[8]){
    int i;
    address = address & 7;  //max 8 char
    for(i=0;i<8;i++){
        writeCommand(0x40 | (address * 8) + i);
        writeData(pattern[i]);
        }
}   

void TextLCD::setLCDparam(LCDType _type){
    switch (_type) {
    
        case LCD16x2:
        case LCD16x2B:
            LCDparam.columns = 16;
            break;
        case LCD20x2:
        case LCD20x4:
            LCDparam.columns = 20;
            break;
        case LCD24x2:
            LCDparam.columns = 24;
            break;
    }
    if (_type == LCD20x4) 
        LCDparam.rows = 4;
    else 
        LCDparam.rows = 2;
        
    LCDparam.adresses[0] = 0;
    
    if (_type == LCD16x2B)
        LCDparam.adresses[1] = 40;
    else
        LCDparam.adresses[1] = 0x40;
        
    if (_type == LCD20x4) {
        LCDparam.adresses[2] = 0x14;
        LCDparam.adresses[3] = 0x54;}
    else {
        LCDparam.adresses[2] = 0;
        LCDparam.adresses[3] = 0;}
        
    LCDparam.delay = 50;            // 50 us delays as default              
}
        
