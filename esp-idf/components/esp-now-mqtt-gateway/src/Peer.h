#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include "Utils.h"
#include "Types.h"
#include "EspNowMessageQueue.h"

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
    void (*handleCommand)(const char *);
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
    inline static void (*handleCommand)(const char *);
    inline static char timezonePosix[64] = "UTC0";
    inline static time_t lastSyncEpoch = 0;

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
      Peer::handleCommand = peerConfig.handleCommand;

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
      initQueue();
    }

    inline static void timeSync()
    {
      static const unsigned long CHECK_INTERVAL = 2000;
      static unsigned long lastCheck = 0;
      static bool firstCall = true;

      unsigned long nowMillis = millis();

      if (!firstCall && (nowMillis - lastCheck < CHECK_INTERVAL))
        return;

      lastCheck = nowMillis;
      firstCall = false;

      time_t now;
      time(&now);

      if (now < 1577836800)
      {
        Serial.println("Time not synced → starting sync");
        timeSyncMessage();
        return;
      }

      if (lastSyncEpoch == 0 || (now - lastSyncEpoch) >= 86400)
      {
        Serial.println("24h elapsed or first sync → requesting time sync");
        timeSyncMessage();
      }
    }

    inline static void mqttMessage(const char *topic, const char *text)
    {
      EspNowMessage msg = {};
      msg.type = MessageType::TEXT_MESSAGE;

      strlcpy(msg.payload.mqttEspNowMessage.topic, topic, ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE - 1);
      strlcpy(msg.payload.mqttEspNowMessage.text, text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1);

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void notificationMessage(const char *title, const char *body)
    {
      EspNowMessage msg = {};
      msg.type = MessageType::NOTIFICATION_MESSAGE;

      strlcpy(msg.payload.notificationEspNowMessage.title, title, ESP_NOW_MQTT_GATEWAY_NOTIFICATION_TITLE_SIZE - 1);
      strlcpy(msg.payload.notificationEspNowMessage.body, body, ESP_NOW_MQTT_GATEWAY_NOTIFICATION_BODY_SIZE - 1);

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void timeSyncMessage()
    {
      EspNowMessage msg = {};
      msg.type = MessageType::TIME_SYNC_MESSAGE;

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void sleepyDataMessage(const char *text)
    {
      EspNowMessage msg = {};
      msg.type = MessageType::SLEEPY_DATA_MESSAGE;

      strlcpy(msg.payload.sleepyDataEspNowMessage.text, text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1);
      msg.payload.sleepyDataEspNowMessage.text[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1] = '\0';

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void sleepyCommandMessage()
    {
      EspNowMessage msg = {};
      msg.type = MessageType::SLEEPY_COMMAND_MESSAGE;

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void wolMessage(const uint16_t port, const uint8_t mac[MAC_SIZE_BYTES])
    {
      EspNowMessage msg = {};
      msg.type = MessageType::WOL_MESSAGE;
      msg.payload.wolEspNowMessage.port = port;
      memcpy(msg.payload.wolEspNowMessage.mac, mac, MAC_SIZE_BYTES);

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
    }

    inline static void metricMessage(const char body[GRAFANA_BODY_SIZE])
    {
      EspNowMessage msg = {};
      msg.type = MessageType::METRIC_MESSAGE;
      strlcpy(msg.payload.metricEspNowMessage.body, body, GRAFANA_BODY_SIZE);

      bool result = enqueueMessage(
          gatewayMac,
          (const uint8_t *)&msg,
          sizeof(EspNowMessage));

      if (result)
        Serial.println("ESP-NOW: Packet accepted");
      else
        Serial.println("ESP-NOW: Send failed");
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
      strlcpy(topic, msg->payload.mqttEspNowMessage.topic, ESP_NOW_MQTT_GATEWAY_TOPIC_SIZE);
      strlcpy(textData, msg->payload.mqttEspNowMessage.text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE);

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
      Serial.println("Lol:");
      Serial.println(msg->payload.timeSyncEspNowMessage.timezonePosix);
      strlcpy(Peer::timezonePosix,
              msg->payload.timeSyncEspNowMessage.timezonePosix,
              TIMEZONE_POSIX_SIZE);

      struct timeval tv = {
          .tv_sec = epoch,
          .tv_usec = 0};
      settimeofday(&tv, nullptr);

      // Apply timezone
      setenv("TZ", Peer::timezonePosix, 1);
      tzset();

      // Remember when we last synchronized
      Peer::lastSyncEpoch = epoch;

      Serial.println("ESP-NOW: Time Synced");
      printCurrentTime();
      break;
    }
    case EspNowMqttGateway::MessageType::SLEEPY_COMMAND_MESSAGE:
    {
      memcpy(textData, msg->payload.sleepyCommandEspNowMessage.text, ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE);
      textData[ESP_NOW_MQTT_GATEWAY_MQTT_MESSAGE_TEXT_PAYLOAD_SIZE - 1] = '\0';

      Serial.println("------------------------------------------");
      Serial.printf("Data Length: %d\n", len);
      Serial.printf("Text: %s\n", msg->payload.sleepyCommandEspNowMessage.text);
      Serial.println("------------------------------------------\n");

      Peer::handleCommand(textData);

      break;
    }

    default:
      Serial.printf("ESP-NOW: Unknown type %d\n", msg->type);
      break;
    }
  }
}
