#pragma once

// ----------------------
// Preferences namespace
// ----------------------
#define PREFERENCES_NAMESAPCE "DEVICE"

// ----------------------
// Fish feeding interval
// ----------------------
#define FEED_INTERVAL 172800 // 48h

// ----------------------
// Pin connections
// ----------------------
#define LED_PIN 8
#define LED_COUNT 1
#define SERVO_PIN 2

// ----------------------
// Wi-Fi credentials
// ----------------------
const char *ssid = "ssid";
const char *password = "password";

// ----------------------
// MQTT credentials
// ----------------------
const char *mqtt_server = "mqtt_server";
const int mqtt_port = 8883;
const char *mqtt_user = "mqtt_user";
const char *mqtt_password = "mqtt_password";

// ----------------------
// Cloudflare credentials
// ----------------------
const char http_base_url[] = "http_base_url";
const char notify_endpoint[] = "notify_endpoint";
const char api_key[] = "api_key";

// ----------------------
// MQTT Topics
// ----------------------
const char *mqtt_discovery_request_topic = "iot-commander/discovery/request";
const char *mqtt_discovery_response_topic = "iot-commander/discovery/response";
const char *mqtt_request_topic = "iot-commander/esp-fish-feeder/request";
const char *mqtt_response_topic = "iot-commander/esp-fish-feeder/response";

// ----------------------
// Device
// ----------------------
const char *device_id = "ebfae72e-99c8-4c32-aac3-e817748f625f";
const char *device_name = "FISH_FEEDER";

// ----------------------
// TLS Root Certificate
// ----------------------
const char rootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)EOF";

const char httpRootCA[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
...
-----END CERTIFICATE-----
)EOF";