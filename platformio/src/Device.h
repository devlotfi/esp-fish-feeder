#pragma once

#include <etl/variant.h>
#include <stdint.h>
#include <EspCommander.h>
#include "Properties.h"
#include "Vars.h"
#include "Led.h"
#include "Feed.h"

EspCommander::Value rgbLedQueryResults[] = {
    EspCommander::Value(EspCommander::Value::Params{
        .name = "color",
        .type = EspCommander::ValueType::COLOR,
        .required = true,
    }),
    EspCommander::Value(EspCommander::Value::Params{
        .name = "brightness",
        .type = EspCommander::ValueType::RANGE,
        .required = true,
        .min = 0,
        .max = 255,
    }),
};
auto rgbLedQuery = EspCommander::Query(EspCommander::Query::Params{
    .name = "RGB Led",
    .results = rgbLedQueryResults,
    .handler = [](EspCommander::HandlerValue results[], etl::optional<const char *> &error)
    {
        results[0] = (const char *)currentColor;
        results[1] = (int)currentBrightness;
    },
});

static char lastFeedStr[32];
EspCommander::Value lastFishFeedQueryResults[] = {
    EspCommander::Value(EspCommander::Value::Params{
        .name = "last fed",
        .type = EspCommander::ValueType::STRING,
        .required = true,
    }),
};
auto lastFishFeedQuery = EspCommander::Query(EspCommander::Query::Params{
    .name = "Last fed",
    .results = lastFishFeedQueryResults,
    .handler = [](EspCommander::HandlerValue results[], etl::optional<const char *> &error)
    {
        FeedConfig conf = loadFeedConfig();
        if (conf.lastFeedTime == 0)
        {
            strcpy(lastFeedStr, "never");
        }
        else
        {
            time_t t = conf.lastFeedTime;
            struct tm *tm_info = localtime(&t);
            strftime(lastFeedStr, sizeof(lastFeedStr), "%Y-%m-%d %H:%M:%S", tm_info);
        }
        results[0] = (const char *)lastFeedStr;
    },
});

EspCommander::Value setRgbLedActionParameters[] = {
    EspCommander::Value(EspCommander::Value::Params{
        .name = "color",
        .type = EspCommander::ValueType::COLOR,
        .required = true,
    }),
    EspCommander::Value(EspCommander::Value::Params{
        .name = "brightness",
        .type = EspCommander::ValueType::RANGE,
        .required = true,
        .min = 0,
        .max = 255,
    }),
};
auto setRgbLedAction = EspCommander::Action(EspCommander::Action::Params{
    .name = "set RGB LED",
    .parameters = setRgbLedActionParameters,
    .handler = [](EspCommander::HandlerValue parameters[], EspCommander::HandlerValue results[], etl::optional<const char *> &error)
    {
        const char *colorParam = etl::get<const char *>(parameters[0].value());
        int brightnessParam = etl::get<int>(parameters[1].value());

        strncpy(currentColor, colorParam, sizeof(currentColor) - 1);
        currentColor[sizeof(currentColor) - 1] = '\0';
        currentBrightness = brightnessParam;

        Serial.println(currentColor);
        Serial.println(currentBrightness);

        LedConfig ledConfig = {};
        ledConfig.brightness = currentBrightness;
        strncpy(ledConfig.color, currentColor, sizeof(ledConfig.color) - 1);
        ledConfig.color[sizeof(ledConfig.color) - 1] = '\0';
        saveConfig(ledConfig);
        setLed(colorParam, (uint8_t)currentBrightness);
    },
});

auto feedFishAction = EspCommander::Action(EspCommander::Action::Params{
    .name = "feed fish",
    .handler = [](EspCommander::HandlerValue parameters[], EspCommander::HandlerValue results[], etl::optional<const char *> &error)
    {
        pendingFeed = true;
    },
});

static uint8_t requestBuffer[ESP_COMMANDER_JSON_BUFFER_SIZE];
static uint8_t responseBuffer[ESP_COMMANDER_JSON_BUFFER_SIZE];
EspCommander::StaticBufferAllocator requestAllocator(requestBuffer, sizeof(requestBuffer));
EspCommander::StaticBufferAllocator responseAllocator(responseBuffer, sizeof(responseBuffer));

EspCommander::Query queries[] = {
    rgbLedQuery,
    lastFishFeedQuery,
};
EspCommander::Action actions[] = {
    setRgbLedAction,
    feedFishAction,
};
auto device = EspCommander::Device(EspCommander::Device::Params{
    .id = device_id,
    .name = device_name,
    .requestTopic = mqtt_request_topic,
    .responseTopic = mqtt_response_topic,
    .queries = queries,
    .actions = actions,
    .requestAllocator = requestAllocator,
    .responseAllocator = responseAllocator,
});