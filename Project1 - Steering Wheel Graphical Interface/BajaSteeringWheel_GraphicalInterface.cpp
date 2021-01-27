// Code written by Tyler Montcalm for the Baja ECVT capstone
// This code displays information that was received from the ECVT stm32 board via SPI onto a 3.5inch tft display



//////////////SCREEN_H_FILES//////////////
#include <Adafruit_GFX.h>    // Core graphics library
#include <MCUFRIEND_kbv.h>   // Hardware-specific library
MCUFRIEND_kbv tft;
//////////////BOARD_TO_BOARD_H_FILES//////////////
#include <SPI.h>
//////////////SCREEN_FONTS//////////////
#include <Fonts/FreeSans9pt7b.h>
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSerif12pt7b.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <FreeDefaultFonts.h>
//////////////COLORS//////////////
#define BLACK           0x0000
#define RED             0xF800
#define GREEN           0x07E0
#define WHITE           0xFFFF
#define GREY            0x8410
#define LTBLUE          0xB6DF
#define LTTEAL          0xBF5F
#define LTGREEN         0xBFF7
#define LTCYAN          0xC7FF
#define LTRED           0xFD34
#define LTMAGENTA       0xFD5F
#define LTYELLOW        0xFFF8
#define LTORANGE        0xFE73
#define LTPINK          0xFDDF
#define LTPURPLE        0xCCFF
#define LTGREY          0xE71C
#define BLUE            0x001F
#define TEAL            0x0438
#define GREEN           0x07E0
#define CYAN            0x07FF
#define RED             0xF800
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define ORANGE          0xFD20
#define PINK            0xF81F
#define PURPLE          0x801F
#define GREY            0xC618
#define WHITE           0xFFFF
#define BLACK           0x0000
#define DKBLUE          0x000D
#define DKTEAL          0x020C
#define DKGREEN         0x03E0
#define DKCYAN          0x03EF
#define DKRED           0x6000
#define DKMAGENTA       0x8008
#define DKYELLOW        0x8400
#define DKORANGE        0x8200
#define DKPINK          0x9009
#define DKPURPLE        0x4010
#define DKGREY          0x4A49
//////////////VARIABLES_SPI//////////////
char buf[100];
char buf_past[100];
volatile byte pos;
volatile boolean process_it;
int rpm1=2000;
int rpm2=1000;
int gear;
bool first_manual;
bool first_launch;
bool first_auto;
//////////////VARIABLES_SCREEN//////////////
bool new_manual_press;
bool new_auto_press;
bool new_launch_press;
bool was_launch;
bool was_manual;
//////////////FUNCTIONS_SCREEN//////////////
void initial_drawings();
void check_mode();
void manual_display();
void auto_display();
void launch_display();
void test_input_data();
void showmsgXY_white(int x, int y, int sz, const GFXfont *f,  char buf);
void showmsgXY_msgwhite(int x, int y, int sz, const GFXfont *f, const char *msg);
void showmsgXY_msgblue(int x, int y, int sz, const GFXfont *f, const char *msg);
void showmsgXY_msgyellow(int x, int y, int sz, const GFXfont *f, const char *msg);
void save_last();

/////////////TEMP_VARIABLE///////////
//////////////SETUP/////////////
void setup(void)
{
Serial.begin(9600);
uint16_t ID = tft.readID();
if (ID == 0xD3) ID = 0x9481;
tft.begin(ID);
tft.setRotation(1);
pinMode(MISO,OUTPUT);
pinMode(10,INPUT_PULLUP);
SPCR |= bit (SPE);
pos=0;
process_it =false;
tft.fillScreen(BLACK);
//initial_drawings();
SPI.attachInterrupt();
//test_input_data();
new_manual_press=true;
new_auto_press=true;
new_launch_press=true;
bool first_manual=true;
bool first_launch=true;
bool first_auto=true;
}

ISR (SPI_STC_vect)
{
byte c = SPDR;  // grab byte from SPI Data Register
  
  // add to buffer if room
  if (pos < (sizeof (buf) - 1))
    buf [pos++] = c;
    
  // example: newline means time to process buffer
  if (pos>6)
  {
    process_it = true;
    cli();
  }
}

void loop(void)
{  
  cli();
 manual_display();
  
    if (process_it)
    {
    buf [pos] = 0;  
    Serial.println();
    Serial.print(buf[0]);
    Serial.print(buf[1]);
    Serial.print(buf[2]);
    Serial.print(buf[3]);
    Serial.print(buf[4]);
    Serial.print(buf[5]);
    Serial.print(buf[6]);
    pos = 0;
    process_it = false;
    }  // end of flag set
    save_last();
    sei();
    delay(10);
    
}


void showmsgXY(int x, int y, int sz, const GFXfont *f,  char buf)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(GREEN);
    tft.setTextSize(sz);
    tft.print(buf);
    
   
}
void showmsgXY_white(int x, int y, int sz, const GFXfont *f,  char buf)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(WHITE);
    tft.setTextSize(sz);
    tft.print(buf);
    
   
}

void showmsgXY_red(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(RED);
    tft.setTextSize(sz);
    tft.print(msg);
    
}
void showmsgXY_msgwhite(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(WHITE);
    tft.setTextSize(sz);
    tft.print(msg);
    
}
void showmsgXY_msgblue(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(BLUE);
    tft.setTextSize(sz);
    tft.print(msg);
    
}
void showmsgXY_msgyellow(int x, int y, int sz, const GFXfont *f, const char *msg)
{
    int16_t x1, y1;
    uint16_t wid, ht;
    tft.setFont(f);
    tft.setCursor(x, y);
    tft.setTextColor(YELLOW);
    tft.setTextSize(sz);
    tft.print(msg);
    
}

void initial_drawings()
{
showmsgXY_red(25, 140, 1, &FreeSerif12pt7b, "RPM1");
showmsgXY_red(130, 140, 1, &FreeSerif12pt7b, "RPM2");
showmsgXY_red(220, 140, 1, &FreeSerif12pt7b, "GEAR RATIO");
showmsgXY_red(400, 140, 1, &FreeSerif12pt7b, "CVT T");
showmsgXY_red(20, 40, 1, &FreeSerif12pt7b, "TIMER");
tft.drawRect(0,0,230,60,WHITE);
tft.drawRect(-10,110,120,300,WHITE);
tft.drawRect(110,110,100,300,WHITE);
tft.drawRect(210,110,160,300,WHITE);
tft.drawRect(370,110,130,300,WHITE);
}

void check_mode()
{
  
}

void manual_display()
{
  if(new_manual_press)
  {
   tft.fillScreen(BLACK);
   showmsgXY_msgwhite(70, 40, 1, &FreeMonoBoldOblique24pt7b, "RPM:");
   showmsgXY_msgblue(10,300, 2, &FreeMonoBoldOblique24pt7b, "M");
  // tft.drawRect(170,95,140,150,WHITE);
   new_manual_press=false;
  }
  else
  {
   if(buf[0]!=buf_past[0]||first_manual==true)
   {
    tft.fillRect(174,10,30,35,BLACK);
    showmsgXY(175, 40, 1, &FreeMonoBoldOblique24pt7b, buf[0]);
    }
   if(buf[1]!=buf_past[1]||first_manual==true)
   {
    tft.fillRect(204,10,30,35,BLACK);
    showmsgXY(205, 40, 1, &FreeMonoBoldOblique24pt7b, buf[1]);
   }
   if(buf[2]!=buf_past[2]||first_manual==true)
   {
    tft.fillRect(234,10,30,35,BLACK);
    showmsgXY(235, 40, 1, &FreeMonoBoldOblique24pt7b, buf[2]);
   }
   if(buf[3]!=buf_past[3]||first_manual==true)
   {
     tft.fillRect(264,10,30,35,BLACK);
     showmsgXY(265, 40, 1, &FreeMonoBoldOblique24pt7b, buf[3]);
   }
   if(buf[4]!=buf_past[4]||first_manual==true) 
   {
    tft.fillRect(171,96,140,150,BLACK);
    showmsgXY(180, 220, 4, &FreeMonoBoldOblique24pt7b, buf[4]);
   }
   first_manual=false;
  }
}
void launch_display()
{
  if(new_launch_press)
  {
   tft.fillScreen(BLACK);
   showmsgXY_msgyellow(50, 40, 1, &FreeMonoBoldOblique24pt7b, "LAUNCH CONTROL");
   showmsgXY_msgwhite(40, 190, 1, &FreeMonoBoldOblique24pt7b, "RPM: ");
   new_launch_press=false;
  }
  else
  {
   if(buf[0]!=buf_past[0])
   {
    tft.fillRect(179,140,60,65,BLACK);
    showmsgXY(180, 200, 2, &FreeMonoBoldOblique24pt7b, buf[0]);
   }
   if(buf[1]!=buf_past[1])
   {
    tft.fillRect(239,140,60,65,BLACK);
    showmsgXY(240, 200, 2, &FreeMonoBoldOblique24pt7b, buf[1]);
   }
   if(buf[2]!=buf_past[2])
   {
    tft.fillRect(309,140,60,65,BLACK);  
    showmsgXY(310, 200, 2, &FreeMonoBoldOblique24pt7b, buf[2]);
   }
   if(buf[3]!=buf_past[3])
   {
    tft.fillRect(379,140,60,65,BLACK);
    showmsgXY(380, 200, 2, &FreeMonoBoldOblique24pt7b, buf[3]);    
   }
  }
}
void auto_display()
{
  if(new_auto_press)
  {
   tft.fillScreen(BLACK);
   showmsgXY_msgwhite(10,300, 2, &FreeMonoBoldOblique24pt7b, "A");
   showmsgXY_msgwhite(320,185, 1, &FreeMonoBoldOblique24pt7b, "KM/H");
   showmsgXY_msgwhite(5,40, 1, &FreeMonoBoldOblique24pt7b, "Temp:");
   new_auto_press=false;
  }
  else
  {  
    if(buf[5]!=buf_past[5])
    {
     tft.fillRect(139,10,30,35,BLACK); 
     showmsgXY(140, 40, 1, &FreeMonoBoldOblique24pt7b, buf[5]);
    }
    if(buf[6]!=buf_past[6])
    {
     tft.fillRect(169,10,30,35,BLACK); 
     showmsgXY(170, 40, 1, &FreeMonoBoldOblique24pt7b, buf[6]);
    }
    if(buf[7]!=buf_past[7])
    {
     tft.fillRect(179,125,65,85,BLACK); 
     showmsgXY(180, 200, 2, &FreeMonoBoldOblique24pt7b, buf[7]);
    }
    if(buf[8]!=buf_past[8])
    {
     tft.fillRect(239,125,65,85,BLACK); 
     showmsgXY(240, 200, 2, &FreeMonoBoldOblique24pt7b, buf[8]); 
    }
  }
}

void test_input_data()
{
  buf[0]='1';
  buf[1]='2';
  buf[2]='3';
  buf[3]='4';
  buf[4]='4';
  buf[5]='0';
  buf[6]='0';
  buf[7]='5';
  buf[8]='5';
}

void save_last()
{
//Serial.println();
for(int i=0;i<100;i++)
{
  buf_past[i]=buf[i];
 // Serial.print(buf_past[i]);
}
}