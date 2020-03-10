#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

class MP_BME280
{
  Adafruit_BME280 bme; // I2C
  int BME280address;
  float temperature = 0.0, humidity = 0.0, pressure = 0.0;
  uint32_t lastReading = 0;  
  uint32_t refreshTime = 0;
  
  public:
  MP_BME280(int _address, uint32_t _refresh)
  {
      Serial.println(F("BME280 test"));
      BME280address = _address;
      refreshTime = _refresh;
  }

  void Initiate()
  {
      bool statusBME280;
  
      statusBME280 = bme.begin(BME280address);  
      if (!statusBME280) {
          Serial.println("Could not find a valid BME280 sensor, check wiring!");
      }
      
      Serial.println("-- Default Test --");
  
      Serial.println();
  }

  void Update()
  {
    if(millis() - lastReading > refreshTime)
    {
      temperature = bme.readTemperature();
      pressure = bme.readPressure() / 100.0F;
      humidity = bme.readHumidity();
      lastReading = millis();
    }
  }

  float getTemperature()
  {
      Update();
      return temperature;
  }

  float getHumidity()
  {
      Update();
      return humidity;
  }

  float getPressure()
  {
      Update();
      return pressure;
  }
  
  void printValues() {
      Serial.print("Temperature = ");
      Serial.print(bme.readTemperature());
      Serial.println(" *C");
  
      Serial.print("Pressure = ");
  
      Serial.print(bme.readPressure() / 100.0F);
      Serial.println(" hPa");
  
      Serial.print("Humidity = ");
      Serial.print(bme.readHumidity());
      Serial.println(" %");
  
      Serial.println();
  }  
};

