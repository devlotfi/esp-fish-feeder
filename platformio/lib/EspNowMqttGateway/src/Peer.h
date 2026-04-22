#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "Utils.h"
#include "Types.h"

namespace EspNowMqttGateway
{
  void onSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status);
  void onRecieve(const esp_now_recv_info_t *info, const uint8_t *data, int len);

  struct PeerConfig
  {
    const char *pmk;
    const char *lmk;
    const char *gatewayMac;
    const char *peerMac;
    uint8_t channel;
    void (*handleRecieve)(const char *, const char *);
  };

  class Peer
  {
  public:
    inline static uint8_t pmk[ESP_NOW_KEY_SIZE_BYTES];
    inline static uint8_t lmk[ESP_NOW_KEY_SIZE_BYTES];
    inline static uint8_t gatewayMac[MAC_SIZE_BYTES];
    inline static uint8_t peerMac[MAC_SIZE_BYTES];
    inline static uint8_t channel;
    inline static void (*handleRecieve)(const char *, const char *);

    inline static void init(PeerConfig &peerConfig)
    {
      Serial.println(peerConfig.pmk);
      Serial.println(peerConfig.lmk);
      Serial.println(peerConfig.gatewayMac);
      Serial.println(peerConfig.peerMac);
      Serial.println(peerConfig.channel);
      keyHexToBytes(peerConfig.pmk, pmk);
      keyHexToBytes(peerConfig.lmk, lmk);
      macStringToBytes(peerConfig.gatewayMac, gatewayMac);
      macStringToBytes(peerConfig.peerMac, peerMac);
      channel = peerConfig.channel;
      Peer::handleRecieve = peerConfig.handleRecieve;

      WiFi.mode(WIFI_STA);
      esp_wifi_set_mac(WIFI_IF_STA, peerMac);
      esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
      esp_wifi_set_ps(WIFI_PS_NONE);

      if (esp_now_init() != ESP_OK)
      {
        Serial.println("ESP-NOW init failed");
        return;
      }

      esp_now_set_pmk(pmk);

      esp_now_peer_info_t peerInfo = {};
      memcpy(peerInfo.peer_addr, gatewayMac, MAC_SIZE_BYTES);
      peerInfo.encrypt = true;
      peerInfo.channel = channel;
      memcpy(peerInfo.lmk, lmk, ESP_NOW_KEY_SIZE_BYTES);
      if (esp_now_add_peer(&peerInfo) != ESP_OK)
        Serial.println("Failed to add peer");

      esp_now_register_recv_cb(onRecieve);
      esp_now_register_send_cb(onSent);
    }

    inline static void timeSync()
    {
      static const unsigned long CHECK_INTERVAL = 2000; // check every 2s
      static unsigned long lastCheck = 0;
      static bool firstCall = true;

      unsigned long nowMillis = millis();

      // If it's not the first call, enforce the 2-second delay
      if (!firstCall && (nowMillis - lastCheck < CHECK_INTERVAL))
        return;

      lastCheck = nowMillis;
      firstCall = false;

      time_t now;
      time(&now);

      // 1577836800 = Jan 1 2020
      if (now < 1577836800)
      {
        Serial.println("Time not synced → starting sync");
        timeSyncMessage();
      }
    }

    inline static void mqttMessage(const char *topic, const char *text)
    {
      EspNowMessage msg = {};
      msg.type = MessageType::TEXT_MESSAGE;

      strncpy(msg.payload.mqttEspNowMessage.topic, topic, ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE - 1);
      msg.payload.mqttEspNowMessage.topic[ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE - 1] = '\0';

      strncpy(msg.payload.mqttEspNowMessage.text, text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1);
      msg.payload.mqttEspNowMessage.text[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1] = '\0';

      esp_err_t result = esp_now_send(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result == ESP_OK)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.printf("ESP-NOW: Send failed, err=0x%x\n", result);
    }

    inline static void notificationMessage(const char *title, const char *body)
    {
      EspNowMessage msg = {};
      msg.type = MessageType::NOTIFICATION_MESSAGE;

      strncpy(msg.payload.notificationEspNowMessage.title, title, ESP_NOW_MQTT_GATEWAY_NOTIFICATION_TITLE_SIZE - 1);
      msg.payload.notificationEspNowMessage.title[ESP_NOW_MQTT_GATEWAY_NOTIFICATION_TITLE_SIZE - 1] = '\0';

      strncpy(msg.payload.notificationEspNowMessage.body, body, ESP_NOW_MQTT_GATEWAY_NOTIFICATION_BODY_SIZE - 1);
      msg.payload.notificationEspNowMessage.body[ESP_NOW_MQTT_GATEWAY_NOTIFICATION_BODY_SIZE - 1] = '\0';

      esp_err_t result = esp_now_send(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result == ESP_OK)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.printf("ESP-NOW: Send failed, err=0x%x\n", result);
    }

    inline static void timeSyncMessage()
    {
      EspNowMessage msg = {};
      msg.type = MessageType::TIME_SYNC_MESSAGE;

      esp_err_t result = esp_now_send(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result == ESP_OK)
        Serial.println("ESP-NOW: Time Sync Packet accepted");
      else
        Serial.printf("ESP-NOW: Send failed, err=0x%x\n", result);
    }
  };

  inline void onSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status)
  {
    Serial.print("ESP-NOW: Send status -> ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
  }

  static char topic[ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE];
  static char textData[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE];
  inline void onRecieve(const esp_now_recv_info_t *info, const uint8_t *data, int len)
  {
    if (!data || len != sizeof(EspNowMqttGateway::EspNowMessage))
    {
      Serial.println("ESP-NOW: Invalid frame size");
      return;
    }

    const EspNowMqttGateway::EspNowMessage *msg = reinterpret_cast<const EspNowMqttGateway::EspNowMessage *>(data);

    switch (msg->type)
    {
    case EspNowMqttGateway::MessageType::TEXT_MESSAGE:
    {
      memcpy(topic, msg->payload.mqttEspNowMessage.topic, ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE - 1);
      topic[ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE - 1] = '\0';

      memcpy(textData, msg->payload.mqttEspNowMessage.text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE);
      textData[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1] = '\0';

      Serial.println("------------------------------------------");
      Serial.printf("Data Length: %d\n", len);
      Serial.printf("Topic: %s\n", topic);
      Serial.printf("Text: %s\n", msg->payload.mqttEspNowMessage.text);
      Serial.println("------------------------------------------\n");

      Peer::handleRecieve(topic, textData);

      break;
    }
    case EspNowMqttGateway::MessageType::TIME_SYNC_MESSAGE:
    {
      uint32_t epoch = msg->payload.timeSyncEspNowMessage.epoch;
      struct timeval tv = {.tv_sec = epoch};
      settimeofday(&tv, nullptr);
      Serial.println("ESP-NOW: Time Synced");
      break;
    }

    default:
      Serial.printf("ESP-NOW: Unknown type %d\n", msg->type);
      break;
    }
  }
}
