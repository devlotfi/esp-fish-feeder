#pragma once

#include <ArduinoJson.h>

namespace EspCommander
{
  class Serializable
  {
  public:
    virtual void serialize(ArduinoJson::JsonObject &) = 0;
  };
}