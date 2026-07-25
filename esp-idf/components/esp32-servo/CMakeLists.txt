idf_component_register(
  SRCS 
    "src/ESP32PWM.cpp"
    "src/ESP32Servo.cpp"
  INCLUDE_DIRS "src"
  REQUIRES espressif__arduino-esp32
)
