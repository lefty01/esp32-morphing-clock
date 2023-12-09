#include "common.h"
#include "temp_pressure_sensor.h"

#define MY_BMP280_ADDRESS (0x76)
#define MY_BMP280_CHIPID (0x60)
#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP280 bmp; // use I2C interface
Adafruit_Sensor *bmp_temp = bmp.getTemperatureSensor();
Adafruit_Sensor *bmp_pressure = bmp.getPressureSensor();




void configureTempSensor() {

  bool status = bmp.begin(MY_BMP280_ADDRESS, MY_BMP280_CHIPID);

  if (!status) {
    Serial.println(F("Error initialising BMP280"));
    Serial.println(status);
    Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
    logStatusMessage("BMP280 ERROR");
    return;
  }
  Serial.println(F("BMP280 OK"));
  logStatusMessage("BMP280 OK");

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
		  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
		  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
		  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
		  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  //bmp_temp->printSensorDetails();

}

float getTempData() {
  sensors_event_t temp_event;

  bmp_temp->getEvent(&temp_event);
  // Serial.print(F("Temperature = "));
  // Serial.print(temp_event.temperature);
  // Serial.println(" *C");

  return temp_event.temperature;
}

float getPressureData() {
  sensors_event_t pressure_event;

  bmp_pressure->getEvent(&pressure_event);
  // Serial.print(F("Pressure = "));
  // Serial.print(pressure_event.pressure);
  // Serial.println(" hPa");

  return pressure_event.pressure;
}
