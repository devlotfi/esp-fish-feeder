#pragma once

#include "Config.h"
#include "Serializable.h"
#include "Types.h"
#include "Value.h"
#include <ArduinoJson.h>
#include <etl/span.h>

namespace EspCommander
{
  class SleepyAction : public Serializable
  {
  public:
    struct Params
    {
      const char *name;
      etl::span<Value> parameters;
      SleepyActionHandler handler;
    };

    const char *name;
    etl::span<Value> parameters;
    SleepyActionHandler handler;

    SleepyAction(Params params) : name(params.name),
                                  parameters(params.parameters),
                                  handler(params.handler)
    {
    }

    void serialize(ArduinoJson::JsonObject &obj) override
    {
      obj["name"] = name;
      for (auto item : parameters)
      {
        auto parameterObj = obj["parameters"].add<ArduinoJson::JsonObject>();
        item.serialize(parameterObj);
      }
    }
  };
}