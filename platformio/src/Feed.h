#pragma once

#include <stdint.h>
#include <etl/string.h>
#include "Properties.h"
#include "Vars.h"

uint64_t getUnixTime()
{
  time_t now;
  time(&now);
  return (uint64_t)now;
}

struct FeedConfig
{
  uint64_t lastFeedTime;
};

void saveFeedConfig(FeedConfig conf)
{
  preferences.putBytes("feed", &conf, sizeof(FeedConfig));
}

FeedConfig loadFeedConfig()
{
  FeedConfig conf = {
      .lastFeedTime = 0};
  preferences.getBytes("feed", &conf, sizeof(FeedConfig));
  return conf;
}

void feedFish()
{
  servo.attach(SERVO_PIN, 500, 2300);

  // feed 2 times
  delay(500);
  servo.write(0);
  delay(600);
  servo.write(180);
  delay(1200);

  servo.write(0);
  delay(600);
  servo.write(180);
  delay(1200);

  servo.detach();

  FeedConfig conf = loadFeedConfig();
  uint64_t t = getUnixTime();
  if (t > 100000)
  {
    conf.lastFeedTime = t;
    saveFeedConfig(conf);
  }
  saveFeedConfig(conf);

  Serial.println("Fish fed and time saved");

  EspNowMqttGateway::Peer::notificationMessage("Fish feeder", "The fish have been fed.");
}

void checkAutoFeed()
{
  uint64_t now = getUnixTime();
  if (now < 100000)
    return;

  static unsigned long lastCheck = 0;

  if (millis() - lastCheck < 60000)
    return; // check every minute
  lastCheck = millis();

  FeedConfig conf = loadFeedConfig();

  if (conf.lastFeedTime == 0)
  {
    conf.lastFeedTime = now;
    saveFeedConfig(conf);
    return;
  }

  if (now - conf.lastFeedTime >= FEED_INTERVAL)
  {
    Serial.println("Auto feeding triggered");
    pendingFeed = true;
  }
}