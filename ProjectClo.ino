#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>

#include "GUI.h"
#include "pins.h"
#include "io.h"

// // Static IP Configuration for the hotspot network
// IPAddress local_IP(10, 0, 0, 10);  // Set a static IP in the same range as your hotspot
// IPAddress gateway(10, 0, 0, 1);     // Hotspot gateway (typically 10.0.0.1)
// IPAddress subnet(255, 255, 255, 0);    // Subnet mask

TFT_eSPI tft = TFT_eSPI();
GUI gui(tft);             

AsyncWebServer server(80);

int last_style_index = -1;
int last_change_index = -1;

const char* ssid = "test";  // Your hotspot SSID
const char* password = "miawmiaw";  // Your hotspot password

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_inputs();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected: ");
  Serial.println(WiFi.localIP());  

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD card failed!");
    return;
  }

  // Initialize display
  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  // Draw the initial UI
  gui.drawUI(0, 0);
  last_style_index = 0;
  last_change_index = 0;

  // Set up the upload endpoint
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Upload OK");
  }, handleUpload);

  // Serve static files
  server.serveStatic("/server.jpg", SD, "/server.jpg");

  // Basic route
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "ESP32 Image Receiver Ready");
  });

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);

  int style_index = map(pot1, 0, 4095, 0, 2);
  int change_index = map(pot2, 0, 4095, 0, 3);

  if (style_index != last_style_index || change_index != last_change_index) {
    gui.drawUI(style_index, change_index);
    last_style_index = style_index;
    last_change_index = change_index;
  }

  delay(20);
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index,
                  uint8_t *data, size_t len, bool final) {
  static File uploadFile;

  if (index == 0) {
    Serial.printf("Upload Start: %s\n", filename.c_str());
    uploadFile = SD.open("/server.jpg", FILE_WRITE);
  }

  if (uploadFile) {
    uploadFile.write(data, len);
  }

  if (final) {
    if (uploadFile) {
      uploadFile.close();
      Serial.println("Upload finished");
      gui.setImage("/server.jpg"); // Update display with the new image
    }
  }
}
