#include <Arduino.h>

#define log(x, msg, ...) \
  Serial.printf("[%d] " msg "\n", x, __VA_ARGS__)