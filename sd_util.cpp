#include "sd_util.h"

bool init_sd(int cs_pin) {
    if (!SD.begin(cs_pin)) {
      Serial.println("Initialization failed!");
      return false;
    }
  
    Serial.println("SD card initialized.");
    File myFile = SD.open("/example.txt", FILE_WRITE);
    if (myFile) {
      myFile.println("Hello, SD card!");
      myFile.close();
    } else {
      Serial.println("Error opening file.");
    }
    return true;
  }