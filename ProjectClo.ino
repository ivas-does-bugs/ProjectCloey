#include <lvgl.h>
#include <TFT_eSPI.h>

#include <SPI.h>
#include <SD.h>

// SD card pins
#define SD_CS    5    
#define SD_MOSI  23   
#define SD_MISO  19   
#define SD_SCK   18   

// Potentiometer pins
#define POT1_PIN 33
#define POT2_PIN 33

// Screen settings
#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320
#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];


lv_obj_t *style_label_text;
lv_obj_t *change_label_text;

// Options arrays
const char *style_options[] = {"Casual", "Formal", "Sport"};
const char *change_options[] = {"All", "Shirt", "Pants", "Shoes"};

// Logging for debugging
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// === GUI ===
void lv_create_main_gui(void) {

  //Set bg color white
  lv_obj_t *scr = lv_scr_act(); 
  lv_obj_set_style_bg_color(scr, lv_color_hex(0x000000), 0);

  // Bottom bar
  lv_obj_t *bottom_bar = lv_obj_create(lv_scr_act());
  lv_obj_set_size(bottom_bar, SCREEN_WIDTH, 50);
  lv_obj_align(bottom_bar, LV_ALIGN_BOTTOM_MID, 0, 0);
  lv_obj_set_style_bg_color(bottom_bar, lv_color_hex(0xADD8E6), 0); // Light blue
  lv_obj_set_style_pad_all(bottom_bar, 5, 0);
  lv_obj_set_layout(bottom_bar, LV_LAYOUT_FLEX);
  lv_obj_set_flex_flow(bottom_bar, LV_FLEX_FLOW_ROW);
  lv_obj_set_flex_align(bottom_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

  // Style section
  lv_obj_t *style_container = lv_obj_create(bottom_bar);
  lv_obj_remove_style_all(style_container);
  lv_obj_set_size(style_container, 110, LV_SIZE_CONTENT);
  lv_obj_set_layout(style_container, LV_LAYOUT_FLEX);

  lv_obj_t *style_label = lv_label_create(style_container);
  lv_label_set_text(style_label, "Style:");

  style_label_text = lv_label_create(style_container);
  lv_label_set_text(style_label_text, style_options[0]);  // Default to "Casual"

  // Change section
  lv_obj_t *change_container = lv_obj_create(bottom_bar);
  lv_obj_remove_style_all(change_container);
  lv_obj_set_size(change_container, 110, LV_SIZE_CONTENT);
  lv_obj_set_layout(change_container, LV_LAYOUT_FLEX);

  lv_obj_t *change_label = lv_label_create(change_container);
  lv_label_set_text(change_label, "Change:");

  change_label_text = lv_label_create(change_container);
  lv_label_set_text(change_label_text, change_options[0]);  // Default to "All"
}

// === Update Labels Based on Potentiometer ===

void update_labels_from_pots() {
  // Read potentiometers
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  // Map potentiometer values to array indices
  int style_index = map(pot1, 0, 4095, 0, 2);  // 0 to 2 for style
  int change_index = map(pot2, 0, 4095, 0, 3); // 0 to 3 for change

  // Update the label texts
  lv_label_set_text(style_label_text, style_options[style_index]);
  lv_label_set_text(change_label_text, change_options[change_index]);
}

void setup() {
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);

    // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  File myFile = SD.open("/example.txt", FILE_WRITE);
  if (myFile) {
    myFile.println("Hello, SD card!");
    myFile.close();
  } else {
    Serial.println("Error opening file.");
  }

  // Setup analog input pins
  pinMode(POT1_PIN, INPUT);
  pinMode(POT2_PIN, INPUT);
  
  // Start LVGL
  lv_init();
  lv_log_register_print_cb(log_print);

  // Init display
  lv_display_t *disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);

  // Create UI
  lv_create_main_gui();
}

void loop() {
  lv_task_handler();
  lv_tick_inc(5);
  delay(5);

  update_labels_from_pots();
}
