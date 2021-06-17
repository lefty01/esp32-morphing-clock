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
#define MQTT_STATUS_TOPIC             MQTT_CLIENT_ID "/state"
#define MQTT_GENERAL_CMD_TOPIC        MQTT_CLIENT_ID "/cmd"
#define MQTT_UPDATE_CMD_TOPIC         MQTT_CLIENT_ID "/cmd/update"
#define MQTT_SEND_MESSAGE_TOPIC       MQTT_CLIENT_ID "/message/send"

// How often we refresh the time from the NTP server
#define NTP_REFRESH_INTERVAL_SEC 3600

// Timezone difference from GMT, expressed in seconds
#define TIMEZONE_DELTA_SEC 7200
// DST delta to apply
#define TIMEZONE_DST_SEC 0 //3600 FIXME does DST work?! doesnt look like

// How long are informational messages kept on screen
#define LOG_MESSAGE_PERSISTENCE_MSEC 30000

// How long do we consider the sensor data valid before declaring the sensor dead
#define SENSOR_DEAD_INTERVAL_SEC 600

// weather api config
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
// Panel size
#define PANEL_WIDTH   64
#define PANEL_HEIGHT  64
#define MATRIX_WIDTH  PANEL_WIDTH  // used in ESP32-HUB75-MatrixPanel_I2S_DMA.h
#define MATRIX_HEIGHT PANEL_HEIGHT // used in ESP32-HUB75-MatrixPanel_I2S_DMA.h

// Clock
#define CLOCK_X 0
#define CLOCK_Y 24
#define CLOCK_SEGMENT_HEIGHT 6
#define CLOCK_SEGMENT_WIDTH  6
#define CLOCK_SEGMENT_SPACING 4
#define CLOCK_WIDTH  6*(CLOCK_SEGMENT_WIDTH+CLOCK_SEGMENT_SPACING)+4
#define CLOCK_HEIGHT 2*CLOCK_SEGMENT_HEIGHT+3
//color565 == ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)
#define CLOCK_DIGIT_COLOR  ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0xFF >> 3)
//Delay in ms for clock animation - should be below 30ms for a segment size of 8
#define CLOCK_ANIMATION_DELAY_MSEC 20

// Day of week FIXME position
#define DOW_X PANEL_WIDTH / 3 // FIXME: width for panels other than 128
#define DOW_Y 0 //21
#define DOW_COLOR ((0x00 & 0xF8) << 8) | ((0x40 & 0xFC) << 3) | (0xFF >> 3)
// Date
#define DATE_X DOW_X
#define DATE_Y DOW_Y+11
#define DATE_COLOR DOW_COLOR
//Width and height are for both DATE and DOW
#define DATE_WIDTH 40
#define DATE_HEIGHT 18


// Weather sensor data
#define SENSOR_DATA_X 0
#define SENSOR_DATA_Y 0
#define SENSOR_DATA_WIDTH PANEL_WIDTH // FIXME
#define SENSOR_DATA_HEIGHT 6
#define SENSOR_DATA_COLOR ((0x00 & 0xF8) << 8) | ((0x8F & 0xFC) << 3) | (0x00 >> 3)
#define SENSOR_ERROR_DATA_COLOR ((0xFF & 0xF8) << 8) | ((0x00 & 0xFC) << 3) | (0x00 >> 3)

// Light sensor data
#define LIGHT_DATA_X 0
#define LIGHT_DATA_Y 9
#define LIGHT_DATA_WIDTH PANEL_WIDTH // FIXME
#define LIGHT_DATA_HEIGHT 8
#define LIGHT_DATA_COLOR ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0x00 >> 3)

// Log messages at the bottom
#define LOG_MESSAGE_COLOR ((0xFF & 0xF8) << 8) | ((0x00 & 0xFC) << 3) | (0x00 >> 3)

#define BITMAP_X 0
#define BITMAP_Y 44

#define HEARTBEAT_X PANEL_WIDTH - 8
#define HEARTBEAT_Y 0



#endif
