#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

void displaySensorData();
void drawTestBitmap();
void drawHeartBeat();
void drawBitmap(int startx, int starty, int width, int height, uint32_t *bitmap);
void draw5DayForecastIcons(struct forecast_info *forecasts, int num);
void draw5DayForecastValues(struct forecast_info *forecasts, int num);
int fetchOpenWeatherData(uint32_t loc_id, const char *units, const char *appid, struct forecast_info *forecasts);

#endif
