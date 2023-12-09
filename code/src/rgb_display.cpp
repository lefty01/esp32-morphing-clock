
#include "rgb_display.h"

#include <ESP32-HUB75-MatrixPanel-I2S-DMA.h>

#include "common.h"

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
// From: https://gist.github.com/davidegironi/3144efdc6d67e5df55438cc3cba613c8
uint16_t colorWheel(uint8_t pos) {
  if(pos < 85) {
    return dma_display->color565(pos * 3, 255 - pos * 3, 0);
  } else if(pos < 170) {
    pos -= 85;
    return dma_display->color565(255 - pos * 3, 0, pos * 3);
  } else {
    pos -= 170;
    return dma_display->color565(0, pos * 3, 255 - pos * 3);
  }
}

void display_init() {
  HUB75_I2S_CFG::i2s_pins _pins={R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN};
  HUB75_I2S_CFG mxconfig(
			 PANEL_WIDTH, // Module width
			 PANEL_HEIGHT, // Module height
			 1, // chain length
			 _pins // pin mapping
			 );
  // mxconfig.gpio.e = E_PIN;
  mxconfig.driver = HUB75_I2S_CFG::FM6124;
  dma_display = new MatrixPanel_I2S_DMA(mxconfig);

  // MUST DO THIS FIRST!
  dma_display->begin(); // Use default values for matrix dimentions and pins supplied within ESP32-HUB75-MatrixPanel-I2S-DMA.h
  //dma_display->setPanelBrightness(110);
}

void logStatusMessage(const char *message, bool publish=true) {
  Serial.println(message);
  // Clear the last line first!
  dma_display->fillRect(0, 56, PANEL_WIDTH, 8, 0);

  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves

  dma_display->setCursor(0, 56);   // Write on last line

  dma_display->setTextColor(LOG_MESSAGE_COLOR);
  dma_display->print(message);

  if (publish)
    client.publish(MQTT_STATUS_TOPIC, message);

  messageDisplayMillis = millis();
  logMessageActive = true;
}


void logStatusMessage(String message) {
  logStatusMessage(message.c_str());
}

// FIXME hardcoded 56 value !
void clearStatusMessage() {
   dma_display->fillRect(0, 56, PANEL_WIDTH, 8, 0);
   logMessageActive = false;
}

void clearForecast() {
   dma_display->fillRect(0, BITMAP_Y, PANEL_WIDTH, 8, 0);
}

void clearSensorData() {
  dma_display->fillRect(SENSOR_DATA_X, SENSOR_DATA_Y,
			SENSOR_DATA_WIDTH, SENSOR_DATA_HEIGHT, 0);
}


//void displaySensorData(const MqttSensors &s) {
// todo: right now on my 64x64 I have space for three sensor values in the
//       top-left corner of the display. so if more values are to be displayed
//       then cycle through them on each call of displaySensorData
void displaySensorData() {
  //static int cycle = 0;
  Serial.printf("number of sensors: %d\n", mySensors.size());

  for (size_t i = 0; i < mySensors.size(); ++i) {
    Serial.printf("[%d] type of sensors: %d\n", i, mySensors[i].type);
    Serial.printf("[%d] is dead?:        %d\n", i, mySensors[i].isDead);
      // float or int? depend on type? or use extra flag
    if (!mySensors[i].isDead) {
      Serial.printf("[%d] value int:     %d\n", i, mySensors[i].val_i);
      Serial.printf("[%d] value float:   %f\n", i, mySensors[i].val_f);
    }
    //
    //
    //mySensors[i].newData = false;
    //}
  }
  // for (int i; i = 0; ++i) {
  //   int val = s[i].val_i;
  // }
/* void do_something(struct MqttSensor *s) */
/* { */
/*   switch(s->type) */
/*   { */
/*     case TEMPERATURE:  // do something with s->val_f */
/*       break; */
/*     case HUMIDITY:  // do something with s->val_i */
/*       break; */
/*     case CO2:  // do something with s->val_i */
/*       break; */
/*   } */
/* } */

#if 0
  if (sensorDead) {
    dma_display->fillRect(SENSOR_DATA_X, SENSOR_DATA_Y, SENSOR_DATA_WIDTH, SENSOR_DATA_HEIGHT, 0);
    dma_display->setTextSize(1);     // size 1 == 8 pixels high
    dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
    dma_display->setTextColor(SENSOR_ERROR_DATA_COLOR);

    dma_display->setCursor(SENSOR_DATA_X, SENSOR_DATA_Y);
    dma_display->print("No sensor data!");
  }

  if (newMqttSensorData) {
    dma_display->fillRect(SENSOR_DATA_X, SENSOR_DATA_Y, SENSOR_DATA_WIDTH, SENSOR_DATA_HEIGHT, 0);
    dma_display->setTextSize(1);     // size 1 == 8 pixels high
    dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
    dma_display->setTextColor(SENSOR_DATA_COLOR);
//    dma_display->setFont(&FreeSerifBold12pt7b);

    dma_display->setCursor(SENSOR_DATA_X, SENSOR_DATA_Y);
    dma_display->printf("%4.1f C           %3d%%", sensorTemp, sensorHumi);

    // Draw the degree symbol manually
    dma_display->fillRect(SENSOR_DATA_X + 25, SENSOR_DATA_Y, 2, 2, SENSOR_DATA_COLOR);

    newMqttSensorData = false;
  }
#endif
}

void displayLightData(float luxValue) {
  dma_display->fillRect(LIGHT_DATA_X, LIGHT_DATA_Y, LIGHT_DATA_WIDTH, LIGHT_DATA_HEIGHT, 0);

  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  dma_display->setTextColor(SENSOR_DATA_COLOR);
  dma_display->setCursor(LIGHT_DATA_X, LIGHT_DATA_Y);
  dma_display->printf("%4.0f", luxValue);
}


// getting the first item (+3h) from forecast info and show temp and humidity
void displayWeatherData(const struct city_info &info) {
  Serial.printf("displayWeatherData: temp=%2.1f, humi=%d\n",
		info.forecasts[0].temp, info.forecasts[0].humidity);
fixme ... 
  dma_display->fillRect(SENSOR_DATA_X, SENSOR_DATA_Y,
			SENSOR_DATA_WIDTH, SENSOR_DATA_HEIGHT, 0);
  dma_display->setTextSize(1);     // size 1 == 8 pixels high
  dma_display->setTextWrap(false); // Don't wrap at end of line - will do ourselves
  dma_display->setTextColor(SENSOR_DATA_COLOR);
  //dma_display->setFont(&FreeSerifBold12pt7b);

  dma_display->setCursor(SENSOR_1_DATA_X, SENSOR_1_DATA_Y);
  dma_display->printf("%2.1f", info.forecasts[0].temp);

  dma_display->setCursor(SENSOR_2_DATA_X, SENSOR_2_DATA_Y);
  dma_display->printf("%3d%%", info.forecasts[0].humidity);

  dma_display->setCursor(SENSOR_3_DATA_X, SENSOR_3_DATA_Y);
  dma_display->printf("%4d%", info.forecasts[0].grnd_level);

  // Draw the degree symbol manually
  dma_display->fillRect(SENSOR_DATA_X + 25, SENSOR_DATA_Y, 2, 2, SENSOR_DATA_COLOR);

}

void displayForecastData() {

}

// Simple R/G/B screen fill, for testing displays
void displayTest(int delayMs) {
  dma_display->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, dma_display->color565(255, 0, 0));
  delay(delayMs);
  dma_display->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, dma_display->color565(0, 255, 0));
  delay(delayMs);
  dma_display->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, dma_display->color565(0, 0, 255));
  delay(delayMs);
  dma_display->fillRect(0, 0, MATRIX_WIDTH, MATRIX_HEIGHT, dma_display->color565(0, 0, 0));
}
