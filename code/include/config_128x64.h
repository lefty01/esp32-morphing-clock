#ifndef CONFIG_128x64_H
#define CONFIG_128x64_H

// Panel size
#define PANEL_WIDTH 128
#define PANEL_HEIGHT 64


// Clock
#define CLOCK_X 3
#define CLOCK_Y 21
#define CLOCK_SEGMENT_HEIGHT 8
#define CLOCK_SEGMENT_WIDTH 8
#define CLOCK_SEGMENT_SPACING 5
#define CLOCK_WIDTH 6*(CLOCK_SEGMENT_WIDTH+CLOCK_SEGMENT_SPACING)+4

#define CLOCK_HEIGHT 2*CLOCK_SEGMENT_HEIGHT+3
#define CLOCK_DIGIT_COLOR  ((0x00 & 0xF8) << 8) | ((0xFF & 0xFC) << 3) | (0xFF >> 3)
//Delay in ms for clock animation - should be below 30ms for a segment size of 8
#define CLOCK_ANIMATION_DELAY_MSEC 20

// Day of week
#define DOW_X 90
#define DOW_Y 21

// Date
#define DATE_X DOW_X
#define DATE_Y DOW_Y+11

// Width and height are for both DATE and DOW
#define DATE_WIDTH  40
#define DATE_HEIGHT 18

// Weather sensor data
#define SENSOR_DATA_X 0
#define SENSOR_DATA_Y 0
#define SENSOR_DATA_WIDTH 128
#define SENSOR_DATA_HEIGHT 8

// Light sensor data
#define LIGHT_DATA_X 0
#define LIGHT_DATA_Y 9
#define LIGHT_DATA_WIDTH 72
#define LIGHT_DATA_HEIGHT 8



#define BITMAP_X 0
#define BITMAP_Y 44


#define HEARTBEAT_X 120
#define HEARTBEAT_Y 21


#endif
