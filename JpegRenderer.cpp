#include "JpegRenderer.h"

JpegRenderer::JpegRenderer(TFT_eSPI& tft) : tft(tft) {}

bool JpegRenderer::render(const char* filename, int xpos, int ypos) {
    File jpegFile = SD.open(filename, FILE_READ);
    if (!jpegFile) {
        // Optionally handle error without flooding serial output.
        return false;
    }
    
    if (!JpegDec.decodeSdFile(jpegFile)) {
        // Optionally add error handling here.
        return false;
    }
    
    // Optionally, you can have a separate method for logging that’s enabled only in debug mode.
    jpegRender(xpos, ypos);
    return true;
}

void JpegRenderer::jpegRender(int xpos, int ypos) {
    // Simplified version without extra Serial prints:
    uint16_t *pImg;
    uint16_t mcu_w = JpegDec.MCUWidth;
    uint16_t mcu_h = JpegDec.MCUHeight;
    uint32_t max_x = JpegDec.width + xpos;
    uint32_t max_y = JpegDec.height + ypos;

    bool swapBytes = tft.getSwapBytes();
    tft.setSwapBytes(true);

    uint32_t min_w = (JpegDec.width % mcu_w) ? (JpegDec.width % mcu_w) : mcu_w;
    uint32_t min_h = (JpegDec.height % mcu_h) ? (JpegDec.height % mcu_h) : mcu_h;

    while (JpegDec.read()) {
        pImg = JpegDec.pImage;
        int mcu_x = JpegDec.MCUx * mcu_w + xpos;
        int mcu_y = JpegDec.MCUy * mcu_h + ypos;

        uint32_t win_w = ((mcu_x + mcu_w) <= max_x) ? mcu_w : min_w;
        uint32_t win_h = ((mcu_y + mcu_h) <= max_y) ? mcu_h : min_h;

        if ((mcu_x + win_w) <= tft.width() && (mcu_y + win_h) <= tft.height())
            tft.pushImage(mcu_x, mcu_y, win_w, win_h, pImg);
        else if ((mcu_y + win_h) >= tft.height())
            JpegDec.abort();
    }
    
    tft.setSwapBytes(swapBytes);
}
