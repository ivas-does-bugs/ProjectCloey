#include <lvgl.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// Network credentials
const char* ssid = "test";
const char* password = "miawmiaw";

// Web server instance
AsyncWebServer server(80);

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

// (Optional) Placeholders for LVGL UI elements
lv_obj_t *style_label_text;
lv_obj_t *change_label_text;
const char *style_options[] = {"Casual", "Formal", "Sport"};
const char *change_options[] = {"All", "Shirt", "Pants", "Shoes"};

// Utility function to load a file's contents from the SD card
String loadFile(const char* path) {
  String content = "";
  File file = SD.open(path);
  if (file) {
    while (file.available()) {
      content += (char)file.read();
    }
    file.close();
  }
  return content;
}

// Handle file upload
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // On first chunk, create/truncate the file
  if (index == 0) {
    Serial.println("Upload start: " + filename);
    File uploadFile = SD.open("/" + filename, FILE_WRITE);
    if (uploadFile) {
      uploadFile.close();
    } else {
      Serial.println("Error opening file for writing");
    }
  }
  
  // Append the data to the file
  File uploadFile = SD.open("/" + filename, FILE_APPEND);
  if (uploadFile) {
    uploadFile.write(data, len);
    uploadFile.close();
  }
  
  // On final chunk, send a response
  if (final) {
    request->send(200, "text/html", "<h1>File uploaded successfully!</h1><a href='/'>Go back</a>");
  }
}

// Handle file deletion
void handleFileDeletion(AsyncWebServerRequest *request) {
  if (request->hasParam("filename", true)) {
    String filename = request->getParam("filename", true)->value();
    if (SD.exists(filename)) {
      SD.remove(filename);
      request->send(200, "text/html", "<h1>File deleted successfully!</h1><a href='/'>Go back</a>");
    } else {
      request->send(404, "text/html", "<h1>File not found!</h1><a href='/'>Go back</a>");
    }
  } else {
    request->send(400, "text/html", "<h1>Filename not provided!</h1><a href='/'>Go back</a>");
  }
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println(WiFi.localIP());

  // Initialize the SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Serve index.html at the root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String content = loadFile("/index.html");
    if (content.length() > 0) {
      request->send(200, "text/html", content);
    } else {
      request->send(500, "text/plain", "Error loading index.html");
    }
  });

  // Serve style.css
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    String content = loadFile("/style.css");
    if (content.length() > 0) {
      request->send(200, "text/css", content);
    } else {
      request->send(500, "text/plain", "Error loading style.css");
    }
  });

  // Serve scripts.js
  server.on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest *request){
    String content = loadFile("/scripts.js");
    if (content.length() > 0) {
      request->send(200, "application/javascript", content);
    } else {
      request->send(500, "text/plain", "Error loading scripts.js");
    }
  });

  // Additionally, serve any SD card files under the virtual URL /cardSD
  server.serveStatic("/cardSD", SD, "/");

  // File upload endpoint
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<h1>Upload complete!</h1>");
  }, handleFileUpload);

  // File deletion endpoint
  server.on("/delete", HTTP_POST, handleFileDeletion);

  // Start the server
  server.begin();
}

void loop() {
  // Nothing required here with AsyncWebServer
}
