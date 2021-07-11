
#include <ESP32httpUpdate.h>
//#include <ESP8266WiFi.h>

#include "common.h"
#include "config.h"
#include "creds_mqtt.h"
#include "ota_update.h"
#include "rgb_display.h"


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
}


void perform_update() {
  //Warn before performing update
  Serial.print("Starting OTA update from: ");
  Serial.println(OTA_URL);
  logStatusMessage("OTA Requested!");
  delay(500);

  // Add optional callback notifiers
  // ESPhttpUpdate.onStart(update_started);
  // ESPhttpUpdate.onEnd(update_finished);
  // ESPhttpUpdate.onProgress(update_progress);
  // ESPhttpUpdate.onError(update_error);

  // ESPhttpUpdate.update("sass.ro", 80, OTA_FILENAME);
  t_httpUpdate_return ret = ESPhttpUpdate.update(OTA_URL);
  
  switch (ret) {
      case HTTP_UPDATE_FAILED:
        Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", ESPhttpUpdate.getLastError(), ESPhttpUpdate.getLastErrorString().c_str());
        logStatusMessage("Update failed!");
        break;

      case HTTP_UPDATE_NO_UPDATES:
        Serial.println("HTTP_UPDATE_NO_UPDATES");
        logStatusMessage("No updates!");
        break;

      case HTTP_UPDATE_OK:
        Serial.println("HTTP_UPDATE_OK");
        logStatusMessage("Update OK!");
        break;
  }
}
