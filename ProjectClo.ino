#include <TFT_eSPI.h>
#include "GUI.h"
#include "pins.h"
#include "sd_util.h"
#include "io.h"

TFT_eSPI tft = TFT_eSPI();  // Create display object
GUI gui(tft);              // Create GUI object

// Local state variables for tracking the last drawn indexes.
int last_style_index = -1;
int last_change_index = -1;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_inputs();
  if (!init_sd(SD_CS)) return;

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // Initial UI draw.
  gui.drawUI(0, 0);
  last_style_index = 0;
  last_change_index = 0;
}

void loop() {
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  // Map potentiometer readings to indices.
  int style_index = map(pot1, 0, 4095, 0, 2);
  int change_index = map(pot2, 0, 4095, 0, 3);

  // Redraw the UI only if the state changes.
  if (style_index != last_style_index || change_index != last_change_index) {
    gui.drawUI(style_index, change_index);
    last_style_index = style_index;
    last_change_index = change_index;
  }

  delay(20);  // Small delay for smoother updates.
}
