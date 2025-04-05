#include "gui.h"
#include <SD.h>
#include "pins.h"
#include "sd_util.h"
#include "io.h"
#include <JPEGDecoder.h>

#define LOAD_GFXFF
#define GFXFF 1
#define GLCD  0
#define FONT2 2
#define FONT4 4
#define FONT6 6
#define FONT7 7
#define FONT8 8

int GUI::last_style_index = -1;
int GUI::last_change_index = -1;

GUI::GUI(TFT_eSPI& tft) : tft(tft) {}

void GUI::drawSdJpeg(const char *filename, int xpos, int ypos) {
  File jpegFile = SD.open(filename, FILE_READ);  // Open the file from the SD card
  
  if (!jpegFile) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  Serial.println("===========================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("===========================");

  bool decoded = JpegDec.decodeSdFile(jpegFile);  // Decode JPEG file

  if (decoded) {
    jpegInfo();  // Print information about the image
    jpegRender(xpos, ypos);  // Render the image onto the screen
  } else {
    Serial.println("Jpeg file format not supported!");
  }
}

void GUI::jpegRender(int xpos, int ypos) {
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  bool swapBytes = tft.getSwapBytes();
  tft.setSwapBytes(true);

  uint32_t min_w = jpg_min(mcu_w, max_x % mcu_w);
  uint32_t min_h = jpg_min(mcu_h, max_y % mcu_h);

  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  uint32_t drawTime = millis();
  max_x += xpos;
  max_y += ypos;

  while (JpegDec.read()) {
    pImg = JpegDec.pImage;

    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    if (win_w != mcu_w) {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++) {
        p += mcu_w;
        for (int w = 0; w < win_w; w++) {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    uint32_t mcu_pixels = win_w * win_h;

    if ((mcu_x + win_w) <= tft.width() && (mcu_y + win_h) <= tft.height())
      tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
    else if ((mcu_y + win_h) >= tft.height())
      JpegDec.abort();
  }

  tft.setSwapBytes(swapBytes);
}

void GUI::jpegInfo() {
  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print("Width      :");
  Serial.println(JpegDec.width);
  Serial.print("Height     :");
  Serial.println(JpegDec.height);
  Serial.print("Components :");
  Serial.println(JpegDec.comps);
  Serial.print("MCU / row  :");
  Serial.println(JpegDec.MCUSPerRow);
  Serial.print("MCU / col  :");
  Serial.println(JpegDec.MCUSPerCol);
  Serial.print("Scan type  :");
  Serial.println(JpegDec.scanType);
  Serial.print("MCU width  :");
  Serial.println(JpegDec.MCUWidth);
  Serial.print("MCU height :");
  Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
}

void GUI::draw_ui(int style_index, int change_index) {
  drawSdJpeg("/gavan.jpg", 0, 0);   
  tft.fillRect(0, 270, 240, 50, TFT_LIGHTGREY);  // Bottom bar

  tft.setTextColor(TFT_BLACK, TFT_LIGHTGREY);
  tft.setFreeFont(&FreeMono9pt7b);

  tft.setCursor(10, 290);  // X, Y position for style label
  tft.print("Style: ");
  tft.setCursor(10, 310);  // Position for style value
  tft.println(style_options[style_index]);

  tft.setCursor(130, 290);  // X, Y position for change label
  tft.print("Change: ");
  tft.setCursor(130, 310);  // Position for change value
  tft.println(change_options[change_index]);
}
