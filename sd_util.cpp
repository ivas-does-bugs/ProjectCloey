#include "sd_util.h"

bool init_sd(int cs_pin) {
    if (!SD.begin(cs_pin)) {
      Serial.println("Initialization failed!");
      return false;
    }
  
    Serial.println("SD card initialized.");
    return true;
  }