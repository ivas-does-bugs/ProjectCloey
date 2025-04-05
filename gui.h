#ifndef GUI_H
#define GUI_H

#include <TFT_eSPI.h>
#include <JPEGDecoder.h>

class GUI {
public:
    GUI(TFT_eSPI& tft);
    
    void draw_ui(int style_index, int change_index);
    void drawSdJpeg(const char *filename, int xpos, int ypos);
    void jpegRender(int xpos, int ypos);
    void jpegInfo();
    
    static int last_style_index;
    static int last_change_index;
    
private:
    TFT_eSPI& tft;
    const char* style_options[3] = {"Casual", "Formal", "Sport"};
    const char* change_options[4] = {"All", "Shirt", "Pants", "Shoes"};
};

#endif // GUI_H
