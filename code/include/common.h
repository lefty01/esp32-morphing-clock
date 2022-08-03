#ifndef COMMON_H
#define COMMON_H

#include "config.h"
#include "rgb_display.h"
#include "light_sensor.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ESPNtpClient.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

extern const char* PROG_VERSION;

#ifdef MQTT_USE_SSL
extern WiFiClientSecure wifiClient;
#else
extern WiFiClient wifiClient;
#endif

extern int status;

// Initialize MQTT client
extern PubSubClient client;

// http client (access weather api)
extern HTTPClient http;

//Time of last status update
extern unsigned long lastStatusSend;

//Time of last sensor events
extern unsigned long lastSensorRead;
extern unsigned long lastLightRead;
extern unsigned long lastLedBlink;

//Log message persistence
//Is a log message currently displayed?
extern bool logMessageActive;
//When was the message shown?
extern unsigned long messageDisplayMillis;

// NTP
extern bool wifiFirstConnected;

extern bool syncEventTriggered; // True if a time event has been triggered

//RGB display
extern MatrixPanel_I2S_DMA *dma_display;

//Current time and date
extern struct tm timeinfo;

//Flags to trigger display section updates
extern bool clockStartingUp;
extern bool newSensorData;
extern bool sensorDead;

//The actual sensor data
extern float sensorTemp;
extern int sensorHumi;
extern int sensorCo2Mqtt;

// 5-day forecast ids/icons (https://openweathermap.org/weather-conditions)
struct forecast_info {
  long time;
  int condition;
  float temp;
  int pressure;
  int humidity;
  float wind;
};
struct city_info {
  int  timezone;
  long sunrise;
  long sunset;
  struct forecast_info forecasts[5];
};
//extern struct forecast_info forecasts[5];
extern struct city_info my_weather;

//Just a heartbeat for the watchdog...
extern bool heartBeat;

//Light sensor
#ifdef LIGHT_SENSOR_BH1750
extern BH1750 lightSensor;
#elif LIGHT_SENSOR_TSL2591
extern Adafruit_TSL2591 lightSensor;
#endif


// utility functions
String epoch2String(unsigned long);
String epoch2HHMM(unsigned long);

//Weather data
extern uint8_t forecast5Days[5];
extern int8_t minTempToday;
extern int8_t maxTempToday;

#endif

