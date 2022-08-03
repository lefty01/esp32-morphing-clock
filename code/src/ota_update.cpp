#include <HTTPClient.h>
#include <HTTPUpdate.h>

#include "common.h"
#include "config.h"
#include "creds_mqtt.h"
#include "ota_update.h"
#include "rgb_display.h"


void update_started() {
  Serial.println("CALLBACK:  HTTP update process started");
  logStatusMessage("OTA started...");
}

void update_finished() {
  Serial.println("CALLBACK:  HTTP update process finished");
  logStatusMessage("OTA Done");
}

void update_progress(int cur, int total) {
  Serial.printf("CALLBACK:  HTTP update process at %d of %d bytes...\n", cur, total);
  float percent = ((float)cur / (float)total) * 100;
  char p[16];  // 'OTA: 100 %'

  snprintf(p, 16, "OTA: %.3d %%", (int)percent);

  logStatusMessage(p);
}

void update_error(int err) {
  Serial.printf("CALLBACK:  HTTP update fatal error code %d\n", err);
  logStatusMessage("OTA ERROR!!!");
}


void perform_update() {
  //Warn before performing update
  Serial.print("Starting OTA update from: ");
  Serial.println(OTA_URL);
  logStatusMessage("OTA Requested!");
  delay(500);

  // Add optional callback notifiers
  httpUpdate.onStart(update_started);
  httpUpdate.onEnd(update_finished);
  httpUpdate.onProgress(update_progress);
  httpUpdate.onError(update_error);

  WiFiClient client;
  //wifiClient.setTimeout(20);  // timeout argument is defined in seconds for setTimeout
  //wifiClient.setCACert(exitzero_crt_str);

  t_httpUpdate_return ret = httpUpdate.update(client, OTA_URL);

  switch (ret) {
  case HTTP_UPDATE_FAILED:
    Serial.printf("HTTP_UPDATE_FAILED Error (%d): %s", httpUpdate.getLastError(), httpUpdate.getLastErrorString().c_str());
    logStatusMessage("Update failed!");
    logStatusMessage(httpUpdate.getLastErrorString().c_str());
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
