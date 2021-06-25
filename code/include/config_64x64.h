#ifndef CONFIG_64x64_H
#define CONFIG_64x64_H

#define PANEL_WIDTH   64
#define PANEL_HEIGHT  64


// Clock
#define CLOCK_X 0
#define CLOCK_Y 24
#define CLOCK_SEGMENT_HEIGHT 6
#define CLOCK_SEGMENT_WIDTH  6
#define CLOCK_SEGMENT_SPACING 4
#define CLOCK_WIDTH  6*(CLOCK_SEGMENT_WIDTH+CLOCK_SEGMENT_SPACING)+4
#define CLOCK_HEIGHT 2*CLOCK_SEGMENT_HEIGHT+3

// Day of week
#define DOW_X PANEL_WIDTH / 3
#define DOW_Y 0

// Date
#define DATE_X DOW_X
#define DATE_Y DOW_Y+11

// Width and height are for both DATE and DOW
#define DATE_WIDTH  40
#define DATE_HEIGHT 18


// Weather sensor data
#define SENSOR_DATA_X 0
#define SENSOR_DATA_Y 0
#define SENSOR_DATA_WIDTH PANEL_WIDTH // FIXME
#define SENSOR_DATA_HEIGHT 6

// Light sensor data
#define LIGHT_DATA_X 0
#define LIGHT_DATA_Y 9
#define LIGHT_DATA_WIDTH PANEL_WIDTH // FIXME
#define LIGHT_DATA_HEIGHT 8



#define BITMAP_X 0
#define BITMAP_Y 44


#define HEARTBEAT_X PANEL_WIDTH - 8
#define HEARTBEAT_Y 0


#endif
