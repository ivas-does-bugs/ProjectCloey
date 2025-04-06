#ifndef GUI_H
#define GUI_H

#include <TFT_eSPI.h>
#include "JpegRenderer.h"

#define LOAD_GFXFF
#define GFXFF 1
#define GLCD  0
#define FONT2 2
#define FONT4 4
#define FONT6 6
#define FONT7 7
#define FONT8 8

class GUI {
public:
    GUI(TFT_eSPI& tft);

    void drawUI(int style_index, int change_index);
    void setImage(const char* imagePath); 
    void drawGenerating();
    
private:
    TFT_eSPI& tft;
    JpegRenderer jpegRenderer;

    const char* style_options[3] = {"Casual", "Formal", "Sport"};
    const char* change_options[4] = {"All", "Shirt", "Pants"};

    void drawImage();     
    void drawBottomText();

    const char* currentImagePath = "/Andra.jpg";
    int currentStyleIndex = -1;
    int currentChangeIndex = -1;
};

#endif // GUI_H
