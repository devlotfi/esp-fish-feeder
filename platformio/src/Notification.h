#pragma once

#include <ArduinoJson.h>
#include <IotCommander.h>
#include <etl/string.h>
#include "Properties.h"
#include "Vars.h"

static uint8_t notificationBuffer[IOTC_JSON_BUFFER_SIZE];
IotCommander::StaticBufferAllocator notificationAllocator(notificationBuffer, sizeof(notificationBuffer));
static char payload[1024];

void sendNotification(const char *title, const char *body)
{
  etl::string<256> url;
  url = http_base_url;
  url += notify_endpoint;

  if (https.begin(httpsClient, url.c_str()))
  {
    https.addHeader("Content-Type", "application/json");
    https.addHeader("x-api-key", api_key);

    // 1. Create the JSON document
    // JsonDocument replaces StaticJsonDocument in Version 7
    JsonDocument doc(&notificationAllocator);
    doc.clear();
    doc["title"] = title;
    doc["body"] = body;

    // 2. Serialize JSON to a buffer
    serializeJson(doc, payload);
    Serial.println(payload);
    // 3. Send the POST request
    int httpCode = https.POST((uint8_t *)payload, strlen(payload));

    Serial.printf("Notification [%s] status: %d\n", title, httpCode);

    https.end();
  }
}