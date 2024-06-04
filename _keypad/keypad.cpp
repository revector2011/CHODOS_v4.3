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

/*Simple example

#include "mbed.h"
#include "keypad.h"
	  
Serial pc(USBTX, USBRX); 
Keypad telepad(p13,p14,p15,p16,p17,p18,p19,p20);
					
int main(void){
  char key;
  pc.printf("Enter codes\n\r");
  while(1){ 
	key = telepad.getKey();	
	if(key != KEY_RELEASED){
	pc.printf("%c\n",key);
	wait(0.6);
	 }
	}
}		*/

#include "mbed.h"
#include "keypad.h"

using namespace mbed;

Keypad::Keypad(PinName col1, PinName col2, PinName col3, PinName col4, PinName row1,PinName row2, PinName row3, PinName row4): _col1(col1),
    _col2(col2),_col3(col3),_col4(col4),_rows(row1,row2,row3,row4)    {    
        
}     

int Keypad::getKeyIndex(){
    for(int k=0; k<4; k++) {
            _rows = 0;
    }    
    for(int i=0; i<4; i++) {
            _rows = 1 << i;     
        for(int j=0; j<4;j++){         
            if (_col1)
                return 1+(i*4);
            else if (_col2)
                return 2+(i*4);
               else if (_col3)
                   return 3+(i*4); 
            else if (_col4)
                return 4+(i*4);            
        }
    }    
    return -1;
}          

char Keypad::getKey(){
int k = getKeyIndex();    
if(k != -1)
    return  keys[k-1];
 else 
 return 0;
}
