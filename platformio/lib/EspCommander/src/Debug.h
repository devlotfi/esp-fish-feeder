#pragma once

#include <iostream>

#ifdef ESP_COMMANDER_DEBUG
#define ESP_COMMANDER_LOG(x)     \
  do                             \
  {                              \
    std::cout << x << std::endl; \
  } while (0)
#else
#define ESP_COMMANDER_LOG(x)
#endif