#include "gui.h"

GUI::GUI(TFT_eSPI& tft)
    : tft(tft), jpegRenderer(tft) {}

void GUI::drawUI(int style_index, int change_index) {
    // Clear or set the background as needed
    tft.fillScreen(TFT_BLACK);
    
    // Use the JPEG renderer to draw the background image.
    // Optionally check for success/failure without serial prints.
    jpegRenderer.render("/Andra.jpg", 0, 0);
    
    // Draw the bottom bar and text overlays.
    tft.fillRect(0, 270, 240, 50, TFT_LIGHTGREY);  // Bottom bar

    tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    tft.setFreeFont(&FreeMono9pt7b);
    
    tft.setCursor(10, 290);
    tft.print("Style: ");
    tft.setCursor(10, 310);
    tft.println(style_options[style_index]);
    
    tft.setCursor(130, 290);
    tft.print("Change: ");
    tft.setCursor(130, 310);
    tft.println(change_options[change_index]);
}
