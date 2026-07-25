idf_component_register(
  SRCS "main.cpp"
  INCLUDE_DIRS "../include"
  REQUIRES
    adafruit-neopixel
    etl
    esp32-servo
    esp-commander
    esp-now-mqtt-gateway
)
