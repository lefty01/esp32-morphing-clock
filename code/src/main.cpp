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

//#define CONFIG_FREERTOS_NUMBER_OF_CORES 1
esp_task_wdt_config_t twdt_config = {
    .timeout_ms = WDT_TIMEOUT,
    .idle_core_mask = (1 << CONFIG_FREERTOS_NUMBER_OF_CORES) - 1,    // Bitmask of all cores
    .trigger_panic = true,
};

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

  logStatusMessage("Setting up watchdog...");

  esp_task_wdt_deinit(); //wdt is enabled by default, so we need to deinit it first
  esp_task_wdt_init(&twdt_config); //enable panic so ESP32 restarts
  esp_task_wdt_add(NULL); //add current thread to WDT watch
  // deprecated: esp_task_wdt_init(WDT_TIMEOUT, true);
  // deprecated: esp_task_wdt_add(NULL);
  logStatusMessage("Woof!");

  //logStatusMessage(WiFi.localIP().toString());
  logStatusMessage("Getting weather...");
#ifndef WEATHER_API_PROVIDER
  #error "***** ATTENTION NO WEATHER_API_PROVIDER DEFINED ******"
#elif WEATHER_API_PROVIDER == OPENWEATHERMAP
  getOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &my_weather);
  displayWeatherData(my_weather);
#elif WEATHER_API_PROVIDER == ACCUWEATHER
  getAccuWeatherData();
#else
  #error "Unexpected value for WEATHER_API_PROVIDER defined!"
#endif
  logStatusMessage("Weather recvd!");
  lastWeatherUpdate = millis();

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
    getOpenWeatherData(WEATHER_API_CITY_ID, WEATHER_API_UNITS, WEATHER_API_TOKEN, &my_weather);
    clearForecast();
    clearSensorData();

    draw5DayForecastIcons(my_weather.forecasts, 5);
    displayWeatherData(my_weather);
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
      draw5DayForecastIcons(my_weather.forecasts, 5);
    }
  }

  // Do we have new sensor data?
  if (newSensorData) {
    //logStatusMessage("Sensor data in");
    //displaySensorData(); FIXME position
    //displayTodaysWeather();
  }

  // Is the sensor data too old?
  if ((millis() - lastSensorRead) > (1000 * SENSOR_DEAD_INTERVAL_SEC)) { // TODO: via ticker!?
    sensorDead = true;
    //displaySensorData();FIXME position
  }


  heartBeat = !heartBeat;
  //drawHeartBeat(); // TODO: config option heartbeat

  if ((millis() - lastLightRead) > (1000 * LIGHT_READ_INTERVAL_SEC)) {
    lightUpdate();
    lastLightRead = millis();
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
