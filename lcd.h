#ifndef __LCD_H__
#define __LCD_H__

class LCD
    : public Serial // for printf
{
public:
                        LCD(I2C & in_cI2C);
    virtual int         _putc(int c) = 0;
            
    virtual void        clear() = 0;
    virtual uint8_t     columns() = 0;
    virtual void        createChar(uint8_t location, uint8_t charmap[]) = 0;
    virtual void        home() = 0;
    virtual uint8_t     rows() = 0;    
    virtual void        setCursor(uint8_t in_nX, uint8_t in_nY) = 0;
    virtual void        showBlink(bool in_bShow) = 0;
    virtual void        showCursor(bool in_bShow) = 0;
    virtual void        showDisplay(bool in_bShow) = 0;
    
protected:
            I2C &       m_cI2C;
};

#endif // __LCD_H__
