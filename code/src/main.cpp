/*
ESP32 Matrix Clock - Copyright (C) 2021 Bogdan Sass

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/


#include <Ticker.h>

#include "config.h"
#include "main.h"
#include "common.h"
#include "rgb_display.h"
#include "mqtt.h"
#include "creds_mqtt.h"
#include "clock.h"
#include "weather.h"
#include "buzzer.h"
#include "time.h"

#define EVERY_SECOND     1000
#define EVERY_MINUTE     EVERY_SECOND * 60
#define EVERY_10_MINUTES EVERY_MINUTE * 10

Ticker displayTicker;
//Ticker sensorDataTicker;
//Ticker weatherUpdateTicker;
unsigned long prevEpoch;
unsigned long lastNTPUpdate;
unsigned long sw_timer_10min;


//Just a blinking minion...
bool blinkOn;

/* void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  Serial.print("Day of week: ");
  Serial.println(&timeinfo, "%A");
  Serial.print("Month: ");
  Serial.println(&timeinfo, "%B");
  Serial.print("Day of Month: ");
  Serial.println(&timeinfo, "%d");
  Serial.print("Year: ");
  Serial.println(&timeinfo, "%Y");
  Serial.print("Hour: ");
  Serial.println(&timeinfo, "%H");
  Serial.print("Hour (12 hour format): ");
  Serial.println(&timeinfo, "%I");
  Serial.print("Minute: ");
  Serial.println(&timeinfo, "%M");
  Serial.print("Second: ");
  Serial.println(&timeinfo, "%S");

  Serial.println("Time variables");
  char timeHour[3];
  strftime(timeHour,3, "%H", &timeinfo);
  Serial.println(timeHour);
  char timeWeekDay[10];
  strftime(timeWeekDay,10, "%A", &timeinfo);
  Serial.println(timeWeekDay);
  Serial.println();
} */

void setup(){
  display_init();

  Serial.begin(115200);
  delay(10);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);

  logStatusMessage("Buzzer setup");
  buzzer_init();
  //buzzer_tone(500, 300);
  displayTest(300);

  logStatusMessage("Connecting to WiFi...");
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  logStatusMessage("WiFi connected!");

  logStatusMessage("NTP time...");
  configTime(TIMEZONE_DELTA_SEC, TIMEZONE_DST_SEC, NTP_SERVER);
  lastNTPUpdate = millis();
  logStatusMessage("NTP done!");

  logStatusMessage("MQTT connect...");
  #ifdef MQTT_USE_SSL
  wifiClient.setCACert(server_crt_str);
  wifiClient.setCertificate(client_crt_str);
  wifiClient.setPrivateKey(client_key_str);
  #endif
  client.setServer( MQTT_SERVER, MQTT_PORT );
  client.setCallback(mqtt_callback);
  reconnect();
  lastStatusSend = 0;
  logStatusMessage("MQTT done!");

  logStatusMessage("Initialize TSL...");
  tslConfigureSensor();
  logStatusMessage("TSL done!");

  //logStatusMessage(WiFi.localIP().toString());
  fetchOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &my_weather);
  displayWeatherData(my_weather);

  // DEBUG ...
  for (int i = 0; i < 5; ++i) {
    Serial.println(my_weather.forecasts[i].time);
    Serial.println(my_weather.forecasts[i].condition);
    Serial.println(my_weather.forecasts[i].temp);
    Serial.println(my_weather.forecasts[i].pressure);
    Serial.println(my_weather.forecasts[i].humidity);
    Serial.println(my_weather.forecasts[i].wind);
  }
  Serial.println(my_weather.timezone);
  Serial.println(my_weather.sunrise);
  Serial.println(my_weather.sunset);
  //draw5DayForecast(forecasts, 5);
  drawTestBitmap();

  displayTicker.attach_ms(30, displayUpdater);
  //sensorDataTicker.attach_ms(60 * 1000, sensorUpdater);

  buzzer_tone(1000, 300);
}

uint8_t wheelval = 0;
void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    logStatusMessage("WiFi lost!");
    WiFi.reconnect();
  }

  if (!client.connected()) {
    logStatusMessage("MQTT lost");
    reconnect();
  }
  client.loop();

  // Periodically refresh NTP time
  if (millis() - lastNTPUpdate > (1000 * NTP_REFRESH_INTERVAL_SEC)) {
    logStatusMessage("NTP Refresh");
    configTime(TIMEZONE_DELTA_SEC, TIMEZONE_DST_SEC, NTP_SERVER);
    lastNTPUpdate = millis();
  }

  // 10 minute timer, get weather update
  if ((millis() - sw_timer_10min) > EVERY_10_MINUTES) {
    sw_timer_10min = millis();
    fetchOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &my_weather);
    clearForecast();
    clearSensorData();

    draw5DayForecastIcons(my_weather.forecasts, 5);
    displayWeatherData(my_weather);
  }

  if (digitalRead(BUTTON1_PIN) == LOW) {
    logStatusMessage("Yess... push it again!!");
  }

  //Do we need to clear the status message from the screen?
  if (logMessageActive) {
    if (millis() > messageDisplayMillis + LOG_MESSAGE_PERSISTENCE_MSEC) {
      clearStatusMessage();
      //drawTestBitmap();
      clearForecast();
      draw5DayForecastIcons(my_weather.forecasts, 5);
    }
  }

  // Do we have new sensor data?
  if (newSensorData) {
    //logStatusMessage("Sensor data in");
    //displaySensorData(); FIXME position
  }

  // Is the sensor data too old?
  if (millis() - lastSensorRead > 1000*SENSOR_DEAD_INTERVAL_SEC) { // TODO: via ticker!?
    sensorDead = true;
    //displaySensorData();FIXME position
  }

  float tslData = tslGetLux();
  //displayLightData(tslData); FIXME position

  heartBeat = !heartBeat;
  //drawHeartBeat(); // TODO: config option heartbeat

  delay(500);
}

void displayUpdater() {
  if(!getLocalTime(&timeinfo)){
    logStatusMessage("Failed to get time!");
    return;
  }

  unsigned long epoch = mktime(&timeinfo);
  if (epoch != prevEpoch) {
    displayClock();
    prevEpoch = epoch;
  }
}

//void sensorUpdater() {
//}

//TODO: http://www.rinkydinkelectronics.com/t_imageconverter565.php

//TODO - add heartbeat in loop(), reboot in interrupt if heartbeat lost (sometimes the system hangs during OTA request)
//https://iotassistant.io/esp32/enable-hardware-watchdog-timer-esp32-arduino-ide/

//TODO - get and print weather forecast every X interval (4h?)
//     -> https://github.com/lefty01/esp32-morphing-clock/tree/openweather-api
//TODO - use light sensor data to set display brightness
//TODO - add option to turn off display via MQTT
//TODO - replace bitmap arrays with color565 values!
//TODO - add event-based wifi disconnect/reconnect - https://randomnerdtutorials.com/solved-reconnect-esp32-to-wifi/

//TODO - check asynchronously for buzzer stop
//TODO - move TSL read to async task
