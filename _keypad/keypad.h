/*	 Copyright (c) 2010 Dimiter Kentri

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef KEYPAD_H
#define KEYPAD_H

#include "DigitalIn.h"
#include "BusOut.h"


namespace mbed{

const char NO_KEY = '\0';
#define KEY_RELEASED NO_KEY
          

       
const int keys[16] = {'1','2','3','A',
                      '4','5','6','B',
                      '7','8','9','C',
                      '*','0','#','D'};
                      
class Keypad{    
    public:
        Keypad(PinName col1, PinName col2, PinName col3, PinName col4, PinName row1,PinName row2, PinName row3, PinName row4);
        int getKeyIndex();
        char getKey();

    protected:
        DigitalIn _col1,_col2,_col3,_col4;    
        BusOut _rows;
};

}
#endif
