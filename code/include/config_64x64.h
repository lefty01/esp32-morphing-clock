#ifndef CONFIG_64x64_H
#define CONFIG_64x64_H

#define PANEL_WIDTH   64
#define PANEL_HEIGHT  64


#ifndef MATRIX_WIDTH
#define MATRIX_WIDTH PANEL_WIDTH
#endif
#ifndef MATRIX_HEIGHT
#define MATRIX_HEIGHT PANEL_HEIGHT
#endif


// Clock
#define CLOCK_X 0
#define CLOCK_Y 24
#define CLOCK_SEGMENT_HEIGHT 6
#define CLOCK_SEGMENT_WIDTH  6
#define CLOCK_SEGMENT_SPACING 4
#define CLOCK_WIDTH  6*(CLOCK_SEGMENT_WIDTH+CLOCK_SEGMENT_SPACING)+4
#define CLOCK_HEIGHT 2*CLOCK_SEGMENT_HEIGHT+3

// Day of week
#define DOW_X ((PANEL_WIDTH / 2) - 2)
#define DOW_Y 0

// Date
#define DATE_X DOW_X
#define DATE_Y DOW_Y+11

// Width and height are for both DATE and DOW
#define DATE_WIDTH  40
#define DATE_HEIGHT 18


// Weather sensor data (currently temp and humidity)
#define SENSOR_DATA_X 0
#define SENSOR_DATA_Y 0
#define SENSOR_DATA_WIDTH ((PANEL_WIDTH / 2) - 2)
#define SENSOR_DATA_HEIGHT 18

// ... FIXME: adjust to 64x64 if we want to use it (merged from bogdan)
// Weather - today, and 5-day forecast
#define WEATHER_TODAY_X 56
#define WEATHER_TODAY_Y 1

#define WEATHER_FORECAST_X 90
#define WEATHER_FORECAST_Y 44

//Temperature range for today
#define TEMPRANGE_X 0
#define TEMPRANGE_Y 44
#define TEMPRANGE_WIDTH 64
#define TEMPRANGE_HEIGHT 8


// Light sensor data
#define LIGHT_DATA_X 0
#define LIGHT_DATA_Y 16
#define LIGHT_DATA_WIDTH  ((PANEL_WIDTH / 2) - 2)
#define LIGHT_DATA_HEIGHT 8


#define BITMAP_X 0
#define BITMAP_Y 44


#define HEARTBEAT_X PANEL_WIDTH - 8
#define HEARTBEAT_Y 0


#endif
