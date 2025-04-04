#include "gui.h"

// Global references to label objects
lv_obj_t *style_label_text;
lv_obj_t *change_label_text;

const char *style_options[] = {"Casual", "Formal", "Sport"};
const char *change_options[] = {"All", "Shirt", "Pants", "Shoes"};

void lv_create_main_gui() {
  lv_obj_t *scr = lv_scr_act(); 
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

  lv_obj_t *bottom_bar = lv_obj_create(scr);
  lv_obj_set_size(bottom_bar, 240, 50);
  lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(0xADD8E6), 0);
  lv_obj_set_style_pad_all(bottom_bar, 5, 0);
  lv_obj_set_layout(bottom_bar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  lv_obj_t *style_container = lv_obj_create(bottom_bar);
  lv_obj_remove_style_all(style_container);
  lv_obj_set_size(style_container, 110, LV_SIZE_CONTENT);
  lv_obj_set_layout(style_container, LV_LAYOUT_FLEX);

  lv_obj_t *style_label = lv_label_create(style_container);
  lv_label_set_text(style_label, "Style:");

  style_label_text = lv_label_create(style_container);
  lv_label_set_text(style_label_text, style_options[0]);

  lv_obj_t *change_container = lv_obj_create(bottom_bar);
  lv_obj_remove_style_all(change_container);
  lv_obj_set_size(change_container, 110, LV_SIZE_CONTENT);
  lv_obj_set_layout(change_container, LV_LAYOUT_FLEX);

  lv_obj_t *change_label = lv_label_create(change_container);
  lv_label_set_text(change_label, "Change:");

  change_label_text = lv_label_create(change_container);
  lv_label_set_text(change_label_text, change_options[0]);
}

void update_labels_from_pots() {
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  int style_index = map(pot1, 0, 4095, 0, 2);
  int change_index = map(pot2, 0, 4095, 0, 3);

  lv_label_set_text(style_label_text, style_options[style_index]);
  lv_label_set_text(change_label_text, change_options[change_index]);
}
