#include <Arduino.h>
#include <U8g2lib.h>

#include <Bounce2.h>


#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SDA 4
#define SCL 14

/*
  U8glib Example Overview:
    Frame Buffer Examples: clearBuffer/sendBuffer. Fast, but may not work with all Arduino boards because of RAM consumption
    Page Buffer Examples: firstPage/nextPage. Less RAM usage, should work with all Arduino boards.
    U8x8 Text Only Example: No RAM usage, direct communication with display controller. No graphics, 8x8 Text only.
    
  This is a page buffer example.    
*/

// Please UNCOMMENT one of the contructor lines below
// U8g2 Contructor List (Picture Loop Page Buffer)
// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
// Please update the pin numbers according to your setup. Use U8X8_PIN_NONE if the reset pin is not connected
//U8G2_NULL u8g2(U8G2_R0);	// null device, a 8x8 pixel display which does nothing
//U8G2_SSD1306_128X64_NONAME_1_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 12, /* dc=*/ 4, /* reset=*/ 6);	// Arduboy (Production, Kickstarter Edition)
//U8G2_SSD1306_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_3W_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* cs=*/ 10, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SSD1306_128X64_ALT0_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);   // same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display
//U8G2_SSD1306_128X64_NONAME_1_SW_I2C u8g2(U8G2_R0, /* clock=*/ 16, /* data=*/ 17, /* reset=*/ U8X8_PIN_NONE);   // ESP32 Thing, pure SW emulated I2C
//U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ 16, /* data=*/ 17);   // ESP32 Thing, HW I2C with pin remapping
//U8G2_SSD1306_128X64_NONAME_1_6800 u8g2(U8G2_R0, 13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8G2_SSD1306_128X64_NONAME_1_8080 u8g2(U8G2_R0, 13, 11, 2, 3, 4, 5, 6, A4, /*enable=*/ 7, /*cs=*/ 10, /*dc=*/ 9, /*reset=*/ 8);
//U8G2_SSD1306_128X64_VCOMH0_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);	// same as the NONAME variant, but maximizes setContrast() range
//U8G2_SSD1306_128X64_ALT0_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);	// same as the NONAME variant, but may solve the "every 2nd line skipped" problem
//U8G2_SH1106_128X64_NONAME_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ D8, /* dc=*/ D2, /* reset=*/ D1);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);
//U8G2_SH1106_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);
//U8G2_SH1106_128X64_VCOMH0_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);		// same as the NONAME variant, but maximizes setContrast() range
//U8G2_SH1106_128X64_WINSTAR_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);		// same as the NONAME variant, but uses updated SH1106 init sequence
//U8G2_SH1106_72X40_WISE_1_4W_HW_SPI u8g2(U8G2_R0, /* cs=*/ 10, /* dc=*/ 9, /* reset=*/ 8);


// End of constructor list




#define SUN	0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4

void drawWeatherSymbol(u8g2_uint_t x, u8g2_uint_t y, uint8_t symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic
  
  switch(symbol)
  {
    case SUN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 69);	
      break;
    case SUN_CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 65);	
      break;
    case CLOUD:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 64);	
      break;
    case RAIN:
      u8g2.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8g2.drawGlyph(x, y, 67);	
      break;
    case THUNDER:
      u8g2.setFont(u8g2_font_open_iconic_embedded_6x_t);
      u8g2.drawGlyph(x, y, 67);
      break;      
  }
}

void drawWeather(uint8_t symbol, int degree)
{
  drawWeatherSymbol(0, 48, symbol);
  u8g2.setFont(u8g2_font_logisoso20_tf);
  u8g2.setCursor(48+3, 42);
  u8g2.print(degree);
  u8g2.print("°C");		// requires enableUTF8Print()
}

/*
  Draw a string with specified pixel offset. 
  The offset can be negative.
  Limitation: The monochrome font with 8 pixel per glyph
*/
void drawScrollString(int16_t offset, const char *s)
{
  static char buf[36];	// should for screen with up to 256 pixel width 
  size_t len;
  size_t char_offset = 0;
  u8g2_uint_t dx = 0;
  size_t visible = 0;
  len = strlen(s);
  if ( offset < 0 )
  {
    char_offset = (-offset)/8;
    dx = offset + char_offset*8;
    if ( char_offset >= u8g2.getDisplayWidth()/8 )
      return;
    visible = u8g2.getDisplayWidth()/8-char_offset+1;
    strncpy(buf, s, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(char_offset*8-dx, 62, buf);
  }
  else
  {
    char_offset = offset / 8;
    if ( char_offset >= len )
      return;	// nothing visible
    dx = offset - char_offset*8;
    visible = len - char_offset;
    if ( visible > u8g2.getDisplayWidth()/8+1 )
      visible = u8g2.getDisplayWidth()/8+1;
    strncpy(buf, s+char_offset, visible);
    buf[visible] = '\0';
    u8g2.setFont(u8g2_font_8x13_mf);
    u8g2.drawStr(-dx, 62, buf);
  }
  
}

void draw(const char *s, uint8_t symbol, int degree)
{
  int16_t offset = -(int16_t)u8g2.getDisplayWidth();
  int16_t len = strlen(s);
  for(;;)
  {
    u8g2.firstPage();
    do {
      drawWeather(symbol, degree);
      drawScrollString(offset, s);
    } while ( u8g2.nextPage() );
    delay(20);
    offset+=2;
    if ( offset > len*8+1 )
      break;
  }
}

Bounce debouncerUp = Bounce();  
Bounce debouncerDown = Bounce();
Bounce debouncerMode = Bounce();

int buttonPinUp = 2;
int buttonPinDown = 0;
int buttonPinMode = 12;
int buttonPinEsc = 13;
int debaunceTime = 20;

int packet;
int voltage;


void setup(void) 
{
  u8g2.begin();  
  u8g2.enableUTF8Print();
  pinMode(buttonPinMode, INPUT_PULLUP);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
  debouncerUp.attach(buttonPinUp);
  debouncerDown.attach(buttonPinDown);
  debouncerMode.attach(buttonPinMode);
  debouncerUp.interval(debaunceTime);
  debouncerDown.interval(debaunceTime);
  debouncerMode.interval(debaunceTime);

  

  updateDisplay();
}

unsigned long lastMillis, startMillis, buttonMillis, currentMillis, lastUpdateDisplay;
int degree = 0;
int lastDegree = 9;
float actTemp = 22.75;
float setTemp = 22.0;
bool lastButtonUp, lastButtonDown, lastButtonMode;
bool stateButtonUp, stateButtonDown, stateButtonMode;
bool displayOn=0;

void updateDisplay()
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.drawStr(0,8,"12:24");
 
    u8g2.setCursor(100, 8);
    u8g2.print("86%");

    u8g2.setCursor(80, 63);
    u8g2.print("1014hPa");  
    
    u8g2.setCursor(0, 20);
    u8g2.print("T");
    u8g2.setFont(u8g2_font_u8glib_4_tf);
    u8g2.print("SET ");
    
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.setCursor(0, 44);
    u8g2.print(setTemp, 1); 

    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.setCursor(44, 8);
    u8g2.print(lastMillis);
    u8g2.print(" ms");    // requires enableUTF8Print()
            
    u8g2.drawHLine(0, 10, 130);
    u8g2.drawHLine(0, 52, 130);
    u8g2.setFont(u8g2_font_logisoso24_tf);
    u8g2.setCursor(66, 44);
    u8g2.print(actTemp, 1);
    //u8g2.print("°C");    // requires enableUTF8Print()
    
    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.setCursor(1, 62);

//    u8g2.setFontMode(1);    // 0=solid, 1=transparent
    if(actTemp>setTemp)
    {
      u8g2.drawBox(28,53,27,11);
      u8g2.setFontMode(1);
      u8g2.setDrawColor(2);
      u8g2.print("Heat Cool");
    }
    else
    {
      u8g2.drawBox(0,53,27,11);
      u8g2.setFontMode(1);
      u8g2.setDrawColor(2);
      u8g2.print("Heat Cool");      
    }
    
    degree++;
  } while ( u8g2.nextPage() );

}

void updateSetpoint()
{
  u8g2.firstPage();
  do
  {
    u8g2.setFont(u8g2_font_logisoso50_tf);
    u8g2.setCursor(7, 57);
    u8g2.print(setTemp, 1); 
  } while ( u8g2.nextPage() );
    //PJON
  voltage = (int)setTemp * 10;
  char content[3] = {'V', voltage >> 8, voltage & 0xFF};
}

void loop(void) 
{
  if (debouncerUp.update()) 
  {
    // Get the update value.
    stateButtonUp = debouncerUp.read();
    // Send in the new value.
    if(stateButtonUp == LOW)
    {
      buttonMillis=millis();
      if(displayOn)
      {
        setTemp += 0.5;
        updateSetpoint();
      }
    }
  }
  if (debouncerDown.update()) 
  {
    // Get the update value.
    stateButtonDown = debouncerDown.read();
    // Send in the new value.
    if(stateButtonDown == LOW)
    {
      buttonMillis=millis();
      if(displayOn)
      {
        setTemp -= 0.5;
        updateSetpoint();
      }
    }
  }
  if (debouncerMode.update()) 
  {
    // Get the update value.
    stateButtonMode = debouncerMode.read();
    // Send in the new value.
    if(stateButtonMode == LOW)
    {
      buttonMillis=millis();
    }
  }
  
  startMillis = millis();
  degree = 0;

  
  lastDegree = degree;
  lastMillis = millis() - startMillis;

  if(millis() - buttonMillis > 10000)
  {
    if(displayOn)
    {
      u8g2.setPowerSave(1);
      displayOn = 0;
    }
  }
  else
  {
    if(!displayOn)
    {
      u8g2.setPowerSave(0);
      displayOn = 1;
    }
  }
  if(millis() - buttonMillis > 1000 && millis() - lastUpdateDisplay > 1000 && displayOn)
  {
    updateDisplay();
    lastUpdateDisplay = millis();
  }

}

