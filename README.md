<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/github-banner.png">

# 📜 esp-fish-feeder

The goal of this project is to create an automatic smart fish feeder

## Features

- Feeds fish every 48h or can be triggered manually
- Change color of LED

# 📌 Contents

- [Tech stack](#tech-stack)
- [Building the media controller](#building-the-media-controller)
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

# Building the Media Controller

## Components

- ESP32C3
- SG90 (Servo)
- 3xAA Batteries
- Cables
- Case (Optional)
- Food Container

<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/fritzing.png">

## Libraries

- ArduinoJson
- Embedded Template Library ETL
- PubSubClient
- [IotCommander](https://github.com/devlotfi/iot-commander)

## Configuration

Change settings in [Properties.h](https://github.com/devlotfi/esp-fish-feeder/blob/master/arduino/Properties.h)

The baord relied on an MQTT communication uisng [IotCommander](https://github.com/devlotfi/iot-commander)

## Images

Some images of the final build

<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/build-1.jpg">
<img src="https://raw.githubusercontent.com/devlotfi/esp-fish-feeder/master/github-assets/build-2.jpg">
