#include "config.h"
#include "common.h"
#include "mqtt.h"
#include "rgb_display.h"
#include "version.h"

#ifdef MQTT_USE_SSL
WiFiClientSecure wifiClient;
#else
WiFiClient wifiClient;
#endif
int status = WL_IDLE_STATUS;

// Initialize MQTT client
PubSubClient client(wifiClient);

HTTPClient http;

//Time of last status update
unsigned long lastStatusSend = 0;

//Time of last client.loop()
unsigned long lastLoop = 0;
//Time of last weather sensor data receive
unsigned long lastSensorRead = 0;
//Time of last light sensor read
unsigned long lastLightRead = 0;


// NTP
const int8_t timeZone = 2;
const int8_t minutesTimeZone = 0;
bool wifiFirstConnected = false;
//Current time
struct tm timeinfo;

// Display
MatrixPanel_I2S_DMA *dma_display = nullptr;

// Flags to trigger display updates
bool clockStartingUp = true;
bool newSensorData = false;
bool sensorDead = true;

//Heartbeat marker
bool heartBeat = true;

//Log message persistence
//Is a log message currently displayed?
bool logMessageActive = false;
//When was the message shown?
unsigned long messageDisplayMillis = 0;

// Sensor data
float sensorTemp;
int sensorHumi;
int sensorCo2Mqtt;


// 5-day forecast ids/icons (https://openweathermap.org/weather-conditions)
//struct forecast_info forecasts[5];
struct city_info my_weather;

String epoch2String(unsigned long t) {
  char buf[32];
  struct tm * ts;

  ts = localtime((time_t*)&t);
  strftime(buf, sizeof(buf), "%d.%m.%Y %H:%M:%S", ts);

  return String(buf);
}

String epoch2HHMM(unsigned long t) {
  char buf[6];
  struct tm *ts;

  ts = localtime((time_t*)&t);
  strftime(buf, sizeof(buf), "%H:%M", ts);

  return String(buf);
}
