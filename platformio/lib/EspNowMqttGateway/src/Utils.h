
#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "Config.h"

namespace EspNowMqttGateway
{
  static bool isHexChar(char c)
  {
    return (c >= '0' && c <= '9') ||
           (c >= 'a' && c <= 'f') ||
           (c >= 'A' && c <= 'F');
  }

  bool isValidEspNowKey(const char *key)
  {
    if (!key)
      return false;
    if (strlen(key) != 32)
      return false;
    for (size_t i = 0; i < 32; i++)
      if (!isHexChar(key[i]))
        return false;
    return true;
  }

  bool keyHexToBytes(const char *hex, uint8_t key[16])
  {
    if (!isValidEspNowKey(hex))
      return false;
    for (int i = 0; i < 16; i++)
    {
      char tmp[3] = {hex[i * 2], hex[i * 2 + 1], 0};
      key[i] = strtoul(tmp, NULL, 16);
    }
    return true;
  }

  bool isValidMacAddress(const char *mac)
  {
    if (!mac)
      return false;

    if (strlen(mac) != 17)
      return false;

    for (int i = 0; i < 17; i++)
    {
      if ((i + 1) % 3 == 0)
      {
        if (mac[i] != ':')
          return false;
      }
      else
      {
        if (!isHexChar(mac[i]))
          return false;
      }
    }

    return true;
  }

  bool macStringToBytes(const char *macStr, uint8_t mac[6])
  {
    if (!isValidMacAddress(macStr))
      return false;

    for (int i = 0; i < 6; i++)
    {
      mac[i] = strtoul(macStr + (i * 3), NULL, 16);
    }

    return true;
  }
}