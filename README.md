<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/github-banner.png">

# 📜 esp-fish-feeder

The goal of this project is to create an automatic smart fish feeder

## Features

- Feeds fish every 24h or can be triggered manually
- Change color of LED

# 📌 Contents

- [Tech stack](#tech-stack)
- [Build](#build)
  - [Components](#components)
  - [Libraries](#libraries)
  - [Configuration](#configuration)
  - [Images](#images)

# Tech stack

<p float="left">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/cpp.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/arduino.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/espressif.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/esp-commander.svg">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/esp-now-mqtt-gateway.svg">
</p>

## Diagrams

<p float="left">
  <img height="50px" src="https://devlotfi.github.io/stack-icons/icons/fritzing.svg">
</p>

# Build

## Components

- ESP32C3
- SG90 (Servo)
- 3x 470μf 25V capacitors
- Cables
- Case (Optional)
- Food Container

<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/fritzing.png">

## Libraries

- ArduinoJson
- Embedded Template Library ETL
- PubSubClient
- [EspCommander](https://github.com/devlotfi/esp-commander)
- [EspNowMqttGateway](https://github.com/devlotfi/esp-now-mqtt-gateway)

## Configuration

Change settings in [Properties.h](https://github.com/devlotfi/esp-fish-feeder/blob/master/arduino/Properties.h)

## Images

Some images of the final build

<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/build-1.jpg">
<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/build-2.jpg">
