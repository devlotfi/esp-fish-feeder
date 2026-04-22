#pragma once

// ----------------------
// Preferences namespace
// ----------------------
#define PREFERENCES_NAMESAPCE "DEVICE"

// ----------------------
// Fish feeding interval
// ----------------------
#define FEED_INTERVAL 86400 // 24h

// ----------------------
// Pin connections
// ----------------------
#define LED_PIN 8
#define LED_COUNT 1
#define SERVO_PIN 2

// ----------------------
// ESP-NOW MQTT Gateway config
// ----------------------
const char *gatewayMac = "30:AE:A4:11:22:33";
const char *peerMac = "D6:3F:87:A1:C1:01";
const char *pmk = "1187BA60CAA9A7FA52B2B8C4141B4A9D";
const char *lmk = "3339EAE204DBDDDC75B98F49BCEB26BB";
uint8_t channel = 1;

// ----------------------
// MQTT Topics
// ----------------------
const char *mqtt_discovery_request_topic = "esp-commander/discovery/request";
const char *mqtt_discovery_response_topic = "esp-commander/discovery/response";
const char *mqtt_request_topic = "esp-commander/esp-fish-feeder/request";
const char *mqtt_response_topic = "esp-commander/esp-fish-feeder/response";

// ----------------------
// Device
// ----------------------
const char *device_id = "622070b6-8164-45c3-b11e-47a59197617c";
const char *device_name = "FISH_FEEDER";