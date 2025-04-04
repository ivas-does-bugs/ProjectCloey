#ifndef GUI_H
#define GUI_H

#include "pins.h"
#include <lvgl.h>
#include <TFT_eSPI.h>

void lv_create_main_gui();
void update_labels_from_pots();

#endif