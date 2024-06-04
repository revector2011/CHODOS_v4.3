#include "mbed.h"
#include "Rtc_Ds1307.h"
#include "NewTextLCD.h"
#include "keypad.h"
#include "_24LCXXX.h"
//#include "ds3231.h"
#define ESC 0x1B

void get_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member);
void get_user_input(char* message, uint8_t min, uint8_t max, bool* member);

// ВНИМАНИЕ!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//SB48, SB49    ON!!! на плате поставить перемычку!
//SB54, SB55    ON!!! на плате поставить перемычку!
//SB50          OFF!!!!!!!!!!!
//R34, R36      REMOVE!!!

/*
74C922      13.12.2021 revector
В таблице данных C (KBM) = 10 C (OSC).
Итак, я обычно использую C (OSC   5) = 100 нФ и C (KBM  6) = 1 мкФ. Это работает очень хорошо
*/


//!!!  БЕЗ КОНЦЕВИКА НИЖНИХ ДОРНОВ !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


int EnYear   = 2027;
int EnMonth  = 7;
int EnDate   = 22;
int EnHours  = 18;
int NewValve = 0; //1 - новые клапаны открыывания форм, 0 - старые клапаны
int stop_timer=0;
int Pos      = 0; //Позиция курсора при настройке Даты и Времени
//DigitalOut ResetMCP23017(PA_14);

//              MOSI, MISO, SCLK, CS, name
//SDFileSystem sd(D11, D12, D13, D10, "sd");
//FILE *fp;

//Подключение концевых переключателей к выводам контроллера       409- было 48V - стало 5V  замыкаем на +5V все концевики при срабатывании!!!!!

DigitalIn S41(PC_8); //#define S41_pin  0//7  // 1 - Верхние ножницы внизу                  409-407
DigitalIn S42(PC_6); //#define S42_pin  1//6  // 2 - Формы Закрыты                          409-431
DigitalIn S43(PC_5); //#define S43_pin  2//5  // 3 - Формы Открыты                          409-440
DigitalIn S44(PA_12);//#define S44_pin  3//4  // 4 - Выносные ножницы над формой            (461-407)-444   Было +24V - теперь GND!!!
DigitalIn S45(PA_11);//#define S45_pin  4//3  // 5 - Выносные ножницы в исходном состоянии  (461-407)-433
//**************************************************************
// DigitalIn S48(PB_12);//#define S48_pin  5//2  // 6 - Дорны внизу                            409-438
// DigitalIn S66(PB_14);//#define S66_pin  2//5  //11 - Ножницы к формам                       409-478
// Было - стало
DigitalIn S66(PB_12);  //#define S48_pin  5//2  // 6 - Дорны внизу                            409-438
// DigitalIn S48(PB_14);//#define S66_pin  2//5  //11 - Ножницы к формам                       409-478
int S48 = 0;
//**************************************************************
//Кнопки и переключатели ручного упарвления
DigitalIn S61a(PB_2);//#define S61a_pin 6//1  // 7 - Переключатель режима работы            409-428
//#define S61r_pin 7//0  // 8 - Переключатель режима работы            409-429

DigitalIn S63(PB_1); //#define S63_pin  0//7  // 9 - Запирание форм                         409-417
DigitalIn S64(PB_15);//#define S64_pin  1//6  //10 - Открывание форм                        409-418

DigitalIn S67(PB_13);//#define S67_pin  3//4  //12 - Ножницы от форм                        409-479

DigitalOut K45 (PC_10);
DigitalOut K44 (PC_12);//(PC_12);
DigitalOut Y43 (PA_13);
DigitalOut Y46 (PA_14);
DigitalOut Y44 (PA_15);
DigitalOut Y51 (PB_7 );

DigitalOut Y52L(PC_14);//PH_1);//SB48,49 ON!!! на плате поставить перемычку!
DigitalOut Y52P(PC_15);
DigitalOut Y42L(PH_0 );//SB54, SB55 ON!!! на плате поставить перемычку!
// PH_1
// PC_11
// PD_2
DigitalOut Y42P(PC_2 );
DigitalOut Y47 (PC_3 );

DigitalIn KeyPress(PC_13); //PC_4); // PA_0);// 13.12.2021 revector
DigitalIn Key1(PA_1);//#define S41_pin  0//7  // 1 - Верхние ножницы внизу                  409-407
DigitalIn Key2(PA_4);//#define S42_pin  1//6  // 2 - Формы Закрыты                          409-431
DigitalIn Key4(PB_0);//#define S43_pin  2//5  // 3 - Формы Открыты                          409-440
DigitalIn Key8(PC_1);//#define S44_pin  3//4  // 4 - Выносные ножницы над формой            (461-407)-444   Было +24V - теперь GND!!!


/*
DigitalIn S41(PC_10);//#define S41_pin  0//7  // 1 - Верхние ножницы внизу                  409-407
DigitalIn S42(PC_12);//#define S42_pin  1//6  // 2 - Формы Закрыты                          409-431
DigitalIn S43(PA_13);//#define S43_pin  2//5  // 3 - Формы Открыты                          409-440
DigitalIn S44(PA_14);//#define S44_pin  3//4  // 4 - Выносные ножницы над формой            (461-407)-444   Было +24V - теперь GND!!!
DigitalIn S45(PA_15);//#define S45_pin  4//3  // 5 - Выносные ножницы в исходном состоянии  (461-407)-433
DigitalIn S48(PB_7);//#define S48_pin  5//2  // 6 - Дорны внизу                            409-438
//Кнопки и переключатели ручного упарвления
DigitalIn S61a(PC_14);//#define S61a_pin 6//1  // 7 - Переключатель режима работы            409-428
//#define S61r_pin 7//0  // 8 - Переключатель режима работы            409-429

DigitalIn S63(PC_15);//#define S63_pin  0//7  // 9 - Запирание форм                         409-417
DigitalIn S64(PH_0);//#define S64_pin  1//6  //10 - Открывание форм                        409-418
DigitalIn S66(PC_2);//#define S66_pin  2//5  //11 - Ножницы к формам                       409-478
DigitalIn S67(PC_3);//#define S67_pin  3//4  //12 - Ножницы от форм                        409-479
*/

//free 5 6 7

//------------------------------------
// Hyperterminal configuration
// 115200 bauds, 8-bit data, no parity
//------------------------------------

Serial pc(USBTX, USBRX);

I2C i2c(I2C_SDA, I2C_SCL);


DigitalOut myled1(LED1);



//DS1307 rtc(I2C_SDA, I2C_SCL); //0xd0
Rtc_Ds1307 rtc(I2C_SDA, I2C_SCL); //0xd0

//LCD2004 lcd(i2c);
//          rs,   e,  d0,  d1,   d2,  d3
TextLCD lcd(D8,  D9,  D4,  D5,   D6,  D7, TextLCD::LCD20x4);
//         PA9, PC7, PB5, PB4, PB10, PA8
//          21,  19,  29,  27,   25,  23
//ST7920   (_RS, _RW, _E, DB0, DB1, DB2, DB3, DB4, DB5, DB6, DB7);
//ST7920 glcd( D11,  D12, D2,  D3,  D4,  D5,  D6,  D7,  D8,  D9, D10);

_24LCXXX eeprom(&i2c, 0x50);//0x57

//eeprom.nbyte_read( COUNTERADDRESS, &store_address, sizeof(int) );
//eeprom.nbyte_write( store_address, &b, 6);

int sec = 0;
int min = 0;
int hours = 0;
int day = 0;
int date = 0;
int month = 0;
int year = 0;



int ROM_sec = 0;
int ROM_min = 0;
int ROM_hours = 0;
int ROM_date = 0;
int ROM_month = 0;
int ROM_year = 0;

//double ROMLongTime=0;
//double CTCLongTime=0;

unsigned char display_switch = 0;
unsigned char o,m;

unsigned int store_address;

int i=0,j=0,c=0;
//char portState[4]= {0xEF,0xDF,0xBF,0x7F};
//char inputState[4]= {0x01,0x02,0x04,0x08};
//char key[4][4]= {{0x11,0x12,0x13,0x14},
//    {0x21,0x22,0x23,0x24},
//   {0x41,0x42,0x43,0x44},
//   {0x81,0x82,0x83,0x84}
//};



long count_cycle = 0;

//const char udc_Bat_Hi[] = {0x0E, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x1F, 0x00};  // Battery Full
//uint8_t BOX_FULL[8] = {0xff,
int BOX_FULL[8] = {0xff,
                   0xff,
                   0xff,
                   0xff,
                   0xff,
                   0xff,
                   0xff,
                   0xff,
                  };

int  BOX_NULL[8] = {0xff,
                    0x11,
                    0x11,
                    0x11,
                    0x11,
                    0x11,
                    0x11,
                    0xff,
                   };

char buffer[128];
int readptr = 0;
int led;
//***********************************************************************************************************************************************************
// TIMERS

Timer SetUpCalendarTime; //Только в РУЧНОМ РЕЖИМЕ держим кнопку D заданное время и переходим в установку Даты и Времени. Повторное нажатие кнопки D выход из режима.
float SETUPCalendarTime = 10.0; // 10 сек
volatile uint8_t Calendar_key = 0;
unsigned char SetTimeDateFlag = 0;


Timer ALARM; //Таймер для проверки зависания переверии если кончился то Reset_mcp23017
Timer SETUP;// Таймер времени ожидания возврата системы в исходное состояние
Timer TempTimer;
char SetSETUP= 3.0; //3 сек
float CurrentSETUP = 0;
unsigned  char ActiveSETUP=0;
Timer T48;  //   0. T48 - Время охлаждения                 0-100 сек   // Время выдувания форм: старт после T51
float SetT48 = 15.0;
#define T48_eeprom_addr = 0x00;//eeprom.nbyte_write( T48_eeprom_addr, &SetT48, 4);
float CurrentT48 = 0;
unsigned  char ActiveT48=0;
Timer T49;  //   1. T49 - Время отдувания                  0- 10 сек   // Время сбрасывания давления форм в 0. Запускается после T48. /////////// Задается константой!!!
float SetT49 = 7.5; //7 сек - как на первом ходосе
float CurrentT49 = 0;
unsigned  char ActiveT49=0;
Timer T50;  //   2. T50 - Выдержка на верхние ножницы      0- 10 сек   // Время после команды на смыкание верхних ножниц до начала опускания верхних ножниц.
float SetT50 = 4.5;
#define T50_eeprom_addr = 0x05;//eeprom.nbyte_write( T50_eeprom_addr, &SetT50, 4);
float CurrentT50 = 0;
unsigned  char ActiveT50=0;
Timer T51;  //   3. T51 - Начало выдувания                 0- 10 сек   // Время с момента начала закрывания форм до начала выдувания в форму.
float SetT51 = 3.0;
#define T51_eeprom_addr = 0x0A;//eeprom.nbyte_write( T51_eeprom_addr, &SetT51, 4);
float CurrentT51 = 0;
unsigned  char ActiveT51=0;
Timer T52;  //   4. T52 - Верхние дорны вниз               0- 50 сек   // Старт отсчета с начала опускания верхних ножниц до начала опускания верхних дорнов.
float SetT52 = 3.0;
#define T52_eeprom_addr = 0x0F;//eeprom.nbyte_write( T52_eeprom_addr, &SetT52, 4);
float CurrentT52 = 0;
unsigned  char ActiveT52=0;

Timer OneCycleTime;  //   Индикатор длительности цикла
float CurrentOneCycleTime = 0;

Timer DownDornsTime;  //   Индикатор длительности цикла
float CurrentDownDornsTime = 0.5;


Timer DownClips; //Таймер для
Timer UpClips; //Таймер для


//Timer T53;  //   5. T53 - Верхние дорны вверх              0-100 сек // Упразднено: Поднимаем дорны одновременно с командой открывание форм!!!
//Timer T54;  //   6. T54 - Длина шланга                     0- 50 сек // ЧЕХИ - ДЕБИЛЫ!!! так назвать таймер На самом деле время ДО Начала_Опускания_Верхних_Дорнов!!!
//Timer T55;  //   7. T55 - Задержка опускания нижних дорнов 0- 30 сек // Упразднено: Опускаются при отжатии кончевика Фрмы Закрыты!!!
//***********************************************************************************************************************************************************

//volatile int SetT48;  //   0. T48 - Потенциометр Время охлаждения                 0-100 сек
//volatile int SetT49;  //   1. T49 - Потенциометр Время отдувания                  0- 10 сек
//volatile int SetT50;  //   2. T50 - Потенциометр Выдержка на верхние ножницы      0- 10 сек
//volatile int SetT51;  //   3. T51 - Потенциометр Начало выдувания                 0- 10 сек
//volatile int SetT52;  //   4. T52 - Потенциометр Верхние дорны вниз               0- 50 сек //Старт отсчета в начале опускания верхних ножниц!!!
//volatile int SetT53;  //   5. T53 - Потенциометр Верхние дорны вверх              0-100 сек //Упразднено: Поднимаем дорны при открытии форм!!!
//volatile int SetT54;  //   6. T54 - Потенциометр Длина шланга                     0- 50 сек //ЧЕХИ - ДЕБИЛЫ!!! так назвать таймер
//volatile int SetT55;  //   7. T55 - Потенциометр Задержка опускания нижних дорнов 0- 30 сек //Упразднено: Опускаются при отжатии кончевика Фрмы Закрыты!!!
//volatile int SetSETUP = 1000;//   Время ожидания возврата системы в исходное состояние


DigitalOut myled(LED1);
const unsigned char LOW = 0;
const unsigned char HIGH = 1;
const unsigned char OFF = 0;
const unsigned char ON = 1;

unsigned  char FlagChangeT48; // Флаг на событие изменения SetT48. После срабатывания флага в конце цикла происходит запись в ЕЕПРОМ значения SetT48
unsigned  char FlagChangeT50;
unsigned  char FlagChangeT51;
unsigned  char FlagChangeT52;

volatile unsigned char StepFlag = 0;
volatile unsigned char FormsOK = 0;
volatile unsigned char DownDornsOK = 0;
volatile unsigned char DownClipsOK = 0;
volatile unsigned char UpClipsOK = 0;
volatile unsigned char InjectorOK = 0;
volatile unsigned char Down_Clips_go_Forms = 0;
volatile unsigned char Down_Clips_back_Forms = 0;
volatile unsigned char mm = 0;
volatile unsigned char cc = 0;
volatile unsigned char FlagInjector = 0;
volatile unsigned char StartInjector = 0;
volatile unsigned char AutomaticMode = 0;
volatile unsigned char EndInjector = 0;
volatile unsigned char StartUpDorns = 0;
volatile unsigned char FlagUpDorns = 0;
unsigned char flagTime=0;
unsigned char flag0 = 0;
unsigned char flag1 = 0;
unsigned char flag2 = 0;
unsigned char flag3 = 0;
unsigned char flag4 = 0;
unsigned char flag5 = 0;
unsigned char SetTime = 0;


unsigned char SSA_old = 0;
unsigned char SSB_old = 0;

//mcp23017_POWER - Port_A(0-7) -> OUT Семисторные ключи (0,1,2,3,4,5,6,7)
//mcp23017_POWER - Port_B(0-7) -> OUT Семисторные ключи (0,1,2,3,4,5,6,7)
//MCP23017 mcp23017_POWER    = MCP23017(i2c, 0x20<<1); // Адреc I2C 0x20

//mcp23017_SELECTOR - Port_A(0-7) <- IN Оптопары. Вход концевиков и кнопок (0,1,2,3,4,5,6,7)
//mcp23017_SELECTOR - Port_B(0-7) <- IN Оптопары. Вход концевиков и кнопок (0,1,2,3,4,5,6,7)
//MCP23017 mcp23017_SELECTOR = MCP23017(i2c, 0x21<<1); // Адреc I2C 0x24

//mcp23017_KEYBOARD - Port_A(0-7) <- IN  Матрица клавиш вход (0,1,2,3,4,5,6,7)
//mcp23017_KEYBOARD - Port_B(0-7) -> OUT Матрица клавиш выход(0,1,2,3,4,5,6,7)
//MCP23017 mcp23017_KEYBOARD = MCP23017(i2c, 0x22<<1); // Адреc I2C 0x22

/*           1 0
             2 1
             4 2
             8 3
            16 4
            32 5
            64 6
           128 7
*///S41 = LOW,       // 1 - Верхние ножницы внизу                  HIGH = Активно
/*
volatile unsigned char
                      // S42 = LOW,       // 2 - Формы Закрыты                          HIGH = Активно
                       //S43 = LOW,       // 3 - Формы Открыты                          HIGH = Активно
                       //S44 = LOW,       // 4 - Выносные ножницы над формой            HIGH = Активно
                       S45 = LOW,       // 5 - Выносные ножницы в исходном состоянии  HIGH = Активно
                       S48 = LOW,       // 6 - Дорны внизу                            HIGH = Активно
                       S61a= LOW,       // 7 - Переключатель режима работы Авто       HIGH = Активно
                       S61r= LOW,       // 8 - Переключатель режима работы Ручн       HIGH = Активно
                       S63 = LOW,       // 9 - Запирание форм                         HIGH = Активно
                       S64 = LOW,       //10 - Открывание форм                        HIGH = Активно
                       S66 = LOW,       //11 - Ножницы к формам                       HIGH = Активно
                       S67 = LOW,       //12 - Ножницы от форм                        HIGH = Активно
                       Y42L= LOW,       // 1 - Закрывание форм                        Электроклапан
                       Y42P= LOW,       // 2 - Открывание форм                        Электроклапан
                       Y43 = LOW,       // 3 - Сжатие верхних ножниц                  Электроклапан
                       Y46 = LOW,       // 4 - Сжатие нижних ножниц                   Электроклапан
                       Y47 = LOW,       // 5 - Опускание нижних дорнов                Электроклапан
                       Y51 = LOW,       // 6 - Опускание верхних ножниц               Электроклапан
                       Y52L= LOW,       // 7 - Верхние дорны вверх                    Электроклапан
                       Y52P= LOW,       // 8 - Верхние дорны вниз                     Электроклапан
                       K44 = LOW,       // 9 - Выносные ножницы - движение К  форме   Пускатель!!!
                       K45 = LOW,       //10 - Выносные ножницы - движение ОТ формы   Пускатель!!!
                       Y44 = LOW;       //11 - Вы дувание                              Электроклапан
*/


volatile unsigned char heater21 = LOW,  //Нагреватель 21 зона
                       heater22 = LOW,  //Нагреватель 22 зона
                       heater23 = LOW,  //Нагреватель 23 зона
                       heater24 = LOW,  //Нагреватель 24 зона
                       heater31 = LOW,  //Нагреватель 25 зона
                       heater32 = LOW;  //Нагреватель 26 зона
char customKey;
char customKey_old;


void OUT_PORTS()    // Формирование восьмибитного байта ByteOUT для включения семисторных ключей
{
    /*
       volatile unsigned char ByteOUT=0;
       ByteOUT= Y42L*128+     // 1 - Закрывание форм                        Электроклапан
                Y42P *64+     // 2 - Открывание форм                        Электроклапан
                Y43  *32+     // 3 - Сжатие верхних ножниц                  Электроклапан
                Y46  *16+     // 4 - Сжатие нижних ножниц                   Электроклапан
                Y47   *8+     // 5 - Опускание нижних дорнов                Электроклапан
                Y51   *4+     // 6 - Опускание верхних ножниц               Электроклапан
                Y52L  *2+     // 7 - Верхние дорны вверх                    Электроклапан
                Y52P;         // 8 - Верхние дорны вниз                     Электроклапан
       //Запись в порт вывода
       //mcp23017_POWER.direction(   PORT_B, PORT_DIR_OUT);//mcp23017_POWER    - Port_B(0-7) -> OUT Семисторные ключи (0,1,2,3,4,5,6,7)
       mcp23017_POWER.write(PORT_B, ~ByteOUT);
       ByteOUT= Y44    *128+ // 9 - Выносные ножницы - движение К  форме   Пускатель!!!
                K44     *64+ //10 - Выносные ножницы - движение ОТ формы   Пускатель!!!
                K45     *32+ //11 - Выдувание                              Электроклапан
                heater22*16+ //12 - Нагреватель 22 зона
                heater23 *8+ //13 - Нагреватель 23 зона
                heater24 *4+ //14 - Нагреватель 24 зона
                heater31 *2+ //15 - Нагреватель 31 зона
                heater32;    //16 - Нагреватель 32 зона
       //Запись в порт вывода
       //mcp23017_POWER.direction(   PORT_A, PORT_DIR_OUT);//mcp23017_POWER    - Port_A(0-7) -> OUT Семисторные ключи (0,1,2,3,4,5,6,7)
       mcp23017_POWER.write(PORT_A, ~ByteOUT);*/
}// END Формирование восьмибитного байта ByteOUT для включения семисторных ключей



void setup()
{
//    i2c.frequency(100000);

//glcd.InitDisplay();

    S41.mode(PullDown);
    S42.mode(PullDown);
    S43.mode(PullDown);
    S44.mode(PullDown);
    S45.mode(PullDown);
//    S48.mode(PullDown);
    S61a.mode(PullDown);
    S63.mode(PullDown);
    S64.mode(PullDown);
    S66.mode(PullDown);
    S67.mode(PullDown);

    //Y44.mode(PullDown);

    KeyPress.mode(PullUp);//
    Key1.mode(PullUp);
    Key2.mode(PullUp);
    Key4.mode(PullUp);
    Key8.mode(PullUp);

    Y42L=OFF;
    Y42P=OFF;
    Y43 =OFF;
    Y46 =OFF;
    Y47 =OFF;
    Y51 =OFF;
    Y52L=OFF;
    Y52P=OFF;
    K44 =OFF;
    K45 =OFF;
    Y44 =OFF;
    lcd.writeCGRAM(2, BOX_FULL);
    lcd.writeCGRAM(1, BOX_NULL);
    //rtc.read( 0x20, junk)  //RAM в часах на батарейке 0x08-0x3F
    //for (i=0;i<128;i++) eeprom.nbyte_write(i, 0x00, 1);



}


void refresh_lcd()
{
    if (display_switch == 1) {
        //lcd.locatelcd.locate( 0, 0);
        lcd.locate( 0, 0);
        lcd.printf("S41");
        lcd.putc(S41==1 ? '\x02':'\x01');//==1 ? '1':'0');//lcd.printf("%s", (S41==1 ? "\x8d" : " "));//(col < 8 ? 0x80 : 0xc0) //lcd.printf("%0*d ",1,S41);//
        lcd.locate( 5, 0);
        lcd.printf("S45");
        lcd.putc(S45==1 ? '\x02':'\x01');
        lcd.locate(10, 0);
        lcd.printf("S66");
        lcd.putc(S66==1 ? '\x02':'\x01');
        lcd.locate( 0, 1);
        lcd.printf("S42");
        lcd.putc(S42==1 ? '\x02':'\x01');
        lcd.locate( 0, 2);
        lcd.printf("S43");
        lcd.putc(S43==1 ? '\x02':'\x01');
        lcd.locate( 0, 3);
        lcd.printf("S44");
        lcd.putc(S44==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.locate( 5, 1);
        lcd.printf("S48");
//        lcd.putc(S48==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.putc(S48==1 ? '*':'*');//==1 ? '1':'0');
        lcd.locate( 5, 2);
        lcd.printf("S63");
        lcd.putc(S63==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.locate( 5, 3);
        lcd.printf("S64");
        lcd.putc(S64==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.locate(10, 1);
        lcd.printf("S67");
        lcd.putc(S67==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.locate(10, 2);
        lcd.printf("61a");
        lcd.putc(S61a==1 ? '\x02':'\x01');//==1 ? '1':'0');
        lcd.locate(10, 3);
        lcd.printf("61r ");
        //lcd.putc(S61r==1 ? '\x02':'\x01');
        lcd.locate(19, 1);
        lcd.printf(AutomaticMode == 1 ? "A":"M");
        lcd.locate(18, 0);
        lcd.printf("%0*d",2, StepFlag);
        lcd.locate(15, 2);
        lcd.printf("Cycle");
        lcd.locate(15, 3);
        lcd.printf("%4.1fs", CurrentOneCycleTime);
    } else if (display_switch == 2) {
        lcd.locate(0, 0);
        lcd.printf("T48=%4.1f/", SetT48);
        lcd.printf("%4.1fs",CurrentT48);
        lcd.locate(0, 1);
        lcd.printf("T50=%4.1f/",SetT50);
        lcd.printf("%4.1fs",CurrentT50);
        lcd.locate(0, 2);
        lcd.printf("T51=%4.1f/",SetT51);
        lcd.printf("%4.1fs",CurrentT51);
        lcd.locate(0, 3);
        lcd.printf("T52=%4.1f/",SetT52);
        lcd.printf("%4.1fs",CurrentT52);
        lcd.locate(19, 1);
        lcd.printf(AutomaticMode == 1 ? "A":"M");
        lcd.locate(18, 0);
        lcd.printf("%0*d",2, StepFlag);
        lcd.locate(15, 2);
        lcd.printf("Cycle");
        lcd.locate(15, 3);
        lcd.printf("%4.1fs", CurrentOneCycleTime);
    }  else if (display_switch == 0) {
        lcd.locate(0, 0);
        lcd.printf("Date:%0*d",4,year); //+2000
        lcd.printf("/%0*d",2,month);
        lcd.printf("/%0*d",2,date);
        lcd.locate(0, 1);
        lcd.printf("Time:%0*d",2,hours);
        lcd.printf(":%0*d",2,min);
        lcd.printf(":%0*d",2,sec);
        lcd.printf(" V4.3");   //VVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVVV
        lcd.printf(" ");
        lcd.locate(0, 3);
        lcd.printf("Counter:");
        lcd.printf("%06d",count_cycle);
        lcd.printf(" ");
        lcd.locate(0, 2);
        lcd.printf("Mode:");
        if (AutomaticMode == 1) {
            lcd.printf("Automatic");
        } else {
            lcd.printf("Manual   ");
        }
        lcd.locate(19, 1);
        lcd.printf(AutomaticMode == 1 ? "A":"M");
        lcd.locate(18, 0);
        lcd.printf("%0*d",2, StepFlag);
        lcd.locate(15, 2);
        lcd.printf("Cycle");
        lcd.locate(15, 3);
        lcd.printf("%4.1fs", CurrentOneCycleTime);
    } else if (display_switch == 3) {
        lcd.locate(0, 0);
        lcd.printf("42L");
        lcd.putc(Y42L==1 ? '\x02':'\x01');
        lcd.printf(" 42P");
        lcd.putc(Y42P==1 ? '\x02':'\x01');
        lcd.locate(0, 1);
        lcd.printf("52L");
        lcd.putc(Y52L==1 ? '\x02':'\x01');
        lcd.printf(" 52P");
        lcd.putc(Y52P==1 ? '\x02':'\x01');
        lcd.printf(" Y43");
        lcd.putc(Y43==1 ? '\x02':'\x01');
        lcd.locate(0, 2);
        lcd.printf("K44");
        lcd.putc(K44==1 ? '\x02':'\x01');
        lcd.printf(" K45");
        lcd.putc(K45==1 ? '\x02':'\x01');
        lcd.printf(" Y44");
        lcd.putc(Y44==1 ? '\x02':'\x01');
        lcd.locate(0, 3);
        lcd.printf("Y46");
        lcd.putc(Y46==1 ? '\x02':'\x01');
        lcd.printf(" Y47");
        lcd.putc(Y47==1 ? '\x02':'\x01');
        lcd.printf(" Y51");
        lcd.putc(Y51==1 ? '\x02':'\x01');
        lcd.locate(19, 1);
        lcd.printf(AutomaticMode == 1 ? "A":"M");
        lcd.locate(18, 0);
        lcd.printf("%0*d",2, StepFlag);
        lcd.locate(15, 2);
        lcd.printf("Cycle");
        lcd.locate(15, 3);
        lcd.printf("%4.1fs", CurrentOneCycleTime);
    } else if (display_switch == 4) {

    }
}
//*************************************************************************************************************************************************************************************

void pt()
{
    /*
    Rtc_Ds1307::Time_rtc tm = {};
    if (rtc.getTime(tm) ) {
        pc.printf("%02d:%02d:%02d>", tm.hour, tm.min, tm.sec);
        //pc.printf("Enter command =>");
    } else {
        pc.printf("no_time_chip>");
    }
    */
}

//*************************************************************************************************************************************************************************************

//*************************************************************************************************************************************************************************************
char box(char ii)
{
    char l;
    if (ii>0) {
        l=0x94;
    } else {
        l=0x95;
    }
    return l;
}
//*************************************************************************************************************************************************************************************
void refresh_terminal()
{
    /*
            //pc.printf("\033[2J"); // cls the entire screen.
            pc.printf("\033[0;0f"); // Move cursor to the top left hand corner
            //pc.printf("\033[?25l"); // cursor OFF
            //pc.printf("\033[31");

            //рисуем рамку и строчки
            //pc.printf("\x82\x80\x83\n\r\x84\x80\x85\n\r");
            pc.printf("\x82\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x88\x80\x88\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x88\x80\x88\x80\x80\x88\x80\x83\n\r");
            pc.printf("\x81S41 (UpClips DOWN        )\x81");
            pc.printf("%c",box(S41));
            pc.printf("\x81Y42L(CLOSE Forms    >[]< )\x81");
            pc.printf("%c",box(Y42L));
            pc.printf("\x81");
            pc.printf("21");
            pc.printf("\x81");
            pc.printf("%c",box(heater21));
            pc.printf("\x81\n\r");
            pc.printf("\x81S42 (Forms CLOSE         )\x81");
            pc.printf("%c",box(S42));
            pc.printf("\x81Y42P(OPEN Forms    <[  ]>)\x81");
            pc.printf("%c",box(Y42P));
            pc.printf("\x81");
            pc.printf("22");
            pc.printf("\x81");
            pc.printf("%c",box(heater22));
            pc.printf("\x81\n\r");
            pc.printf("\x81S43 (Forms OPEN          )\x81");
            pc.printf("%c",box(S43));
            pc.printf("\x81Y43 (CLOSE UpClips  -><- )\x81");
            pc.printf("%c",box(Y43 ));
            pc.printf("\x81");
            pc.printf("23");
            pc.printf("\x81");
            pc.printf("%c",box(heater23));
            pc.printf("\x81\n\r");
            pc.printf("\x81S44 (DownClips ON Forms  )\x81");
            pc.printf("%c",box(S44));
            pc.printf("\x81Y46 (CLOSE DownClips-><- )\x81");
            pc.printf("%c",box(Y46 ));
            pc.printf("\x81");
            pc.printf("24");
            pc.printf("\x81");
            pc.printf("%c",box(heater24));
            pc.printf("\x81\n\r");
            pc.printf("\x81S45 (DownClops BACK Forms)\x81");
            pc.printf("%c",box(S45));
            pc.printf("\x81Y47 (DOWN DownDorns      )\x81");
            pc.printf("%c",box(Y47 ));
            pc.printf("\x81");
            pc.printf("31");
            pc.printf("\x81");
            pc.printf("%c",box(heater31));
            pc.printf("\x81\n\r");
            pc.printf("\x81S48 (DownDorns DOWN      )\x81");
            pc.printf("%c",box(S48));
            pc.printf("\x81Y51 (DOWN UpClips        )\x81");
            pc.printf("%c",box(Y51 ));
            pc.printf("\x81");
            pc.printf("32");
            pc.printf("\x81");
            pc.printf("%c",box(heater32));
            pc.printf("\x81\n\r");
            pc.printf("\x81S61a(MODE Auto           )\x81");
            pc.printf("%c",box(S61a));
            pc.printf("\x81Y52L(UP UpDorns          )\x81");
            pc.printf("%c",box(Y52L));
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x81S61r(MODE Manual         )\x81 ");
            pc.printf("\x81Y52P(DOWN UpDorns        )\x81");
            pc.printf("%c",box(Y52P));
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x81S63 (FORMS -><-          )\x81");
            pc.printf("%c",box(S63));
            pc.printf("\x81K44 (DownClips -> FORMS  )\x81");
            pc.printf("%c",box(K44 ));
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x81S64 (FORMS <-->          )\x81");
            pc.printf("%c",box(S64));
            pc.printf("\x81K45 (DownClips <- FORMS  )\x81");
            pc.printf("%c",box(K45 ));
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x81S66 (DownClips -> FORMS  )\x81");
            pc.printf("%c",box(S66));
            pc.printf("\x81Y44 (START HIGH PRESSURE )\x81");
            pc.printf("%c",box(Y44 ));
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x81S67 (DownClips <- FORMS  )\x81");
            pc.printf("%c",box(S67));
            pc.printf("\x81                          \x81");
            pc.printf(" "           );
            pc.printf("\x81");
            pc.printf("  ");
            pc.printf("\x81");
            pc.printf(" "              );
            pc.printf("\x81\n\r");
            pc.printf("\x86\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x88\x80\x80\x80\x80\x80\x88\x80\x80\x80\x80\x80\x88\x80\x80\x80\x89\x80\x89\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x89\x80\x89\x80\x80\x89\x80\x87\n\r");
            pc.printf("\x81Timer T48 \x81");
            pc.printf("%5.1f", SetT48);
            pc.printf("\x81");
            pc.printf("%5.1f", CurrentT48);
            pc.printf("\x81                                       \x81\n\r");
            pc.printf("\x81Timer T49 \x81");
            pc.printf("%5.1f", SetT49);
            pc.printf("\x81");
            pc.printf("%5.1f", CurrentT49);
            pc.printf("\x81                                       \x81\n\r");
            pc.printf("\x81Timer T50 \x81");
            pc.printf("%5.1f", SetT50);
            pc.printf("\x81");
            pc.printf("%5.1f", CurrentT50);
            pc.printf("\x81                                       \x81\n\r");
            pc.printf("\x81Timer T51 \x81");
            pc.printf("%5.1f", SetT51);
            pc.printf("\x81");
            pc.printf("%5.1f", CurrentT51);
            pc.printf("\x81                                       \x81\n\r");
            pc.printf("\x81Timer T52 \x81");
            pc.printf("%5.1f", SetT52);
            pc.printf("\x81");
            pc.printf("%5.1f", CurrentT52);
            pc.printf("\x81                                       \x81\n\r");
            pc.printf("\x86\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x89\x80\x80\x80\x80\x80\x89\x80\x80\x80\x80\x80\x87");
            pc.printf("                                       ");
            pc.printf("\x81\n\r");
            pc.printf("\x81");
            pc.printf(" %0*d/",2,date);
            pc.printf("%0*d/",2,month);
            pc.printf("%0*d",4,year);
            pc.printf("  %0*d:",2,hours);
            pc.printf("%0*d:",2,min);
            pc.printf("%0*d",2,sec);
            pc.printf(" \x81");
            pc.printf("                                       \x81\n\r");
            pc.printf("\x84\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x89\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x80\x85\n\r");
            pc.printf("\n\r");

        pc.printf("0x%x",customKey);
        pc.printf("   ");
        pc.printf("0x%x",KeyPress);
        pc.printf("   \n\r");
    */
}
volatile uint8_t press_key = 0;

//******* KeyPadRead START ******************************************************************************************************************************************************************************
void KeyPadRead()
{
    if (KeyPress == 1 && press_key == 0) {  // Клавиша нажата
        customKey=Key1+Key2*2+Key4*4+Key8*8;
        pc.printf("%0*d",4,customKey);
        pc.printf("   ");
        pc.printf("%0*d",4,customKey_old);
        pc.printf("   \n\r");
        customKey_old = customKey;
        press_key=1;
    }

    if (KeyPress == 0 && press_key == 2) {  // Клавиша не нажата
        customKey=0;
        pc.printf("%0*d",4,customKey);
        pc.printf("   ");
        pc.printf("%0*d",4,customKey_old);
        pc.printf("   \n\r");
        customKey_old = customKey;
        press_key=0;
    }

    if (press_key == 1) {
        if (customKey == 0x03) {         // && display_switch != 3) {         //Нажата кнопка A   Display mode 3
            lcd.cls();
            press_key=2;
            display_switch = 3;
        } else if (customKey == 0x07) {         //&& display_switch != 2) {   //Нажата кнопка B   Display mode 2
            lcd.cls();
            press_key=2;
            display_switch = 2;
        } else if (customKey == 0x0b) {         // && display_switch != 1) {  //Нажата кнопка C   Display mode 1
            lcd.cls();
            press_key=2;
            display_switch = 1;

        } else if (customKey == 0x0f) {         // && display_switch != 0) {  //Нажата кнопка D   Display mode 0
            lcd.cls();
            press_key=2;
            display_switch = 0;

        } else if (customKey == 0x00) {                         // Нажата кнопка 1
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y42L == 1) {
                    Y42L = 0;
                    Y42P = 0;
                } else {
                    Y42L = 1;
                    Y42P = 0;
                }
            }
            if (display_switch == 2) {
                if (SetT48>0.1) {
                    SetT48=SetT48-0.1;
                } else {
                    SetT48=0;
                }
                FlagChangeT48=0;/////
                eeprom.nbyte_write(0x00, &SetT48, 4);
            }
        } else if (customKey == 0x01) {                         // Нажата кнопка 2
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y42P == 1) {
                    Y42P = 0;
                    Y42L = 0;
                } else {
                    Y42P = 1;
                    Y42L = 0;
                }
                //Y42P == 1 ? Y42P = 0 : Y42P = 1;
            }
            if (display_switch == 2) {
                if (SetT48<99.8) {
                    SetT48=SetT48+0.1;
                } else {
                    SetT48=99.9;
                }
                FlagChangeT48=0;/////
                eeprom.nbyte_write(0x00, &SetT48, 4);
            }

        } else if (customKey == 0x02) {                         // Нажата кнопка 3
            press_key=2;
        } else if (customKey == 0x04) {                         // Нажата кнопка 4
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y52L == 1) {
                    Y52L = 0;
                    Y52P = 0;
                } else {
                    Y52P = 0;
                    Y52L = 1;
                }
            }
            if (display_switch == 2) {
                if (SetT50>0.1) {
                    SetT50=SetT50-0.1;
                } else {
                    SetT50=0;
                }
                FlagChangeT50=0;/////
                eeprom.nbyte_write(0x05, &SetT50, 4);
            }
        } else if (customKey == 0x05) {                         // Нажата кнопка 5
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y52P == 1) {
                    Y52P = 0;
                    Y52L = 0;
                } else {
                    Y52L = 0;
                    Y52P = 1;
                }
            }
            if (display_switch == 2) {
                if (SetT50<99.8) {
                    SetT50=SetT50+0.1;
                } else {
                    SetT50=99.9;
                }
                FlagChangeT50=0;/////
                eeprom.nbyte_write(0x05, &SetT50, 4);
            }
        } else if (customKey == 0x06) {                         // Нажата кнопка 6
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y43 == 1) {
                    Y43 = 0;
                } else {
                    Y43 = 1;
                }
            }
        } else if (customKey == 0x08) {                         // Нажата кнопка 7
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                //если можно включить мотор ножниц то включаем
                if (K44 != OFF) {
                    K44 = OFF;//Реле пускателя - Выносные ножницы - движение К  форме
                } else {
                    if (S41 == LOW) { //Верхние ножницы не должны быть вверху!!!
                        if (S44 != HIGH) { //Выносные ножницы не над формой
                            if (K45 == OFF) {
                                K44 = ON;//Реле пускателя - Выносные ножницы - движение К  форме
                            } else {
                                K45 = OFF;
                            }
                        }
                    }
                }
            }
            if (display_switch == 2) {
                if (SetT51>0.1) {
                    SetT51=SetT51-0.1;

                } else {
                    SetT51=0;
                }
                FlagChangeT51=0;/////
                eeprom.nbyte_write(0x0a, &SetT51, 4);
            }
        } else if (customKey == 0x09) {
            press_key=2;
            // Do that need for button "8"
            if (AutomaticMode == 0 && display_switch == 3) {
                if (K45 == HIGH) {
                    K45 = OFF;//Реле пускателя - Выносные ножницы - движение К  форме
                } else {
                    //если можно включить мотор ножниц то включаем
                    if (S41 == LOW) { //Верхние ножницы не должны быть вверху!!!
                        if (S45 != HIGH) { //Выносные ножницы не исходном состоянии
                            if (K44 == LOW) {
                                K45 = ON;//Реле пускателя - Выносные ножницы - движение ОТ формы
                            } else {
                                K44 = OFF;
                            }
                        }
                    }
                }
            }
            if (display_switch == 2) {
                if (SetT51<99.8) {
                    SetT51=SetT51+0.1;
                } else {
                    SetT51=99.9;
                }
                FlagChangeT51=0;/////
                eeprom.nbyte_write(0x0a, &SetT51, 4);
            }
        } else if (customKey == 0x0a) {
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y44 == 1) {
                    Y44 = 0;
                } else {
                    Y44 = 1;
                }
            }
        } else if (customKey == 0x0c) {
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y46 == 1) {
                    Y46 = 0; //Inverted!!!!
                } else {
                    Y46 = 1; //Inverted!!!!
                }
            }
            if (display_switch == 2) {
                if (SetT52>0.1) {
                    SetT52=SetT52-0.1;

                } else {
                    SetT52=0;
                }
                FlagChangeT52=0;//////
                eeprom.nbyte_write(0x0f, &SetT52, 4);
            }
        } else if (customKey == 0x0d) {
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y47 == 1) {
                    Y47 = 0;
                } else {
                    Y47 = 1;
                }
            }
            if (display_switch == 2) {
                if (SetT52<99.8) {
                    SetT52=SetT52+0.1;
                } else {
                    SetT52=99.9;
                }
                FlagChangeT52=0;/////
                eeprom.nbyte_write(0x0f, &SetT52, 4);
            }
        } else if (customKey == 0x0e) {
            press_key=2;
            if (AutomaticMode == 0 && display_switch == 3) {
                if (Y51 == 1) {
                    Y51 = 0;
                } else {
                    Y51 = 1;
                }
            }
        }
    }
}
//******* KeyPadRead END ******************************************************************************************************************************************************************************

void ReadTimers()
{
    if(ActiveSETUP) {
        CurrentSETUP=SETUP.read();
    }
    if(ActiveT48) {
        CurrentT48=T48.read();
    }
    if(ActiveT49) {
        CurrentT49=T49.read();
    }
    if(ActiveT50) {
        CurrentT50=T50.read();
    }
    if(ActiveT51) {
        CurrentT51=T51.read();
    }
    if(ActiveT52) {
        CurrentT52=T52.read();
    }
}

void CheckStartEnable()
{
    stop_timer=0;
    /*    if (year != EnYear) {
            stop_timer=1;
            pc.printf("1Bell...                                                                                       \n\r");
        }
        if (month> EnMonth) {
            stop_timer=1;
            pc.printf("2Bell...                                                                                       \n\r");
        } else {
            if (month== EnMonth) {

                if (date >  EnDate) {
                    stop_timer=1;
                    pc.printf("3Bell...                                                                                       \n\r");
                }
                if (date == EnDate) {
                    pc.printf("4Bell...                                                                                       \n\r");
                    if (hours >= EnHours ) {
                        stop_timer=1;
                        pc.printf("5Bell...                                                                                       \n\r");
                    }
                }
            }
        }*/
    if (stop_timer==1) {
        StepFlag = 0;
        flag0=0;
    } else {
        StepFlag = 1;
        flag0=0;
    }
}








int main()   //MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN MAIN
{

    setup();

    lcd.cls();


    pc.baud(115200);        // COM PORT initialize
    pc.printf("\033[2J");   // cls the entire screen.
    pc.printf("\033[0;0f"); // Move cursor to the top left hand corner

    ReadTimers();
    eeprom.nbyte_read(0x00, &SetT48, 4 );
    eeprom.nbyte_read(0x05, &SetT50, 4 );
    eeprom.nbyte_read(0x0a, &SetT51, 4 );
    eeprom.nbyte_read(0x0f, &SetT52, 4 );
    eeprom.nbyte_read(0x15, &count_cycle, 4 );

    //eeprom.nbyte_write(0x15, 0, 4 );  //   Очистка счетчика циклов

    Rtc_Ds1307::Time_rtc tm = {};
    rtc.startClock();
    if (rtc.getTime(tm) ) {
        sec = tm.sec;
        min = tm.min;
        hours = tm.hour;
        date = tm.date;
        month = tm.mon;
        year = tm.year;
    }

    //CTCLongTime= year*100000000 + month*1000000 + day*10000 + hours*100 + min;
    //eeprom.nbyte_write(0x20, &CTCLongTime, 10 );
    pc.printf("CTC_Time = %d/",year);
    pc.printf("%d/",month);
    pc.printf("%d/",date);
    pc.printf(" %d:",hours);
    pc.printf("%d:",min);
    pc.printf("%d\n\r",sec);//pc.printf("ROMLongTime = %d \n\r", ROMLongTime);
    
/// Раскоментировать для установки времени и даты
       tm.year =   2021;
       tm.mon  =   12;
       tm.date =   14;
       tm.hour =   8;
       tm.min  =   25;
       tm.sec  =   00;
     //rtc.setTime(tm, false, false);
/// **********************************************
//    rtc.startClock();

    if (rtc.getTime(tm) ) {
        sec     = tm.sec;
        min     = tm.min;
        hours   = tm.hour;
        date    = tm.date;
        month   = tm.mon;
        year    = tm.year;
    }
    //CTCLongTime= year*100000000 + month*1000000 + day*10000 + hours*100 + min;
    //eeprom.nbyte_write(0x20, &CTCLongTime, 10 );

    pc.printf("CTC_Time = %d/",year);
    pc.printf("%d/",month);
    pc.printf("%d/",date);
    pc.printf(" %d:",hours);
    pc.printf("%d:",min);
    pc.printf("%d\n\r",sec);//pc.printf("ROMLongTime = %d \n\r", ROMLongTime);
    /*
        eeprom.nbyte_write(0x20, &year,  2 );   //Читаем РОМ
        eeprom.nbyte_write(0x22, &month, 1 );   //Читаем РОМ
        eeprom.nbyte_write(0x23, &date,  1 );   //Читаем РОМ
        eeprom.nbyte_write(0x24, &hours, 1 );   //Читаем РОМ
        eeprom.nbyte_write(0x25, &min,   1 );   //Читаем РОМ
        eeprom.nbyte_write(0x26, &sec,   1 );   //Читаем РОМ
    */
    

       eeprom.nbyte_read(0x20, &ROM_year,  2 );   //Читаем РОМ
       eeprom.nbyte_read(0x22, &ROM_month, 1 );   //Читаем РОМ
       eeprom.nbyte_read(0x23, &ROM_date,  1 );   //Читаем РОМ
       eeprom.nbyte_read(0x24, &ROM_hours, 1 );   //Читаем РОМ
       eeprom.nbyte_read(0x25, &ROM_min,   1 );   //Читаем РОМ
       eeprom.nbyte_read(0x26, &ROM_sec,   1 );   //Читаем РОМ

       pc.printf("ROM_Time = %d/",ROM_year);
       pc.printf("%d/",ROM_month);
       pc.printf("%d",ROM_date);
       pc.printf(" %d:",ROM_hours);
       pc.printf("%d:",ROM_min);
       pc.printf("%d\n\r",ROM_sec);// \n\r", CTCLongTime);

    
    /*
        if ((year != ROM_year) || (month != ROM_month)) {
            tm.year=ROM_year;
            tm.mon =ROM_month;
            tm.date=ROM_date;
            tm.hour=ROM_hours;
            tm.min =ROM_min;
            tm.sec =00;
            rtc.setTime(tm, false, false);
        }
    */
//   while (year!=2015) {
//        lcd.locate(0, 0);
    //lcd.printf("Startup ERROR 0x01");
//    }
//    if (date > 5){
//    while (month>10)   {
//        lcd.locate(0, 0);
    //lcd.printf("Startup ERROR 0x02");
//    }
//}
//while (month>11)   {
//        lcd.locate(0, 0);
    //lcd.printf("Startup ERROR 0x02");
//    }


    //rtc.start_clock();
    //rtc.gettime( &sec, &min, &hours, &day, &date, &month, &year);
    KeyPadRead();



    /*
            while(1) {
                if (rtc.getTime(tm) ) {
                    //pc.printf("The current time is : %02d:%02d:%02d\n\r", tm.hour, tm.min, tm.sec);
                    //pc.printf("The current date is : %s, %02d/%02d/%04d\n\r", rtc.weekdayToString(tm.wday), tm.mon, tm.date, tm.year);
                    sec = tm.sec;
                    min = tm.min;
                    hours = tm.hour;
                    date = tm.date;
                    month = tm.mon;
                    year = tm.year;
                    lcd.locate(0, 1);
                    lcd.printf("Date:%0*d",4 ,year);//+2000
                    lcd.printf("/%0*d",2 ,month);
                    lcd.printf("/%0*d",2 ,date);
                    lcd.locate(0, 2);
                    lcd.printf("Time:%0*d",2 ,hours);
                    lcd.printf(":%0*d",2 ,min);
                    lcd.printf(":%0*d",2 ,sec);
                }
            }
        }*/
//wait(3);refresh_lcd();
    //DS1307
    //char c;
//    Rtc_Ds1307::Time_rtc tm = {};
    //DS1307
    T48.stop();
    T48.reset();
    ActiveT48=1;
    T49.stop();
    T49.reset();
    ActiveT49=1;
    T50.stop();
    T50.reset();
    ActiveT50=1;
    T51.stop();
    T51.reset();
    ActiveT51=1;
    T52.stop();
    T52.reset();
    ActiveT52=1;
    refresh_lcd();
    ActiveT48=0;
    ActiveT49=0;
    ActiveT50=0;
    ActiveT51=0;
    ActiveT52=0;
    FormsOK=0;
    DownDornsOK=0;
    DownClipsOK=0;
    UpClipsOK=0;
    InjectorOK=0;
    Y42L=OFF;
    Y42P=OFF;
    Y52L=OFF;
    Y52P=OFF;
    Y43=OFF;
    Y44=OFF;
    Y46=OFF;
    Y47=OFF;
    Y51=OFF;
    K44=OFF;
    K45=OFF;
    mm = 0;
    cc=0;

//Основной цикл программы ***********************************************************************************************************************
//Основной цикл программы ***********************************************************************************************************************
//Основной цикл программы ***********************************************************************************************************************
//Основной цикл программы ***********************************************************************************************************************
//Основной цикл программы ***********************************************************************************************************************
//Основной цикл программы ***********************************************************************************************************************
    while(1) {  //Основной цикл программы ***********************************************************************************************************************
        //rtc.getTime( &second, &minute, &hour, &dayOfWeek, &dayOfMonth, &month, &year);
        //rtc.gettime( &sec, &min, &hours, &day, &date, &month, &year);
        if (rtc.getTime(tm) ) {
            sec = tm.sec;
            min = tm.min;
            hours = tm.hour;
            date = tm.date;
            month = tm.mon;
            year = tm.year;
            //day = tm.day;
        }
        // while (year!=2015) {lcd.locate(0, 0);            lcd.printf("Startup ERROR 0x01");}
        // while (month>10) {lcd.locate(0, 0);            lcd.printf("Startup ERROR 0x02");}
        //lcd.locate(0, 0);
        //lcd.printf("Set Date&Time!!!");

        //если больше заданного то висим ;-)
        ReadTimers();
        KeyPadRead();
        AutomaticMode=S61a;
        if(AutomaticMode==0) {
            if (flag5!=2) {
                flag5=2;
                StepFlag=0;
                T48.stop();
                T48.reset();
                ActiveT48=1;
                T49.stop();
                T49.reset();
                ActiveT49=1;
                T50.stop();
                T50.reset();
                ActiveT50=1;
                T51.stop();
                T51.reset();
                ActiveT51=1;
                T52.stop();
                T52.reset();
                ActiveT52=1;
                refresh_lcd();
                ActiveT48=0;
                ActiveT49=0;
                ActiveT50=0;
                ActiveT51=0;
                ActiveT52=0;
                FormsOK=0;
                DownDornsOK=0;
                DownClipsOK=0;
                UpClipsOK=0;
                InjectorOK=0;
                Y42L=OFF;
                Y42P=OFF;
                Y52L=OFF;
                Y52P=OFF;
                Y43=OFF;
                Y44=OFF;
                Y46=OFF;
                Y47=OFF;
                Y51=OFF;
                K44=OFF;
                K45=OFF;
                mm = 0;
                cc=0;
            }
        }
        /*while(flagTime!=1) {
            //Работа с часами через консоль
            //if (rtc.getTime(tm) ) {
            pt();//pc.printf("%02d:%02d:%02d", tm.hour, tm.min, tm.sec);
            pc.printf("Enter command =>");
            //}
            while( (c = pc.getc()) != '\r') {
                buffer[readptr++] = c;
            }
            buffer[readptr++] = 0;
            if (strncmp(buffer, "read", 4) == 0) {
                //  perform read
                //pc.printf("Performing read operation\n\r");
                if (rtc.getTime(tm) ) {
                    pc.printf("The current time is : %02d:%02d:%02d\n\r", tm.hour, tm.min, tm.sec);
                    pc.printf("The current date is : %s, %02d/%02d/%04d\n\r", rtc.weekdayToString(tm.wday), tm.date, tm.mon, tm.year);
                }


            } else if (strncmp(buffer, "write", 5) == 0) {
                //  perform write
                pc.printf("Enter the date (date 0..31)");
                pc.scanf("%d", &tm.date);
                pc.printf("Enter the date (month 1..12)");
                pc.scanf("%d", &tm.mon);
                pc.printf("Enter the date (year)");
                pc.scanf("%d", &tm.year);
                pc.printf("Enter the time (hours 0..23)");
                pc.scanf("%d", &tm.hour);
                pc.printf("Enter the time (minutes 0..59)");
                pc.scanf("%d", &tm.min);
                pc.printf("Enter the time (seconds 0..59)");
                pc.scanf("%d", &tm.sec);
                pc.printf("Performing write operation\n\r");

                while(pc.readable())
                    pc.getc();
                rtc.setTime(tm, false, false);
            } else if (strncmp(buffer, "start", 5) == 0) {
                //  start
                pc.printf("Performing start operation\n\r");
                rtc.startClock();
            } else if (strncmp(buffer, "stop", 4) == 0) {
                //  stop
                pc.printf("Performing stop operation\n\r");
                rtc.stopClock();
            } else if (strncmp(buffer, "ena", 3) == 0) {
                int rs;
                pc.printf("Please specify the frequency : [0 = 1Hz, 1 = 4.096kHz, 2 = 8.192kHz, 3 = 32.768kHz] ");
                scanf("%d", &rs);
                pc.printf("Enabling the output with %d option\n\r", rs);
                rtc.setSquareWaveOutput(true, (Rtc_Ds1307::SqwRateSelect_t)rs);
            } else if (strncmp(buffer, "dis", 3) == 0) {
                pc.printf("Disableing square wave output\n\r");
                rtc.setSquareWaveOutput(false, Rtc_Ds1307::RS1Hz);
            } else if (strncmp(buffer, "h", 1) == 0) {
                //  help
                //pc.printf(char(169));
                pc.printf("*************************************\n\r");
                pc.printf("* Menu for RTC Test :               *\n\r");
                pc.printf("* read  - reads the clock           *\n\r");
                pc.printf("* start - start the clock           *\n\r");
                pc.printf("* stop  - stop the clock            *\n\r");
                pc.printf("* write - write the clock           *\n\r");
                pc.printf("* ena   - enable Square wave output *\n\r");
                pc.printf("* dis   - disable square wave outp. *\n\r");
                pc.printf("* exit  - EXIT                      *\n\r");
                pc.printf("* h     - print this menu !!!!!!!!!!*\n\r");
                pc.printf("*************************************\n\r");
            } else if (strncmp(buffer, "exit", 4) == 0) {
                //  null
                flagTime=1;
                pc.printf("exit ok.\n\r");
            } else {
                pc.printf("syntax error\n\r");
            }
            readptr = 0;
            //pc.printf("\n\r\n\r\n\r");
        }*/
        //Конец работы с часами через консоль




//00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
        //Основной цикл работы
        //StepFlag - Номер текущей операции основного цикла
        if (StepFlag == 0) {
            if(AutomaticMode == 1) {
                //Установка исходного состояния
                if (flag5!=1) {
                    SETUP.reset();
                    SETUP.start();
                    mm = 0;
                    cc=0;
                    ActiveSETUP=1;
                    T48.stop();
                    T48.reset();
                    ActiveT48=1;
                    T49.stop();
                    T49.reset();
                    ActiveT49=1;
                    T50.stop();
                    T50.reset();
                    ActiveT50=1;
                    T51.stop();
                    T51.reset();
                    ActiveT51=1;
                    T52.stop();
                    T52.reset();
                    ActiveT52=1;
                    refresh_lcd();
                    ActiveT48=0;
                    ActiveT49=0;
                    ActiveT50=0;
                    ActiveT51=0;
                    ActiveT52=0;
                    FormsOK=0;
                    DownDornsOK=0;
                    DownClipsOK=0;
                    UpClipsOK=0;
                    InjectorOK=0;
                    flag0=0;
                    flag1=0;
                    flag2=0;
                    flag3=0;
                    flag4=0;
                    flag5=1;
                }
                //Время на выполнения исполнительных устройств за которое станок приводится в исходное состояние 3 секунды
                //1. Верхние ножницы поднимаем и раскрываем
                if (UpClipsOK!=true) {
                    if (S41==LOW) {  //   S41=LOW - Верхние ножницы НЕ В НИЖНЕМ ПОЛОЖЕНИИ
                        //Верхние ножницы в исходном состоянии
                        Y51=OFF;         //   Y51=OFF - Верхние ножницы поднять ВВЕРХ
                        Y43=OFF;         //   Y43=OFF - Верхние ножницы РАСКРЫТЬ
                        ////////////OUT_PORTS();
                        UpClipsOK=true;
                    } else {
                        if(flag1!=1) {
                            Y51=OFF;         //   Y51=OFF - Верхние ножницы поднять ВВЕРХ
                            Y43=OFF;         //   Y43=OFF - Верхние ножницы РАСКРЫТЬ
                            //////OUT_PORTS();
                            UpClipsOK=false;
                            flag1=1;
                        }
                    }
                }
                if (FormsOK!=true) {
                    //2. Раскрываем формы
                    if (S43 == HIGH && S42 == LOW) {       //   S43 - Формы РАСКРЫТЫ    S42 - Формы ЗАКРЫТЫ
                        FormsOK=true;
                        Y42L=OFF;      //   Y42L - ЗКРЫВАНИЕ форм
                        Y42P=OFF;       //   Y42P - ОТРЫВАНИЕ форм
                        //////OUT_PORTS();
                    } else {
                        if(flag2!=1) {
                            FormsOK=false;
                            Y42L=OFF;      //   Y42L - ЗКРЫВАНИЕ форм
                            Y42P=ON;       //   Y42P - ОТРЫВАНИЕ форм
                            //////OUT_PORTS();
                            flag2=1;
                        }
                    }
                }

//                if (DownDornsOK!=true) {
//                    //3. Нижние дорны внизу
//                    if (S48 == LOW) { // S48 - Нижние дорны ВНИЗУ
                DownDornsOK=true;
                Y47 = OFF;// Y47 - Нижние дорны UP
                Y52L=ON;  //UpDorns go UP
                Y52P=OFF;
//                        //////OUT_PORTS();
//                    } else {
//                        if(flag3!=1) {
//                            Y47 = OFF;// Y47 - Нижние дорны UP
//                            //////OUT_PORTS();
//                            flag3=1;
//                        }
//                    }
//                }

                if (DownClipsOK!=true) {
                    //4. Выносные ножницы РАСКРЫТЫ и находятсы в положении ОТ ФОРМ
                    if (S45 == HIGH && S44 == LOW) {       // S45 - Выносные ножницы в положении ОТ ФОРМ; S44 - Выносные ножницы НАД ФОРМАМИ
                        DownClipsOK=true;
                        K44=OFF;  //  K44 - Выносные ножницы движение К ФОРМАМ
                        K45=OFF;   //  K45 - Выносные ножницы движение ОТ ФОРМ
                        Y46=OFF; // Y46 - Выносные ножницы РАСКРЫТЬ
                        //////OUT_PORTS();
                    } else {
                        if(flag4!=1) {
                            DownClipsOK=false;
                            K44=OFF;  //  K44 - Выносные ножницы движение К ФОРМАМ
                            Y46=OFF; // Y46 - Выносные ножницы РАСКРЫТЬ
                            //////OUT_PORTS();
                            wait(0.1);
                            K45=ON;   //  K45 - Выносные ножницы движение ОТ ФОРМ
                            //////OUT_PORTS();
                            //                          pc.printf("Down clips opening and go back to forms...                                                                                       \n\r");
                        }
                    }
                }
                if (InjectorOK!=true) {
                    //5. Выдувание отключено
                    Y44=OFF;  //Y44 - Выдувание
                    ////OUT_PORTS();
                    //                  pt();
//                    pc.printf("Injector OFF OK                                                                                       \n\r");
                    InjectorOK=true;
                }
//              if (FormsOK && DownDornsOK && DownClipsOK && UpClipsOK && InjectorOK) {   //revector 2018/07/19
                if (FormsOK                && DownClipsOK && UpClipsOK && InjectorOK) {
//                    pt();

                    CheckStartEnable(); // Дата и время остановки !!!
                }
                if (CurrentSETUP > SetSETUP) {
                    ReadTimers();
                    ActiveSETUP=0;
                    pt();
                    pc.printf("SYSTEM ERROR!!! Restart setup...                                                                                       \n\r");
                    pc.printf("\n\r");
                    if (!FormsOK) {
                        pt();
                        pc.printf("Forms ERROR!!!                                                                                       \n\r");
                    }
                    if (!DownDornsOK) {
                        pt();
                        pc.printf("Down dorns ERROR!!!                                                                                       \n\r");
                    }
                    if (!DownClipsOK) {
                        pt();
                        pc.printf("Down clips ERROR...                                                                                       \n\r");
                    }
                    if (!UpClipsOK) {
                        pt();
                        pc.printf("Up clips ERROR!!!                                                                                       \n\r");
                    }
                    flag5=0;
                    pt();
                    pc.printf("......REPEAT SETUP!!!                                                                                       \n\r");
                    pt();
                }
            } else { //Manual MODE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//++++++++++++++ ЗАЩИТА ДВИГАТЕЛЯ ДВИЖЕНИЯ ВЫНОСНЫХ НОЖНИЦ +++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//                if (K44 == ON && S44 == HIGH) { // Если выносные ножницы в исходном положении (от форм) то выключаем движение от форм
//  /                  K44 = OFF;
//                }
//                if (K45 == ON && S45 == HIGH) { // Если выносные ножницы над формами то выключаем движение к формам
//                    K45 = OFF;
//                }
                //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//******** Открывание и Закрывание форм ********
                if (S63 == LOW && S64 == HIGH) { //Открываем формы
                    if (o != 1) {
                        Y42P = 1;
                        Y42L = 0;
                        o = 1;
                    }
                }
                if (S63 == HIGH && S64 == LOW) { //Закрываем формы
                    if (o != 2) {
                        Y42P = 0;
                        Y42L = 1;
                        o = 2;
                    }
                }
                if (S63 == 0 && S64 == 0) { //Стоп формы
                    if (o != 0) {
                        Y42P = 0;
                        Y42L = 0;
                        o = 0;
                    }
                }
                if (S63 == 1 && S64 == 1) { //Стоп формы
                    if (o != 0) {
                        Y42P = 0;
                        Y42L = 0;
                        o = 0;
                    }
                }
                //******** Движение выносных ножниц ***********
                if (S66 == HIGH && S67 == LOW) { //Нажата кнопка Выносные ножницы К формам S66
                    if (S44 == LOW) {
                        if (m == 0) {
                            K44 = 0;
                            K45 = 0;
                            wait(0.1);
                            K44 = 1;
                            m = 1;
                        }
                    } else {
                        if (m == 1) {
                            K44 = 0;
                            K45 = 0;
                            m = 0;
                        }
                    }
                }
                if (S66 == LOW && S67 == HIGH) { //Выносные ножницы ОТ форм
                    if (S45 == LOW) {
                        if (m == 0) {
                            K45 = 0;
                            K44 = 0;
                            //////OUT_PORTS();
                            wait(0.1);
                            K45 = 1;
                            //////OUT_PORTS();
                            m = 1;
                        }
                    } else {
                        if (m == 1) {
                            K44 = 0;
                            K45 = 0;
                            m = 0;
                        }
                    }
                }
                if (S66 == LOW && S67 == LOW) { //Выносные ножницы СТОП
                    if (m != 0) {
                        K44 = 0;
                        K45 = 0;
                        m = 0;
                    }
                }
                if (S66 == HIGH && S67 == HIGH) { //Выносные ножницы СТОП
                    if (m != 0) {
                        K44 = 0;
                        K45 = 0;
                        m = 0;
                    }
                }
            }
            //////OUT_PORTS();if(display_switch==3){refresh_lcd();}
        }//00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000
//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
//Timer T50;  //   2. T50 - Выдержка на верхние ножницы      0- 10 сек   // Время после команды на смыкание верхних ножниц до начала опускания верхних ножниц.
//int SetT50 = 0;
//
//             S41  1 - Верхние ножницы внизу                  HIGH = Активно
//             S42  2 - Формы Закрыты                          HIGH = Активно
//             S43  3 - Формы Открыты                          HIGH = Активно
//             S44  4 - Выносные ножницы над формой            HIGH = Активно
//             S45  5 - Выносные ножницы в исходном состоянии  HIGH = Активно
//             S48  6 - Дорны внизу                            HIGH = Активно
//
//             Y42L  1 - Закрывание форм                        Электроклапан
//             Y42P  2 - Открывание форм                        Электроклапан
//             Y43   3 - Сжатие верхних ножниц                  Электроклапан
//             Y46   4 - Сжатие нижних ножниц                   Электроклапан
//             Y47   5 - Опускание нижних дорнов                Электроклапан
//             Y51   6 - Опускание верхних ножниц               Электроклапан
//             Y52L  7 - Верхние дорны вверх                    Электроклапан
//             Y52P  8 - Верхние дорны вниз                     Электроклапан
//             K44   9 - Выносные ножницы - движение К  форме   Пускатель!!!
//             K45  10 - Выносные ножницы - движение ОТ формы   Пускатель!!!
//             Y44  11 - Выдувание                              Электроклапан
//
//
// 1. Смыкание верхних ножниц Y43=ON
// 2. Запуск таймера T50 (время до смыкания ножниц)
//      И поднимаем нижние дорны (Y47=OFF)
// 3. По окончании таймера и отпускании датчика S48(Нижние дорны внизу)опускаем верхние ножницы Y51=ON до нажатия концевика S41(верхние ножницы внизу)
//      И раскрываем нижние ножницы Y46=OFF
// 4. После срабатывания (S41(Верхние ножницы опущены)) Начинаем закрывать формы Y42L=ON Y42P=OFF до S42=1(формы закрыты) И
//      И одновременно запускаем ********** Процедуру выдувания-отдувки (T51(время до начала выдувания)+T48(время выдувки)+T49(Время отдувки))
// 5. Если формы закрыты (S43=1(формы закрыты) & S42=0(формы раскрыты)) - открываем верхние ножницы Y43=OFF и поднимаем их Y51=OFF
//      И если нижние ножницы не над формами S44=0 ТО двигаем нижние ножницы к формам K45=OFF K44=ON
// 6. Если таймер T49 закончился то раскрываем формы Y42L=OFF Y42P=ON
// 7. Если формы прироткрылись S42=0 то опускаем нижние дорны Y47=ON
// 8. Усли формы раскрыты S43=1 и нижние дорны внизу S48=1 то двигаем нижние ножницы От форм K44=OFF K45=ON до S45=1
// 9. Если выносные ножницы в исходном состоянии S45=1 то K44=OFF K45=OFF переход на ШАГ_1.
// Промежуточный_1:  Если K44=ON и S44=1 то K44=OFF K45=OFF
// Промежуточный_2:  Если таймер T51 закончился то запускаем таймер T48
// Промежуточный_3:  Если таймер T48 закончился то запускаем таймер T49
        if (StepFlag == 1) {
            if (flag0!=1) {
                pc.printf("Step 1 START...                                                                                       \n\r");
                T48.reset();
                T49.reset();
                T50.reset();
                T51.reset();
                T52.reset();
                OneCycleTime.reset();
                ActiveT48=1;
                ActiveT49=1;
                ActiveT50=1;
                ActiveT51=1;
                ActiveT52=1;
                ReadTimers();
                ALARM.stop();
                ALARM.reset();
                OneCycleTime.start();
                T50.start();
                mm=0;
                flag0=1;
                pc.printf("UpClips CLOSE (Y43= ON)...                                                                                       \n\r");
                Y43=ON;
                pc.printf("DownDorns UP  (Y47=OFF)...                                                                                       \n\r");
                Y47=OFF;
            }
            if (CurrentT50 > SetT50) {
                T50.stop();
                ReadTimers();
                ActiveT50=0;
                pc.printf("Timer T50 END.                                                                                       \n\r");
                pc.printf("Step 1 END.                                                                                       \n\r");
                StepFlag = 2;
                flag0=0;
            }
        }//11111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111
//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
        if (StepFlag == 2) {
            if (flag0!=1) {
                pc.printf("Step 2 START...                                                                                       \n\r");
                flag0=1;
                pc.printf("Wait S48=LOW(DownDorns UP)...                                                                                       \n\r");
            }
            if (S48==LOW) {
                pc.printf("DownClips OPEN (Y46=OFF)...                                                                                       \n\r");
                Y46=OFF;
                pc.printf("UpClips DOWN   (Y51= ON)...                                                                                       \n\r");
                Y51=ON;
                pc.printf("Step 2 END.                                                                                       \n\r");
                pc.printf("                                                                                       \n\r");
                StepFlag = 3;
                flag0=0;
            }
        }//22222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222222
//33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333
        if (StepFlag == 3) {
            if (flag0!=1) {
                pc.printf("Step 3 START. Wait S41=HIGH  (UpClips DOWN)...                                                                             \n\r");
                flag0=1;
                pc.printf("Wait S41=HIGH  (UpClips DOWN)...                                                                                       \n\r");
            }
            if (S41==HIGH) {
                pc.printf("UpClips DOWN. Close FORMS... START UpDorns to DOWN... START Injector... Step 3 END.                                      \n\r");
                //S41_print();
                Y42L=ON; //Закрываем формы
                Y42P=OFF;
                //////OUT_PORTS();
                StartUpDorns = 1;
                FlagUpDorns = 0;
                StartInjector = 1;
                FlagInjector = 0;
                EndInjector = 0;
                pc.printf("                                                                                       \n\r");
                StepFlag = 4;
                flag0=0;
            }
        }//33333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333333
//44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
        if (StepFlag == 4) {
            if (flag0!=1) {
                pc.printf("Step 4 START. Wait S42=HIGH  (FORMS Closed)...                                                                                  \n\r");
                flag0=1;
            }
            if (S42==HIGH) {
//For NEW Valve     Для старых комментируем
                if (NewValve == 1) {
                    Y42L=OFF;   //Выключаем катушку клапана чтобы не перегрелась!!!!!!!!!!!!!!!!!!!
                }
                if (c==0) {
                    pc.printf("UpClips OPEN (Y43=OFF)... UpClips UP (Y51=OFF)... Step 4 END.                                                                 \n\r");
                    Y43=OFF;//Open
                    wait(0.5);
                    TempTimer.reset();
                    TempTimer.start();
                    c=1;
                }
                if (c==1 && TempTimer.read() > 0.5) {
                    TempTimer.stop();
                    TempTimer.reset();
                    Y51=OFF;//Up
                    StepFlag = 5;
                    flag0=0;
                    c=0;
                }
            }
        }//44444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444
//55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555
        if (StepFlag == 5) {
            if (flag0!=1) {
                pc.printf("Step 5 START. Wait S41=LOW  (UpClips UP)...                                                                            \n\r");
                flag0=1;
            }
            if (S41==LOW) {
                mm=0;
                //cc=0;
                Down_Clips_go_Forms = 1;
                pc.printf("Down Clips go Forms... Step 5 END.                                                                                  \n\r");
                StepFlag = 6;
                flag0=0;
            }
        }//55555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555555
//66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666
        if (StepFlag == 6) {
            if (flag0!=1) {
                pc.printf("Step 6 START. Wait S44=HIGH && S45=LOW (DownClips ON FORMS)...                                                                    \n\r");
                flag0=1;
            }
            if (S44==HIGH && S45 == LOW) {
                pc.printf("Step 6 END.                                                                                 \n\r");
                StepFlag = 7;
                flag0=0;
            }
        }//66666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666666
//77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777
        if (StepFlag == 7) {
            if (flag0!=1) {
                pc.printf("Step 7 START. Wait END Injector...\n\r");
                pc.printf("                                                                                       \n\r");
                flag0=1;
            }
            if (EndInjector==1) {
                //Down_Clips_go_Forms = 1;
                pc.printf("END Injector... Opening FORMS... Step 7 END.                                             \n\r");
                //Y52L = ON;
                //Y52P = OFF;
                Y42L = OFF;
                Y42P = ON;
                Y47  = ON;  // Опускаем нижние дорны  - Для схемы мез концевика нижних дорнов!!!!
                StepFlag = 8;
                flag0=0;
            }
        }//77777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777777
//88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
        if (StepFlag == 8) {
            if (flag0!=1) {
                pc.printf("Step 8 START. Wait S42=LOW FORMS Open...                                                                                       \n\r");
                flag0=1;
            }
            if (S42==LOW) {
//For NEW Valve     Для старых комментируем
                if (NewValve == 1) {
                    Y42P=OFF;   //Выключаем катушку клапана чтобы не перегрелась!!!!!!!!!!!!!!!!!!!
                }
                pc.printf("Step 8 END.                                                                                       \n\r");
                Y47=ON;


                StepFlag = 9;
                flag0=0;
            }
        }//88888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888888
//99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
        if (StepFlag == 9) {
            if (flag0!=1) {
                pc.printf("Step 9 START. Wait S43=HIGH FORMS Open and S48=HIGH DownDorns DOWN...                                                                                       \n\r");
                flag0=1;
            }
//          if (S43==HIGH && S48==HIGH) {   //revector 2018/07/19
            if (S43==HIGH             ) {
                //Вставить проверку опускания нижних дорнов !!!!!!!!!!!+++++++++++++++++++!!!!!!!!!!!!!+++++++++++++++++++++!!!!!!!!!!!!!!!!!!!!!!
                //refresh_terminal();
                pc.printf("UpDorns UP...                                                                                       \n\r");
                Y52L = ON;
                Y52P = OFF;
                pc.printf("FORMS Opened. Step 9 END.                                                                                      \n\r");
                mm=2;
                Down_Clips_back_Forms=1;
                StepFlag = 10;
                flag0=0;
            }
        }//99999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999
//10101010101010101010101010101010101010101010101010101010101010101010101010101010110101010101010101010101010101010101010101010101010101010101010101010101010101010
        if (StepFlag == 10) {
            if (flag0!=1) {
                pc.printf("Step 10 START. Wait DownClips BACK FORMS...                                                                                       \n\r");
                flag0=1;
            }
            if (S45==HIGH && K45==OFF) {
//Пробуем клапан. Защита от перегрева катушек. Если клапан работает как двухпозиционный!!!
//                Y52L = ON; Y52P = OFF;
                //refresh_terminal();
                pc.printf("Step 10 END.                                                                                       \n\r");
                pc.printf("END CYCLE. RESART PROCESS!!!                                                                                       \n\r");
                count_cycle++;
                eeprom.nbyte_write(0x15, &count_cycle, 4 );
                if (FlagChangeT48!=1) {
                    eeprom.nbyte_write(0x00, &SetT48, 4);
                    FlagChangeT48=1;
                }
                if (FlagChangeT50!=1) {
                    eeprom.nbyte_write(0x05, &SetT50, 4);
                    FlagChangeT50=1;
                }
                if (FlagChangeT51!=1) {
                    eeprom.nbyte_write(0x0a, &SetT51, 4);
                    FlagChangeT51=1;
                }
                if (FlagChangeT52!=1) {
                    eeprom.nbyte_write(0x0f, &SetT52, 4);
                    FlagChangeT52=1;
                }
                CurrentOneCycleTime=OneCycleTime.read();
                CheckStartEnable();
                //     StepFlag = 1;
                //     flag0=0;
                if (rtc.getTime(tm) ) {
                    sec = tm.sec;
                    min = tm.min;
                    hours = tm.hour;
                    date = tm.date;
                    month = tm.mon;
                    year = tm.year;

                    eeprom.nbyte_write(0x20, &year,  2 );   //Читаем РОМ
                    eeprom.nbyte_write(0x22, &month, 1 );   //Читаем РОМ
                    eeprom.nbyte_write(0x23, &date,  1 );   //Читаем РОМ
                    eeprom.nbyte_write(0x24, &hours, 1 );   //Читаем РОМ
                    eeprom.nbyte_write(0x25, &min,   1 );   //Читаем РОМ
                    eeprom.nbyte_write(0x26, &sec,   1 );   //Читаем РОМ
                }




            }
        }//101010101010101010101010101010101010101010101010101010101010101010101010101010101101010101010101010101010101010101010101010101010101010101010101010101010
//ENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDENDEND
        // StartInjector - процесс последовательного выполнения трех таймеров:
        // Т51 - Время до начала выдувания
        // Т48 - Продолжительность выдувания + Включение клапана подачи воздуха в формы Y44
        // Т49 - Время сбрасывания давления в формах
        //if (AutomaticMode == 1) {
        if (StartInjector == 1) {
            if (FlagInjector == 0) {
                //Запускаем таймер Т51 T[3] Время до начала выдувания
                T51.reset();  //Запускаем таймер Т51 T[3] Время до начала выдувания
                T51.start();
                ActiveT51=1;
                ReadTimers();
                FlagInjector = 1;
            }
            if (FlagInjector == 1 && CurrentT51 > SetT51) {
                //Включаем выдувание
                T51.stop();
                ReadTimers();
                ActiveT51=0;
                Y44 = ON; //Включаем выдувание
                T48.reset();
                T48.start();  // Запускаем таймер Т48 Время начала выдувания
                ReadTimers();
                ActiveT48=1;
                FlagInjector = 2;
            }
            if (FlagInjector == 2 && CurrentT48 > SetT48) {
                T48.stop();
                ReadTimers();
                ActiveT48=0;
                Y44 = OFF; //ВЫключаем выдувание
                T49.reset();
                T49.start();  // Запускаем таймер Т49 время отдувки
                ReadTimers();
                ActiveT49=1;
                FlagInjector = 3;
            }
            if (FlagInjector == 3 && CurrentT49 > SetT49) {
                T49.stop();
                ReadTimers();
                ActiveT48=0;
                StartInjector = 0;
                FlagInjector = 0;
                EndInjector = 1;
            }
        }// INJECTION END


// Промежуточный процесс=============================================================================================
        // Нужен для автоматического движения ножниц к формам и от форм
        // И на обработку таймера открытия верхних ножниз перед их поднятием
        if (Down_Clips_go_Forms == 1) {
            if (S41 == LOW) { //Верхние ножницы не должны быть внизу!!!
                if (mm == 0 && S44 == LOW) {
                    K44 = OFF;
                    K45 = OFF;
                    //////OUT_PORTS();
                    pc.printf("++++++++++++++++++++ Automatic MODE...Down_Clips go to FORMS :                                           \n\r");
                    //wait(0.5);
                    K44 = ON;
                    //////OUT_PORTS();
                    ALARM.reset();
                    ALARM.start();
                    pc.printf("++++++++++++++++++++ Wait for S44 HIGH... (DnClips ON FORMS)                   \n\r");
                    mm = 1;
                }
                if (mm == 1 && S44 == HIGH && S45 == LOW) {
                    ALARM.stop();
                    ALARM.reset();
                    K44 = OFF;
                    K45 = OFF;
                    //Y46 = ON; // Закрываем нижние ножницы!!!!
                    pc.printf("++++++++++++++++++++ Down_Clips ON FORMS; Down_Clips CLOSE                                                 \n\r");
                    cc = 1;
                    mm = 2;
                    DownClips.reset();
                    DownClips.start();
                }
                if (cc==1 && DownClips.read()>0.5) {
                    DownClips.stop();
                    DownClips.reset();
                    cc=0;
                    Down_Clips_go_Forms = 0;
                    Y46 = ON; // Закрываем нижние ножницы!!!!
                }
                if (mm==1 && ALARM.read()>4) { //Мы ждем 4 секунды пока едут ножницы. Если едут долго то мы ПОВИСЛИ и делаем сброс перефирии
                    ALARM.stop();
                    ALARM.reset();
                    K44 = OFF;
                    K45 = OFF;
                    //Вывести на экран аларм по концевику S44
                    lcd.cls();
                    display_switch = 4;
                    lcd.locate(0, 0);
                    lcd.printf("        WANING!     ");
                    lcd.locate(0, 2);
                    lcd.printf("Check S44 and S45!!!");
                    StepFlag=99;
                    T48.stop();
                    T48.reset();
                    ActiveT48=1;
                    T49.stop();
                    T49.reset();
                    ActiveT49=1;
                    T50.stop();
                    T50.reset();
                    ActiveT50=1;
                    T51.stop();
                    T51.reset();
                    ActiveT51=1;
                    T52.stop();
                    T52.reset();
                    ActiveT52=1;
                    refresh_lcd();
                    ActiveT48=0;
                    ActiveT49=0;
                    ActiveT50=0;
                    ActiveT51=0;
                    ActiveT52=0;
                    FormsOK=0;
                    DownDornsOK=0;
                    DownClipsOK=0;
                    UpClipsOK=0;
                    InjectorOK=0;
                    Y42L=OFF;
                    Y42P=OFF;
                    Y52L=OFF;
                    Y52P=OFF;
                    Y43=OFF;
                    Y44=OFF;
                    Y46=OFF;
                    Y47=OFF;
                    Y51=OFF;
                    K44=OFF;
                    K45=OFF;
                    mm = 0;
                    Down_Clips_go_Forms =0;
                }
            }
        }
        if (Down_Clips_back_Forms == 1) {
            //if (S43 == HIGH) {//!!! Формы должны быть открыты!!!
            if (mm == 2  && cc==0 && S45 == LOW) {
                K45 = OFF;
                K44 = OFF;
                ////OUT_PORTS();
                pc.printf("---------------------- Automatic MODE...DnClips go back to FORMS :                                                               \n\r");
                //wait(0.5);
                K45 = ON;
                ////OUT_PORTS();
                ALARM.reset();
                ALARM.start();
                mm = 3;
                pc.printf("---------------------- Wait S45 HIGH... (DnClips BACK FORMS)                                           \n\r");
            }
            if (mm == 3 && S45 == HIGH && S44 == LOW) {
                ALARM.stop();
                ALARM.reset();
                K44 = OFF;
                K45 = OFF;
                ////OUT_PORTS();
                pc.printf("---------------------- Automatic MODE...Down_Clips STOP :                                                                         \n\r");
                mm = 0;
                Down_Clips_back_Forms = 0;
            }
            if (mm==3 && ALARM.read()>4) { //Мы ждем 4 секунды пока едут ножницы. Если едут долго то мы ПОВИСЛИ и делаем сброс перефирии
                ALARM.stop();
                ALARM.reset();
                K44 = OFF;
                K45 = OFF;
                // Вывести на экран аларм по концевику S45
                display_switch = 4;
                lcd.cls();
                lcd.locate(0, 0);
                lcd.printf("        WANING!     ");
                lcd.locate(0, 2);
                lcd.printf("Check S44 and S45!!!");
                T48.stop();
                T48.reset();
                ActiveT48=1;
                T49.stop();
                T49.reset();
                ActiveT49=1;
                T50.stop();
                T50.reset();
                ActiveT50=1;
                T51.stop();
                T51.reset();
                ActiveT51=1;
                T52.stop();
                T52.reset();
                ActiveT52=1;
                refresh_lcd();
                ActiveT48=0;
                ActiveT49=0;
                ActiveT50=0;
                ActiveT51=0;
                ActiveT52=0;
                FormsOK=0;
                DownDornsOK=0;
                DownClipsOK=0;
                UpClipsOK=0;
                InjectorOK=0;
                Y42L=OFF;
                Y42P=OFF;
                Y52L=OFF;
                Y52P=OFF;
                Y43=OFF;
                Y44=OFF;
                Y46=OFF;
                Y47=OFF;
                Y51=OFF;
                K44=OFF;
                K45=OFF;
                mm = 0;
                Down_Clips_back_Forms =0;
                StepFlag=99;
            }
            //}
        }
        //=====================================================================================================================================
        // Конец промежуточного процесса

        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        // StartUpDorns - процесс последовательного выполнения двух таймеров:
        // Дорны вверх поднимаются при начале раскрытия форм
        // Т53 - Дорны вниз   StartUpDorns
        if (StartUpDorns == 1) {
            if (FlagUpDorns == 0) {
                pc.printf("+ Timer T52 START ... (Time of DnDorns DOWN)                                                      \n\r");
                T52.reset();
                T52.start();  // Запускаем таймер Т52
                ReadTimers();
                ActiveT52=1;
                pc.printf("+ Wait for Timer T52 END....                                                                \n\r");
                FlagUpDorns = 1;
            }
            if (FlagUpDorns == 1) {
                if (CurrentT52 > SetT52) {
                    T52.stop();
                    ReadTimers();
                    ActiveT52=0;
                    //Опускаем верхние дорны
                    pc.printf("+ Timer T53 END. (Time to DnDorns DOWN)                                                    \n\r");
                    Y52P = ON;
                    Y52L = OFF;
                    FlagUpDorns = 0;//2;
                    StartUpDorns = 0;
                    ////OUT_PORTS();
                }
            }
        }
        // StartUpDorns END
        //++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        ////OUT_PORTS();//if(display_switch==3){refresh_lcd();}

        //+++
        refresh_terminal();
        refresh_lcd();

        lcd.locate(18, 0);
        lcd.printf("%0*d",2, StepFlag);
        if (led==0) {
            led=1;
            if (display_switch==2) {
                refresh_lcd();
            }
        } else {
            led=0;
        }
        myled1 = led;
    } //Основной цикл программы
}//Main END.
/**********************************************************************
* Function: get_user_input()
* Parameters: message - user prompt
*             min - minimum value of input
*             max - maximum value of input
*             member - pointer to struct member
* Returns: none
*
* Description: get time/date input from user
*
**********************************************************************/
void get_user_input(char* message, uint8_t min, uint8_t max, uint32_t* member)
{
    uint32_t temp;

    do {
        printf("\n%s", message);

        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        scanf("%d", &temp);

        *member = temp;

        if((*(member)< min) || (*(member) > max)) {
            printf("\n\rERROR-RTI");
        }
    } while((*(member) < min) || (*(member) > max));
}


void get_user_input(char* message, uint8_t min, uint8_t max, bool* member)
{
    uint32_t temp;

    do {
        printf("\n%s", message);

        //for some reason mbed doesn't like a pointer to a member in scanf
        //term.scanf("%d", member); works with gcc on RPi
        scanf("%d", &temp);

        *member = temp;

        if((*(member)< min) || (*(member) > max)) {
            printf("\nERROR-RTI");
        }
    } while((*(member) < min) || (*(member) > max));
}


