#pragma once

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <PubSubClient.h>
#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <Preferences.h>

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Preferences preferences;
Servo servo;

char currentColor[8] = "#ffffff";
int currentBrightness = 128;

WiFiClientSecure espClient;
WiFiClientSecure httpsClient;
HTTPClient https;
PubSubClient client(espClient);