/*
Version:
- 1.00 - starting point, old code with SH1106
- 1.01 - code was cleaned, 4th button added - PB Minus, PB Plus, PB Enter, PB Escape
- 1.02 - split the code, add BME280
- 1.03 - add PID controller and ThermoElectricalValve output

*/
#define MP_DEBUG // comment if you don't need the debug

#include <Arduino.h>
#include <U8g2lib.h>
#include <PID_v1.h>
#include <Bounce2.h>
#include "BME280.h"
#include "ThermoElectricValve.h"
#include "MP_Polygon.h"

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#define SDA 4
#define SCL 14

MP_BME280 tempSensor(0x76, 100); //i2c address; minimal sample time

// The complete list is available here: https://github.com/olikraus/u8g2/wiki/u8g2setupcpp
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R2, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);


Bounce debouncerUp = Bounce();  
Bounce debouncerDown = Bounce();
Bounce debouncerEnter = Bounce();

int buttonPinUp = 2;
int buttonPinDown = 0;
int buttonPinEnter = 12;
int buttonPinEsc = 13;
int debaunceTime = 20;

double SP = 22.0, PV = 22.0, MV = 0.0; // PID
//Define the aggressive and conservative Tuning Parameters
double aggKp=4, aggKi=0.2, aggKd=1;
double consKp=1, consKi=0.05, consKd=0.25;

PID myPID(&PV, &MV, &SP, consKp, consKi, consKd, DIRECT);
//ThermoElectricValve(float _setpoint, int _relayPin, bool invertedRelay)
//ThermoElectricValve valve(15, 0); // SSR PIN
ThermoElectricValve valve(16, 1); // ONBOARD LED FOR TEST

double scaledMV = MV;

int packet;
int voltage;

unsigned long lastMillis, startMillis, buttonMillis, currentMillis, lastUpdateDisplay;
float actTemp = 22.75;
float lastTemp = actTemp;
float setTemp = 22.0;
uint8_t actHum = 50;
uint16_t actPressure = 1014;
bool lastButtonUp, lastButtonDown, lastButtonEnter;
bool stateButtonUp, stateButtonDown, stateButtonEnter;
bool displayOn=0;

void setup(void) 
{
  Serial.begin(9600);
  Wire.begin(SDA, SCL);
  u8g2.begin();  
  u8g2.enableUTF8Print();
  tempSensor.Initiate(); 
  pinMode(buttonPinEnter, INPUT_PULLUP);
  pinMode(buttonPinUp, INPUT_PULLUP);
  pinMode(buttonPinDown, INPUT_PULLUP);
  debouncerUp.attach(buttonPinUp);
  debouncerDown.attach(buttonPinDown);
  debouncerEnter.attach(buttonPinEnter);
  debouncerUp.interval(debaunceTime);
  debouncerDown.interval(debaunceTime);
  debouncerEnter.interval(debaunceTime);
  updateDisplay();
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 100);
}

void updateDisplay()
{
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.drawStr(0,8,"12:24");
 
    u8g2.setCursor(100, 8);
    u8g2.print(actHum);
    u8g2.print(" %");

    u8g2.setCursor(80, 63);
    u8g2.print(actPressure);
    u8g2.print(" hPa");  
    
    u8g2.setCursor(0, 20);
    u8g2.print("T");
    u8g2.setFont(u8g2_font_u8glib_4_tf);
    u8g2.print("SET ");
    
    u8g2.setFont(u8g2_font_logisoso16_tf);
    u8g2.setCursor(0, 44);
    u8g2.print(setTemp, 1); 

    u8g2.setFont(u8g2_font_t0_11_tf);
    u8g2.setCursor(44, 8);
    //u8g2.print(lastMillis);
    u8g2.print(MV);
    u8g2.print("/");
    u8g2.print(scaledMV);
//    u8g2.print(" ms");    // requires enableUTF8Print()
            
    u8g2.drawHLine(0, 10, 130);
    u8g2.drawHLine(0, 50, (int)scaledMV*128/100); //Progress bar - valve output
    u8g2.drawHLine(0, 51, (int)MV*128/100); //Progress bar - valve output    
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
}

void loop(void) 
{
  // GET DATA FROM SENSOR
  lastTemp = actTemp;
  actTemp = tempSensor.getTemperature();
  actHum = (int)tempSensor.getHumidity();
  actPressure = (int)tempSensor.getPressure();

  if(actTemp != actTemp) //NaN
  {
    actTemp = lastTemp;
  }
  
  // PID BEBIN ******************************************************
  PV = actTemp;
  SP = setTemp;

  double gap = abs(SP-PV); //distance away from setpoint
  if (gap < 0.25)
  {  //we're close to setpoint, use conservative tuning parameters
    myPID.SetTunings(consKp, consKi, consKd);
  }
  else
  {
     //we're far from setpoint, use aggressive tuning parameters
     myPID.SetTunings(aggKp, aggKi, aggKd);
  }

  myPID.Compute();
  scaledMV = MP_Polygon(MV, 0, 5, 95, 100, 0, 2, 8, 100);
  valve.Update(scaledMV);
  // END PID  ********************************************************
  
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
  if (debouncerEnter.update()) 
  {
    // Get the update value.
    stateButtonEnter = debouncerEnter.read();
    // Send in the new value.
    if(stateButtonEnter == LOW)
    {
      buttonMillis=millis();
    }
  }
  
  startMillis = millis();
  lastMillis = millis() - startMillis;

  if(millis() - buttonMillis > 60000)
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

