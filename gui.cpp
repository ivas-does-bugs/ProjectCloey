#include "gui.h"

GUI::GUI(TFT_eSPI& tft)
    : tft(tft), jpegRenderer(tft) {}

void GUI::setImage(const char* imagePath) {
        currentImagePath = imagePath;
        Serial.print("Current path:");
        Serial.println(currentImagePath);
        drawImage();  // Redraw only if image changed
}

void GUI::drawUI(int style_index, int change_index) {
    // Draw image only if it's the first time or changed externally
    if (currentStyleIndex == -1 && currentChangeIndex == -1) {
        drawImage();  // Only once during init
    }

    // Redraw bottom bar only if values changed
    if (style_index != currentStyleIndex || change_index != currentChangeIndex) {
        currentStyleIndex = style_index;
        currentChangeIndex = change_index;
        drawBottomText();
    }
}

void GUI::drawGenerating() {
    int boxWidth = 200;
    int boxHeight = 50;
    int boxX = (tft.width() - boxWidth) / 2;
    int boxY = (tft.height() - boxHeight) / 2;
    
    tft.fillRect(boxX, boxY, boxWidth, boxHeight, TFT_WHITE);
    tft.drawRect(boxX, boxY, boxWidth, boxHeight, TFT_BLACK);
    
    tft.setCursor(boxX + 10, boxY + 20);
    tft.println("Generating...");
    tft.setCursor(boxX + 10, boxY + 40);
    tft.println("Please wait");
}

void GUI::drawImage() {
    tft.fillScreen(TFT_BLACK);
    jpegRenderer.render(currentImagePath, 0, 0);
    drawBottomText();
}

void GUI::drawBottomText() {
    tft.fillRect(0, 270, 240, 50, TFT_LIGHTGREY);  // Clear bottom bar

    tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
    tft.setFreeFont(&FreeMono9pt7b);
    
    tft.setCursor(10, 290);
    tft.print("Style: ");
    tft.setCursor(10, 310);
    tft.println(style_options[currentStyleIndex]);
    
    tft.setCursor(130, 290);
    tft.print("Change: ");
    tft.setCursor(130, 310);
    tft.println(change_options[currentChangeIndex]);
}
