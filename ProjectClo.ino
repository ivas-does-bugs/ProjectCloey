#include <TFT_eSPI.h>
#include "pins.h"
#include "sd_util.h"
#include "io.h"
#include "gui.h"

TFT_eSPI tft = TFT_eSPI();  // Create display object
GUI gui(tft);  // Create GUI object

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_inputs();
  if (!init_sd(SD_CS)) return;

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);

  gui.draw_ui(0, 0);  // Initial draw of the UI
}

void loop() {
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  int style_index = map(pot1, 0, 4095, 0, 2);
  int change_index = map(pot2, 0, 4095, 0, 3);

  // Only redraw if the index changes
  if (style_index != GUI::last_style_index || change_index != GUI::last_change_index) {
    gui.draw_ui(style_index, change_index);
    GUI::last_style_index = style_index;
    GUI::last_change_index = change_index;
  }

  delay(100);  // Add a small delay for smoother updates
}
