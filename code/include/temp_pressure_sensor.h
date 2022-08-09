#ifndef TEMP_PRESSURE_SENSOR_H
#define TEMP_PRESSURE_SENSOR_H

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>


void configureTempSensor();
float getTempData();
float getPressureData();

#endif

