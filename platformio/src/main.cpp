#include <EspCommander.h>
#include "Properties.h"
#include "Vars.h"
#include "Device.h"
#include "Led.h"

static char output[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE];
void onReceive(const char *topic, const char *text)
{
  if (strcmp(mqtt_discovery_request_topic, topic) == 0)
  {
    Serial.println("Discovery called");
    device.discovery(output, ESP_COMMANDER_JSON_BUFFER_SIZE);
    EspNowMqttGateway::Peer::mqttMessage(mqtt_discovery_response_topic, output);
  }
  else if (strcmp(mqtt_request_topic, topic) == 0)
  {
    Serial.println("Request called");
    device.request(text, output, ESP_COMMANDER_JSON_BUFFER_SIZE);
    Serial.println("Request result: ");
    Serial.println(output);
    EspNowMqttGateway::Peer::mqttMessage(mqtt_response_topic, output);
  }
}

void setup_led()
{
  preferences.begin(PREFERENCES_NAMESAPCE, false);
  auto ledConfig = loadConfig();
  strncpy(currentColor, ledConfig.color, sizeof(currentColor) - 1);
  currentColor[sizeof(currentColor) - 1] = '\0';
  currentBrightness = ledConfig.brightness;
  Serial.println(ledConfig.color);
  Serial.println(ledConfig.brightness);
  setLed(ledConfig.color, ledConfig.brightness);
}

void setup_servo()
{
  ESP32PWM::allocateTimer(0);
  ESP32PWM::allocateTimer(1);
  ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);
  servo.setPeriodHertz(50);
}

void setup()
{
  Serial.begin(115200);
  setup_led();
  setup_servo();

  EspNowMqttGateway::PeerConfig peerConfig{
      .pmk = pmk,
      .lmk = lmk,
      .gatewayMac = gatewayMac,
      .peerMac = peerMac,
      .channel = channel,
      .handleRecieve = onReceive,
  };
  EspNowMqttGateway::Peer::init(peerConfig);
}

void loop()
{
  EspNowMqttGateway::Peer::timeSync();
  checkAutoFeed();
  if (pendingFeed)
  {
    pendingFeed = false;
    feedFish();
  }
}