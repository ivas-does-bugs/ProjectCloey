#include <lvgl.h>
#include <TFT_eSPI.h>

#include "pins.h"
#include "gui.h"
#include "sd_util.h"
#include "io.h"

// Screen settings
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_inputs();

  if (!init_sd(SD_CS)) return;

  lv_init();
  lv_log_register_print_cb(log_print);

  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);

  lv_create_main_gui();
}

void loop() {
  lv_task_handler();
  lv_tick_inc(5);
  delay(5);

  update_labels_from_pots();
}
