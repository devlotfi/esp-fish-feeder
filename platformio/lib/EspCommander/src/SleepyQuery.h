#pragma once

#include "Config.h"
#include "Serializable.h"
#include "Types.h"
#include "Value.h"
#include <ArduinoJson.h>
#include <etl/span.h>

namespace EspCommander
{
  class SleepyQuery : public Serializable
  {
  public:
    struct Params
    {
      etl::span<Value> results;
      QueryHandler handler;
    };

    etl::span<Value> results;
    QueryHandler handler;

    SleepyQuery(Params params) : results(params.results),
                                 handler(params.handler)
    {
    }

    void serialize(ArduinoJson::JsonObject &obj) override
    {
      for (auto item : results)
      {
        auto parameterObj = obj["results"].add<ArduinoJson::JsonObject>();
        item.serialize(parameterObj);
      }
    }
  };
}