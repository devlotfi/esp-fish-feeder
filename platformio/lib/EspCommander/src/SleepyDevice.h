#pragma once

#include <ArduinoJson.h>
#include <etl/span.h>
#include <etl/optional.h>
#include "SleepyAction.h"
#include "Config.h"
#include "SleepyQuery.h"
#include "Types.h"

namespace EspCommander
{
  class SleepyDevice
  {
  public:
    struct Params
    {
      etl::optional<SleepyQuery> query;
      etl::span<SleepyAction> actions;
      ArduinoJson::Allocator &jsonAllocator;
    };

    etl::optional<SleepyQuery> query;
    etl::span<SleepyAction> actions;
    ArduinoJson::Allocator &jsonAllocator;

    SleepyDevice(Params params) : query(params.query),
                                  actions(params.actions),
                                  jsonAllocator(params.jsonAllocator)
    {
    }

    void data(char *jsonStringData, size_t jsonStringDataSize)
    {
      etl::optional<const char *> error;
      HandlerValue handlerResults[ESP_COMMANDER_MAX_RESULTS];
      ArduinoJson::JsonDocument dataDoc(&jsonAllocator);

      if (query.has_value())
      {
        auto queryObj = dataDoc["query"].to<ArduinoJson::JsonObject>();
        query.value().serialize(queryObj);

        ESP_COMMANDER_LOG("Query handler begin");
        query->handler(handlerResults, error);
        ESP_COMMANDER_LOG("Query handler end");
        if (error.has_value())
        {
          ESP_COMMANDER_DEBUG("Query handler error");
          return;
        }

        auto resultsJson = dataDoc["results"].to<ArduinoJson::JsonObject>();
        for (size_t i = 0; i < query->results.size(); i++)
        {
          bool valid = query->results[i].validateResult(handlerResults[i], resultsJson);
          if (!valid)
          {
            ESP_COMMANDER_DEBUG(LibraryErrors::INVALID_RESULTS);
            return;
          }
        }
      }

      auto actionsArray = dataDoc["actions"].to<ArduinoJson::JsonArray>();
      for (auto &action : actions)
      {
        auto actionJson = actionsArray.add<ArduinoJson::JsonObject>();
        action.serialize(actionJson);
      }

      ArduinoJson::serializeJson(dataDoc, jsonStringData, jsonStringDataSize);
    }

    struct FormatValidationResult
    {
      bool valid;
      const char *name;
    };
    FormatValidationResult validateFormat(ArduinoJson::JsonDocument &requestDoc)
    {
      FormatValidationResult result = {.valid = false};
      ArduinoJson::JsonVariant actionVariant = requestDoc["action"];
      ArduinoJson::JsonVariant parametersVariant = requestDoc["parameters"];
      if (
          actionVariant && actionVariant.is<const char *>() &&
          parametersVariant && parametersVariant.is<ArduinoJson::JsonObject>())
      {
        result.name = actionVariant.as<const char *>();
        result.valid = true;
      }
      return result;
    }

    void action(const char *jsonStringAction)
    {
      etl::optional<const char *> error;
      ArduinoJson::JsonDocument actionDoc(&jsonAllocator);
      ArduinoJson::DeserializationError jsonError = ArduinoJson::deserializeJson(actionDoc, jsonStringAction);
      if (jsonError)
      {
        ESP_COMMANDER_LOG(LibraryErrors::INVALID_JSON);
        return;
      }

      auto formatValidationResult = validateFormat(actionDoc);
      if (!formatValidationResult.valid)
      {
        ESP_COMMANDER_LOG(LibraryErrors::INVALID_FORMAT);
        return;
      }

      ArduinoJson::JsonObject parametersJson = actionDoc["parameters"].as<ArduinoJson::JsonObject>();
      SleepyAction *action = nullptr;
      HandlerValue handlerParameters[ESP_COMMANDER_MAX_PARAMETERS];

      for (auto &item : actions)
      {
        if (strcmp(item.name, formatValidationResult.name) == 0)
        {
          action = &item;
          break;
        }
      }
      if (!action)
      {
        ESP_COMMANDER_LOG(LibraryErrors::ACTION_NOT_FOUND);
        return;
      }

      for (size_t i = 0; i < action->parameters.size(); i++)
      {
        bool valid = action->parameters[i].validateParameter(parametersJson, handlerParameters[i]);
        if (!valid)
        {
          ESP_COMMANDER_LOG(LibraryErrors::INVALID_PARAMETERS);
          return;
        }
      }

      ESP_COMMANDER_LOG("Handler begin");
      action->handler(handlerParameters, error);
      ESP_COMMANDER_LOG("Handler end");
      if (error.has_value())
      {
        ESP_COMMANDER_DEBUG("Action handler error");
        return;
      }
    }
  };
}