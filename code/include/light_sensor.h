#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#ifdef LIGHT_SENSOR_BH1750
#include <BH1750.h>
#elif LIGHT_SENSOR_TSL2591
#include <Adafruit_TSL2591.h>
#else
#include <Adafruit_Sensor.h>
#endif

void configureLightSensor();
float getLightData();

#endif
