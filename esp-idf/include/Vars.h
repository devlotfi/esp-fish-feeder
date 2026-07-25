#pragma once

#include <Adafruit_NeoPixel.h>
#include <ESP32Servo.h>
#include <Preferences.h>
#include <EspNowMqttGateway.h>

Adafruit_NeoPixel pixel(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);
Preferences preferences;
Servo servo;
volatile bool pendingFeed = false;
char currentColor[8] = "#ffffff";
int currentBrightness = 128;