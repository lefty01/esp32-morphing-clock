#ifndef CONFIG_H
#define CONFIG_H

//#define MQTT_USE_SSL 1
//#define USE_ANDROID_AP 1

// some MQTT configs
#define MQTT_HOSTNAME "esp32-morphclock"
#define MQTT_CLIENT_ID MQTT_HOSTNAME
#define MQTT_REPORT_INTERVAL_MILLIS 30000

// MQTT Topics
#define MQTT_TEMPERATURE_SENSOR_TOPIC MQTT_CLIENT_ID "/sensor/temperature"
#define MQTT_HUMIDITY_SENSOR_TOPIC    MQTT_CLIENT_ID "/sensor/humidity"
#define MQTT_CO2_SENSOR_TOPIC         MQTT_CLIENT_ID "/co2_ampel1/co2ppm"
#define MQTT_STATUS_TOPIC             MQTT_CLIENT_ID "/state"
#define MQTT_GENERAL_CMD_TOPIC        MQTT_CLIENT_ID "/cmd"
#define MQTT_UPDATE_CMD_TOPIC         MQTT_CLIENT_ID "/cmd/update"
//#define MQTT_VERSION_CMD_TOPIC        MQTT_CLIENT_ID "/cmd/version"
#define MQTT_SEND_MESSAGE_TOPIC       MQTT_CLIENT_ID "/message/send"
#define MQTT_BUZZER_CONFIG_TOPIC      MQTT_CLIENT_ID "/buzzer/config"

// How often we refresh the time from the NTP server
#define NTP_REFRESH_INTERVAL_SEC 3600
#define NTP_SERVER "europe.pool.ntp.org"
// https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
#define NTP_MY_TIMEZONE "CET-1CEST,M3.5.0,M10.5.0/3"

// Timezone difference from GMT, expressed in seconds
#define TIMEZONE_DELTA_SEC 7200
// DST delta to apply
#define TIMEZONE_DST_SEC 0 //3600 FIXME does DST work?! doesnt look like

// How long are informational messages kept on screen
#define LOG_MESSAGE_PERSISTENCE_MSEC 30000

// How long do we consider the sensor data valid before declaring the sensor dead
#define SENSOR_DEAD_INTERVAL_SEC 600

// weather api config
#define WEATHER_API_PROVIDER OPENWEATHERMAP
#define WEATHER_API_CITY_ID  2947444
#define WEATHER_API_UNITS    "metric"
#define WEATHER_API_BASE_URL "http://api.openweathermap.org"

//Button pin
#define BUTTON1_PIN 0

//Buzzer pin
#define BUZZER_PIN 2
#define BUZZER_PWM_CHANNEL 0
#define BUZZER_PWM_RESOLUTION 8

// Screen positioning settings
// define Panel type, selects panel size
//#define PANEL_SIZE_64_x_64 -> platformio.ini
#if defined PANEL_SIZE_64_x_64
#include "config_64x64.h"
#elif defined PANEL_SIZE_128_x_64
#include "config_128x64.h"
#else
#error ***** ATTENTION NO PANEL SIZE DEFINED ******
#endif


#define CLOCK_DIGIT_COLOR  ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0xFF >> 3)

// Delay in ms for clock animation - should be below 30ms for a segment size of 8
#define CLOCK_ANIMATION_DELAY_MSEC 20

// Day of week color
#define DOW_COLOR ((0x00 & 0xF8) << 8) | ((0x40 & 0xFC) << 3) | (0xFF >> 3)

// Date color
#define DATE_COLOR DOW_COLOR


// Weather sensor data
#define SENSOR_DATA_COLOR ((0x00 & 0xF8) << 8) | ((0x8F & 0xFC) << 3) | (0x00 >> 3)
#define SENSOR_ERROR_DATA_COLOR ((0xFF & 0xF8) << 8) | ((0x00 & 0xFC) << 3) | (0x00 >> 3)

#define LIGHT_DATA_COLOR ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0x00 >> 3)
//Maximum lux value that will be accepted as valid (sometimes the sensor will return erroneous values)
#define LIGHT_THRESHOLD 9999
#define LIGHT_READ_INTERVAL_SEC 5

// Log messages at the bottom
#define LOG_MESSAGE_COLOR ((0xFF & 0xF8) << 8) | ((0x00 & 0xFC) << 3) | (0x00 >> 3)

// Watchdog settings
#define WDT_TIMEOUT 300   // If the WDT is not reset within X seconds, reboot the unit
                          // Do NOT set this too low, or the WDT will prevent OTA updates from completing!!

#define TEMPRANGE_COLOR ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0xFF >> 3)

// How often to refresh weather forecast data
// (limited by API throttling)
#define WEATHER_REFRESH_INTERVAL_SEC 900


#endif
