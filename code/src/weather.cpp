#include "weather.h"
#include "common.h"
#include "creds_accuweather.h"

#include <HTTPClient.h>
#include <ArduinoJson.h>
//#include <Fonts/FreeSerifBold12pt7b.h>

uint8_t forecast5Days[5] = {0,0,0,0,0};
int8_t minTempToday = 0;
int8_t maxTempToday = 0;


//Source: http://www.newdesignfile.com/post_pixelated-graphic-arts_325919/


/* Python code to convert 8x8 icons:
test= [ 0x78, 0x84, 0x84, 0x84, 0x64, 0x44, 0x44, 0x38]  // Column-based, from a SURE 16x32 matrix

def convert(test, width, height, color):
   for y in range(height):
     for x in range(width):
       cr_bit = (test[x] >> (height-y-1)) & 0x01
       if (cr_bit==1):
         print("{:s}, ".format(color), end='')
       else:
         print("0x000000, ", end='')
     print()
*/

uint32_t static cloud_8x8[] = {
  0x000000, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x000000, 0x000000, 0x000000, 0x000000,
  0x00FFFF, 0x000000, 0x000000, 0x000000, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x000000,
  0x00FFFF, 0x000000, 0x000000, 0x000000, 0x00FFFF, 0x000000, 0x000000, 0x00FFFF,
  0x00FFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00FFFF,
  0x00FFFF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x00FFFF,
  0x000000, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x00FFFF, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
};

uint32_t static sun_8x8[] = {
  0xFFFF00, 0x000000, 0x000000, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0xFFFF00,
  0x000000, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFF00, 0x000000,
  0x000000, 0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0xFFFF00,
  0xFFFF00, 0x000000, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0xFFFF00, 0xFFFF00, 0x000000, 0x000000, 0x000000,
  0x000000, 0xFFFF00, 0x000000, 0x000000, 0x000000, 0x000000, 0xFFFF00, 0x000000,
  0xFFFF00, 0x000000, 0x000000, 0x000000, 0xFFFF00, 0x000000, 0x000000, 0xFFFF00,
};

uint32_t static showers_8x8[] = {
  0x000000, 0x8000FF, 0x8000FF, 0x8000FF, 0x000000, 0x000000, 0x000000, 0x000000,
  0x8000FF, 0x000000, 0x000000, 0x000000, 0x8000FF, 0x8000FF, 0x8000FF, 0x000000,
  0x8000FF, 0x000000, 0x000000, 0x000000, 0x8000FF, 0x000000, 0x000000, 0x8000FF,
  0x8000FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x8000FF,
  0x8000FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x8000FF,
  0x000000, 0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF, 0x8000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x000000,
  0x000000, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
};

uint32_t static rain_8x8[] = {
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000
};

uint32_t static storm_8x8[] = {
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000,
  0x0000FF, 0x000000, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x000000,
  0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
  0x0000FF, 0x0000FF, 0x0000FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000,
};

uint32_t static snow_8x8[] = {
  0x8080FF, 0x000000, 0x8080FF, 0x000000, 0x000000, 0x8080FF, 0x000000, 0x8080FF,
  0x000000, 0x8080FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x8080FF, 0x000000,
  0x8080FF, 0x000000, 0x8080FF, 0x000000, 0x000000, 0x8080FF, 0x000000, 0x8080FF,
  0x000000, 0x000000, 0x000000, 0x8080FF, 0x8080FF, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x8080FF, 0x8080FF, 0x000000, 0x000000, 0x000000,
  0x8080FF, 0x000000, 0x8080FF, 0x000000, 0x000000, 0x8080FF, 0x000000, 0x8080FF,
  0x000000, 0x8080FF, 0x000000, 0x000000, 0x000000, 0x000000, 0x8080FF, 0x000000,
  0x8080FF, 0x000000, 0x8080FF, 0x000000, 0x000000, 0x8080FF, 0x000000, 0x8080FF,
};

uint32_t static fog_8x8[] = { // FIXME white lines or something...
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000,
  0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF,
  0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000, 0x0000FF, 0x000000
};

uint32_t static heart_8x8[] = {
  0x000000, 0xFF0000, 0xFF0000, 0x000000, 0xFF0000, 0xFF0000, 0x000000, 0x000000,
  0xFF0000, 0x000000, 0x000000, 0xFF0000, 0x000000, 0x000000, 0xFF0000, 0x000000,
  0xFF0000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0xFF0000, 0x000000,
  0x000000, 0xFF0000, 0x000000, 0x000000, 0x000000, 0xFF0000, 0x000000, 0x000000,
  0x000000, 0xFF0000, 0x000000, 0x000000, 0x000000, 0xFF0000, 0x000000, 0x000000,
  0x000000, 0x000000, 0xFF0000, 0x000000, 0xFF0000, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0xFF0000, 0x000000, 0x000000, 0x000000, 0x000000,
  0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000, 0x000000
};

// Get color565 directly from 24-bit RGB value
// TODO - replace arrays with color565 values!
uint16_t color565(uint32_t rgb) {
  return (((rgb>>16) & 0xF8) << 8) |
    (((rgb>>8) & 0xFC) << 3) |
    ((rgb & 0xFF) >> 3);
};

void drawTestBitmap() {
  drawBitmap(BITMAP_X,       BITMAP_Y,  8,  8, sun_8x8);
  drawBitmap(BITMAP_X +   9, BITMAP_Y,  8,  8, cloud_8x8);
  drawBitmap(BITMAP_X +  18, BITMAP_Y,  8,  8, rain_8x8);
  drawBitmap(BITMAP_X +  27, BITMAP_Y,  8,  8, showers_8x8);
  drawBitmap(BITMAP_X +  36, BITMAP_Y,  8,  8, snow_8x8);
  drawBitmap(BITMAP_X +  45, BITMAP_Y,  8,  8, storm_8x8);
  //drawBitmap(BITMAP_X +  55, BITMAP_Y, 12, 20, minion);
}

// this draws five icons that could represent the next couple of hours (+3h, +6h, ...)
// or we could put an avg. per day and show next 5-day forecast
void draw5DayForecastIcons(struct forecast_info *forecasts, int num) {
  for (int n = 0; n < num; ++n) { // for each "day"
    if (forecasts[n].condition >= 200 && forecasts[n].condition < 300) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, storm_8x8);
    }
    if ((forecasts[n].condition >= 300 && forecasts[n].condition < 400) ||
	(forecasts[n].condition >= 520 && forecasts[n].condition < 600)) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, showers_8x8);
    }
    if (forecasts[n].condition >= 500 && forecasts[n].condition < 510) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, rain_8x8);
    }
    if ((forecasts[n].condition >= 600 && forecasts[n].condition < 700) ||
	(forecasts[n].condition == 511)) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, snow_8x8);
    }
    if (forecasts[n].condition >= 700 && forecasts[n].condition < 800) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, fog_8x8);
    }
    if (forecasts[n].condition >= 800 && forecasts[n].condition < 802) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, sun_8x8);
    }
    if (forecasts[n].condition >= 802 && forecasts[n].condition < 900) {
      drawBitmap(BITMAP_X + n * 10, BITMAP_Y, 8, 8, cloud_8x8);
    }
  }
}

void draw5DayForecastValues(struct forecast_info *forecasts, int num) {

}

// Draw one of the available weather icons in the specified space
void drawWeatherIcon(int startx, int starty, int width, int height, uint8_t icon, bool enlarged) {
  switch (icon) {
    case 0:
      drawBitmap(startx, starty, width, height, sun_8x8, enlarged);
      break;
    case 1:
      drawBitmap(startx, starty, width, height, cloud_8x8, enlarged);
      break;
    case 2:
      drawBitmap(startx, starty, width, height, showers_8x8, enlarged);
      break;
    case 3:
      drawBitmap(startx, starty, width, height, rain_8x8, enlarged);
      break;
    case 4:
      drawBitmap(startx, starty, width, height, storm_8x8, enlarged);
      break;
    case 5:
      drawBitmap(startx, starty, width, height, snow_8x8, enlarged);
      break;
  }
}

void displayTodaysWeather() {
  drawWeatherIcon(WEATHER_TODAY_X, WEATHER_TODAY_Y, 8, 8, forecast5Days[0], true);
}

void displayTodaysTempRange() {
  dma_display->fillRect(TEMPRANGE_X, TEMPRANGE_Y, TEMPRANGE_WIDTH, TEMPRANGE_HEIGHT, 0);
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  dma_display->setTextColor(TEMPRANGE_COLOR);

  dma_display->setCursor(TEMPRANGE_X, TEMPRANGE_Y);
  dma_display->printf("%3d/%3d C", minTempToday, maxTempToday);

  // Draw the degree symbol manually
  dma_display->fillRect(TEMPRANGE_X + 44, TEMPRANGE_Y, 2, 2, TEMPRANGE_COLOR);
}

void displayWeatherForecast() {
  for (int i=1; i<5; i++) {  //skip day 0, since we are already displaying it somewhere else using displayTodaysWeather()
    drawWeatherIcon(WEATHER_FORECAST_X + 9*(i-1), WEATHER_FORECAST_Y, 8, 8, forecast5Days[i], false);
  }
}

void displayWeatherData() {
  displayTodaysWeather();
  displayTodaysTempRange();
  displayWeatherForecast();
}

//Source: https://github.com/witnessmenow/LED-Matrix-Display-Examples/blob/master/LED-Matrix-Mario-Display/LED-Matrix-Mario-Display.ino
// void drawBitmap(int startx, int starty, int width, int height, uint32_t *bitmap) {
//   int counter = 0;
//   for (int yy = 0; yy < height; yy++) {
//     for (int xx = 0; xx < width; xx++) {
//       dma_display->drawPixel(startx+xx, starty+yy, color565(bitmap[counter]));
//       counter++;
//     }
//   }
// }

// Draw the bitmap, with an option to enlarge it by a factor of two
void drawBitmap(int startx, int starty, int width, int height, uint32_t *bitmap, bool enlarged /* = false*/) {
  int counter = 0;

  for (int yy = 0; yy < height; yy++) {
    for (int xx = 0; xx < width; xx++) {
      if (enlarged) {
	dma_display->drawPixel(startx+2*xx,   starty+2*yy,   color565(bitmap[counter]));
	dma_display->drawPixel(startx+2*xx+1, starty+2*yy,   color565(bitmap[counter]));
	dma_display->drawPixel(startx+2*xx,   starty+2*yy+1, color565(bitmap[counter]));
	dma_display->drawPixel(startx+2*xx+1, starty+2*yy+1, color565(bitmap[counter]));
      }
      else {
	dma_display->drawPixel(startx+xx, starty+yy, color565(bitmap[counter]));
      }
      counter++;
    }
  }
}


void drawHeartBeat() {
  if (!heartBeat) {
    dma_display->fillRect(HEARTBEAT_X, HEARTBEAT_Y, 8, 8, 0);
  }
  else {
    drawBitmap(HEARTBEAT_X, HEARTBEAT_Y, 8, 8, heart_8x8);
  }
}


// Return a mapping from the Accuweather icons to the
// internal icons:
// 0 - sun
// 1 - clouds
// 2 - showers
// 3 - rain
// 4 - storm
// 5 - snow
// Based on https://apidev.accuweather.com/developers/weatherIcons
int accuWeatherIconMapping(int icon) {
  if (icon <= 5)  return 0;
  if (icon <= 11) return 1;
  if (icon <= 14) return 2;
  if (icon <= 17) return 4;
  if (icon == 18) return 3;
  if (icon <= 29) return 5;
  if (icon == 30) return 0;
  if (icon <= 32) return 2;
  return 0;
}


/* Start of code to get data from openweathermap - based on work by https://github.com/lefty01
*/
int getOpenWeatherData(uint32_t loc_id, const char *units, const char *appid,
			 struct city_info *info)
{
  StaticJsonDocument<272> filter;
  filter["city"] = true;

  JsonObject filter_list_0 = filter["list"].createNestedObject();
  filter_list_0["dt"] = true;

  JsonObject filter_list_0_weather_0 = filter_list_0["weather"].createNestedObject();
  filter_list_0_weather_0["icon"] = true;
  filter_list_0_weather_0["id"] = true;
  filter_list_0["wind"]["speed"] = true;

  JsonObject filter_list_0_main = filter_list_0.createNestedObject("main");
  filter_list_0_main["temp"] = true;
  filter_list_0_main["temp_min"] = true;
  filter_list_0_main["temp_max"] = true;
  filter_list_0_main["pressure"] = true;   // Atmospheric pressure on the sea level, hPa
  filter_list_0_main["grnd_level"] = true; // Atmospheric pressure on the ground level, hPa

  filter_list_0_main["humidity"] = true;

  char url[128];

  // sanity check units ...
  // strcmp(units, "standard") ... "metric", or "imperial"
  snprintf(url, 128, "http://api.openweathermap.org/data/2.5/forecast?id=%u&units=%s&appid=%s",
	   loc_id, units, appid);

  // Allocate the largest possible document (platform dependent)
  // DynamicJsonDocument doc(ESP.getMaxFreeBlockSize());
  //DynamicJsonDocument doc(8192);
  DynamicJsonDocument doc(12288);

  http.useHTTP10(true);
  http.begin(url);
  http.GET();

  DeserializationError error = deserializeJson(doc, http.getStream(),
					       DeserializationOption::Filter(filter));
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return 1;
  }

  // Release unused memory
  doc.shrinkToFit();

  // see: https://openweathermap.org/weather-conditions#Weather-Condition-Codes-2
  //      for description of weather_0_id

  // TODO: get an avg. or just one forecast (eg. at noon) per day
  //       such that we have one value for each of the next five days

  uint8_t n = 0;
  for (JsonObject elem : doc["list"].as<JsonArray>()) {
    long dt           = elem["dt"]; // timestamp epoch
    JsonObject main   = elem["main"];

    float main_temp     = main["temp"]; // eg. 26.12, ...       degree Celsius
    float main_temp_min = main["temp_min"]; // 22.76, 17.26, ...
    float main_temp_max = main["temp_max"]; // 26.26, 20.84, ...
    int main_pressure   = main["pressure"]; // eg. 1014, ...    hPa
    int main_grnd_level = main["grnd_level"]; // 969, 970, ...
    int main_humidity   = main["humidity"]; // eg. 76,   ...    Humidity, %
    float wind_speed    = elem["wind"]["speed"]; // eg.  ...    m/s
    int weather_0_id    = elem["weather"][0]["id"]; // eg. ...  condition status
    const char* weather_0_icon = elem["weather"][0]["icon"];  // eg. "03d", "04d"

    Serial.println(F("-------------------------------------------------"));
    Serial.printf("forecast No. %d\n", n);
    Serial.println(dt);
    Serial.println(epoch2String(dt));
    Serial.println(weather_0_id);
    Serial.println(weather_0_icon);
    Serial.println(main_temp);
    Serial.println(main_temp_min);
    Serial.println(main_temp_max);
    Serial.println(main_pressure);
    Serial.println(main_grnd_level);
    Serial.println(main_humidity);
    Serial.println(wind_speed);

    if (n < 5) { // FIXME: look at specific time window and get one id for every day
      info->forecasts[n].condition  = weather_0_id; // forecasts[n]->condition ?? ptr
      info->forecasts[n].temp       = main_temp;
      info->forecasts[n].temp_min   = main_temp_min;
      info->forecasts[n].temp_max   = main_temp_max;
      info->forecasts[n].pressure   = main_pressure;
      info->forecasts[n].grnd_level = main_grnd_level;
      info->forecasts[n].humidity   = main_humidity;
      info->forecasts[n].wind       = wind_speed;
      info->forecasts[n].time       = dt;
    }
    n++;
    // if (n > (5 * 3))
    //   break;
  }
  JsonObject city = doc["city"];
  const char* city_name = city["name"]; // "Böblingen"

  float city_coord_lat = city["coord"]["lat"]; // 48.6833
  float city_coord_lon = city["coord"]["lon"]; // 9.0167

  int city_timezone = city["timezone"]; // 7200 -> use to configure NTP/DST !?
  long city_sunrise = city["sunrise"]; // 1623813640
  long city_sunset = city["sunset"];   // 1623871722

  info->timezone = city_timezone;
  info->sunrise  = city_sunrise;
  info->sunset   = city_sunset;

  Serial.println(city_name);
  Serial.printf("lat: %f, lon: %f\n", city_coord_lat, city_coord_lon);
  Serial.printf("timezone: %d\n", city_timezone);
  Serial.printf("sunrise:  %s\n", epoch2String(city_sunrise).c_str());
  Serial.printf("sunset :  %s\n", epoch2String(city_sunset).c_str());

  return 0;
}


void getAccuWeatherData() {
  HTTPClient http;
  char url[256];
  DynamicJsonDocument doc(16384); // Might be overkill, since the Accuweather JSONs are about 3-5K in length - but better safe...

  StaticJsonDocument<300> filter;
  filter["DailyForecasts"][0]["Date"] = true;
  filter["DailyForecasts"][0]["Temperature"]["Minimum"]["Value"] = true;
  filter["DailyForecasts"][0]["Temperature"]["Maximum"]["Value"] = true;
  filter["DailyForecasts"][0]["Day"]["Icon"] = true;

  /* Filter should look like this:
  {
	"DailyForecasts": [
		"Date": True,
		"Temperature": {
			"Minimum": {
				"Value": True
			},
      "Maximum": {
        "Value": True
      }
		},
	"Day": {
		"Icon": true
	}
	]
  } */

  snprintf( url, 256, "http://dataservice.accuweather.com/forecasts/v1/daily/5day/%s?apikey=%s&metric=true",
	    ACCUWEATHER_CITY_CODE, ACCUWEATHER_API_KEY);

  http.begin(url);
  http.GET(); //TODO - check status code!

  DeserializationError error = deserializeJson( doc, http.getStream(),
						DeserializationOption::Filter(filter));

  if (error) {
    Serial.print(F("deserialization failed: "));
    Serial.println(error.f_str());
    logStatusMessage("Weather data error!");
  }

  doc.shrinkToFit();

  //Just in case we need to debug...
  //serializeJsonPretty(doc, Serial);

  //Populate the variables:
  minTempToday = round( double(doc["DailyForecasts"][0]["Temperature"]["Minimum"]["Value"]) );
  maxTempToday = round( double(doc["DailyForecasts"][0]["Temperature"]["Maximum"]["Value"]) );

  Serial.println(minTempToday);
  Serial.println(maxTempToday);

  for (int i=0; i<5; i++) {
    forecast5Days[i] = accuWeatherIconMapping(doc["DailyForecasts"][i]["Day"]["Icon"]);
  }
}

