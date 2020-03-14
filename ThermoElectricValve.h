#include <math.h>

class ThermoElectricValve
{ 
  int relayPin;
  bool relayON;
  bool relayOFF;
  uint32_t lastMillis;  
  uint16_t counter = 0;
  uint16_t sum = 0;
  uint16_t samples = 1000;
  float actual = 0;  
  float setpoint = 0;
  
  
  public:
  ThermoElectricValve(int _relayPin, bool invertedRelay)
  {
    relayPin = _relayPin; 
    relayON = !invertedRelay;
    relayOFF = invertedRelay; 
    digitalWrite(relayPin, relayOFF);     // Make sure relay is off when starting up
    pinMode(relayPin, OUTPUT);            // Then set relay pins in output mode            
  }

  void Update(float _setpoint)
  {    
    setpoint = constrain(_setpoint, 0, 100) * samples / 100.0; // set the internal range to 0-1000
    if(millis() - lastMillis >= 20)
    {
      if(setpoint == 0)
      {
        digitalWrite(relayPin, relayOFF);
      }
      else
      {       
        if(setpoint >= actual)
        {
          digitalWrite(relayPin, relayON);
          sum++;
        }
        else
        {
          digitalWrite(relayPin, relayOFF);
        }
      }
      
      actual = float(samples) * sum / (counter + 1);
      
      #ifdef MY_DEBUG_MP
        Serial.print("SP: ");
        Serial.print(setpoint, 0);
        Serial.print("\tACT: ");
        Serial.println(actual, 0);        
      #endif

      
      counter++;
      if(counter>samples-1)
      {
        counter = 0;
        sum = 0;
      }
      lastMillis = millis();
    }

    
    
    
  }         
  

};
