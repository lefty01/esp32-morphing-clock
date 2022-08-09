#ifndef COMMON_H
#define COMMON_H

#include <array>

#include "config.h"
#include "rgb_display.h"
#include "light_sensor.h"
#include "temp_pressure_sensor.h"

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
extern unsigned long lastI2cSensorRead;
extern unsigned long lastLedBlink;
extern unsigned long lastMqttSensorRead;
extern unsigned long lastMqttSensorShow;

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
// fixme: differentiate between external (mqtt) and internal (i2c) sensors
extern bool clockStartingUp;
extern bool newMqttSensorData;
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
  float temp_min;
  float temp_max;
  int pressure;
  int grnd_level;
  int humidity;
  float wind;
};
struct city_info {
  int  timezone;
  long sunrise;
  long sunset;
  struct forecast_info forecasts[5];
};

extern struct city_info myWeather;

//Just a heartbeat for the watchdog...
extern bool heartBeat;

//Light sensor
#ifdef LIGHT_SENSOR_BH1750
extern BH1750 lightSensor;
#elif LIGHT_SENSOR_TSL2591
extern Adafruit_TSL2591 lightSensor;
#endif

// map sensors in the array to some name or Id !?
enum SensorId {
  CO2_HOME1 = 0, // at my desk
  
};

enum SensorType {
  TEMPERATURE = 0, /* degree celcius             */
  HUMIDITY,        /* (rel) percentage           */
  PRESSURE,        /* barometric pressure in Pa. */
  LIGHT,           /* light level in lux         */
  CO2              /* co2 level ppm              */
};

struct MqttSensor
{
  SensorType type;
  unsigned long lastRead;  // millis() timestamp
  bool newData;            // mqtt data received
  bool isDead;             // no new data within SENSOR_DEAD_INTERVAL_SEC

  union {
    int   val_i;
    float val_f;
  };
};
typedef std::array<MqttSensor, NUM_MQTT_SENSORS> MqttSensors;

extern MqttSensors mySensors;

// utility functions
String epoch2String(unsigned long);
String epoch2HHMM(unsigned long);

//Weather data
extern uint8_t forecast5Days[5];
extern int8_t minTempToday;
extern int8_t maxTempToday;

#endif

