#ifndef WEATHER_H
#define WEATHER_H

#include <Arduino.h>

void drawTestBitmap();
void displayTodaysWeather();
void displayWeatherForecast();
void displayWeatherData();
void displayTodaysTempRange();
void drawHeartBeat();
void drawBitmap(int startx, int starty, int width, int height, uint32_t *bitmap, bool enlarged=false);
void draw5DayForecastIcons(struct forecast_info *forecasts, int num);
void draw5DayForecastValues(struct forecast_info *forecasts, int num);
int  getOpenWeatherData(uint32_t loc_id, const char *units, const char *appid,
			 struct city_info *info);
void getAccuWeatherData();


#endif
