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
#include <esp_task_wdt.h>

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
unsigned long lastWeatherUpdate;

//Just a blinking heart to show the main thread is still alive...
bool blinkOn;

void setup(){
  display_init();

  Serial.begin(115200);
  delay(10);

  pinMode(BUTTON1_PIN, INPUT_PULLUP);

  logStatusMessage(PROG_VERSION);
  delay(500);

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

  delay(500);
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
  delay(500);

  logStatusMessage("Init light sensor");
  configureLightSensor();
  delay(500);

  logStatusMessage("Init Temp sensor");
  configureTempSensor();
  delay(500);

  logStatusMessage("Setting up watchdog...");
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);
  logStatusMessage("Woof!");

  //logStatusMessage(WiFi.localIP().toString());
  logStatusMessage("Getting weather...");
#ifndef WEATHER_API_PROVIDER
  #error "***** ATTENTION NO WEATHER_API_PROVIDER DEFINED ******"
#elif WEATHER_API_PROVIDER == OPENWEATHERMAP
  getOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &myWeather);
  displayWeatherData(myWeather);
#elif WEATHER_API_PROVIDER == ACCUWEATHER
  getAccuWeatherData();
#else
  #error "Unexpected value for WEATHER_API_PROVIDER defined!"
#endif
  logStatusMessage("Weather recvd!");
  lastWeatherUpdate = millis();

  // DEBUG ...
  for (int i = 0; i < 5; ++i) {
    Serial.println(myWeather.forecasts[i].time);
    Serial.println(myWeather.forecasts[i].condition);
    Serial.println(myWeather.forecasts[i].temp);
    Serial.println(myWeather.forecasts[i].pressure);
    Serial.println(myWeather.forecasts[i].humidity);
    Serial.println(myWeather.forecasts[i].wind);
  }
  Serial.println(myWeather.timezone);
  Serial.println(myWeather.sunrise);
  Serial.println(myWeather.sunset);

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
  if ((millis() - lastNTPUpdate) > (1000 * NTP_REFRESH_INTERVAL_SEC)) {
    logStatusMessage("NTP Refresh");
    configTime(TIMEZONE_DELTA_SEC, TIMEZONE_DST_SEC, NTP_SERVER);
    lastNTPUpdate = millis();
  }

  // Periodically refresh weather forecast
  if ((millis() - lastWeatherUpdate) > (1000 * WEATHER_REFRESH_INTERVAL_SEC)) {
    logStatusMessage("Weather refresh");

    // FIXME: more generic approach to support different weather providers
    // getWeatherData(void *blob); displayWeatherData();
#ifndef WEATHER_API_PROVIDER
  #error "***** ATTENTION NO WEATHER_API_PROVIDER DEFINED ******"
#elif WEATHER_API_PROVIDER == OPENWEATHERMAP
    getOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &myWeather);
    clearForecast();
    clearSensorData();

    draw5DayForecastIcons(myWeather.forecasts, 5);
    displayWeatherData(myWeather);
#elif WEATHER_API_PROVIDER == ACCUWEATHER
    getAccuWeatherData();
    displayWeatherData();
#else
  #error "Unexpected value for WEATHER_API_PROVIDER defined!"
#endif
    lastWeatherUpdate = millis();
  }

  if (digitalRead(BUTTON1_PIN) == LOW) {
    logStatusMessage("Yess... push it again!!");
  }

  // Do we need to clear the status message from the screen?
  if (logMessageActive) {
    if (millis() > (messageDisplayMillis + LOG_MESSAGE_PERSISTENCE_MSEC)) {
      clearStatusMessage();
      //drawTestBitmap();
      clearForecast();
      draw5DayForecastIcons(myWeather.forecasts, 5);
    }
  }

  // Do we have new sensor data? .. actually right now I think I do not want to update on every new data coming in
  // since this might be to fast? ... or what if we want to display a value that is updated only once a day?
  // oh but what if we want to display async events ... maybe those log to the message box??
  if ((millis() - lastMqttSensorShow) > (1000 * MQTT_SENSOR_DISPLAY_INTERVALL_SEC)) {
    //logStatusMessage("Sensor data in");
    displaySensorData();
    lastMqttSensorShow = millis();
  }


  for (size_t i = 0; i < mySensors.size(); ++i) {
    // Is the sensor data too old (still alive)?
    if ((millis() - mySensors[i].lastRead) > (1000 * SENSOR_DEAD_INTERVAL_SEC)) {
      mySensors[i].isDead = true;
    }

    // new data check ... (if sensor is not updating periodically?)
    if (mySensors[i].newData) {
      // display ...
      mySensors[i].newData = false;
    }
  }


  heartBeat = !heartBeat;
  //drawHeartBeat(); // TODO: config option heartbeat

  if ((millis() - lastI2cSensorRead) > (1000 * I2C_READ_INTERVAL_SEC)) {
    lightUpdate();
    i2cSensorUpdate();
    lastI2cSensorRead = millis();
  }


  //Reset the watchdog timer as long as the main task is running
  esp_task_wdt_reset();
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

void i2cSensorUpdate() {
  float lightData = getLightData();
  float tempdata = getTempData();
  float pressuredata = getPressureData();

  Serial.print("temp:     "); Serial.println(tempdata);
  Serial.print("pressure: "); Serial.println(pressuredata);

}

void lightUpdate() {
  float lightData = getLightData();

  if ((lightData >= 0) && (lightData <= LIGHT_THRESHOLD)) {
    displayLightData(lightData);
  }
}



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
