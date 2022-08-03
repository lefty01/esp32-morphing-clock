#include "common.h"
#include "light_sensor.h"


#ifdef LIGHT_SENSOR_BH1750
BH1750 lightSensor = BH1750();
#elif LIGHT_SENSOR_TSL2591
Adafruit_TSL2591 lightSensor = Adafruit_TSL2591(2591);
#else
struct LightDummy {
  bool getEvent(sensors_event_t *event) { return false; }
};
LightDummy lightSensor;
#endif


void configureLightSensor() {
#ifdef LIGHT_SENSOR_BH1750
  Wire.begin();
  //lightSensor.begin(BH1750::ONE_TIME_HIGH_RES_MODE);

  if (lightSensor.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println(F("BH1750 OK"));
  } else {
    Serial.println(F("Error initialising BH1750"));
    logStatusMessage("BH1750 ERROR");
  }
#elif LIGHT_SENSOR_TSL2591
  // You can change the gain on the fly, to adapt to brighter/dimmer light situations
  //lightSensor.setGain(TSL2591_GAIN_LOW);    // 1x gain (bright light)
  lightSensor.setGain(TSL2591_GAIN_MED);      // 25x gain
  //lightSensor.setGain(TSL2591_GAIN_HIGH);   // 428x gain

  // Changing the integration time gives you a longer time over which to sense light
  // longer timelines are slower, but are good in very low light situations!
  //lightSensor.setTiming(TSL2591_INTEGRATIONTIME_100MS);  // shortest integration time (bright light)
  lightSensor.setTiming(TSL2591_INTEGRATIONTIME_200MS);
  //lightSensor.setTiming(TSL2591_INTEGRATIONTIME_300MS);
  //lightSensor.setTiming(TSL2591_INTEGRATIONTIME_400MS);
  //lightSensor.setTiming(TSL2591_INTEGRATIONTIME_500MS);
  //lightSensor.setTiming(TSL2591_INTEGRATIONTIME_600MS);  // longest integration time (dim light)
#endif
}

float getLightData() {
  /* Get a new sensor event */
  sensors_event_t event;
  bool validData = lightSensor.getEvent(&event);

  if (validData) {
    // Serial.print("Light: ");
    // Serial.print(event.light);
    // Serial.println(" lx");
    return event.light;
  }
  return -1;
}
