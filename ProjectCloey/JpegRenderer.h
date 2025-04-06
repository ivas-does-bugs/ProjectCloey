#ifndef JPEG_RENDERER_H
#define JPEG_RENDERER_H

#include <TFT_eSPI.h>
#include <JPEGDecoder.h>
#include <SD.h>

class JpegRenderer {
public:
    JpegRenderer(TFT_eSPI& tft);
    bool render(const char* filename, int xpos, int ypos);
    
private:
    TFT_eSPI& tft;
    void jpegRender(int xpos, int ypos);
};

#endif 