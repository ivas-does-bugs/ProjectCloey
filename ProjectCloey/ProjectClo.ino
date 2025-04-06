#include <WiFi.h>
#include <HTTPClient.h>
#include <ESPAsyncWebServer.h>
#include <SD.h>
#include <SPI.h>

#include "GUI.h"
#include "pins.h"
#include "io.h"

TFT_eSPI tft = TFT_eSPI();
GUI gui(tft);             

AsyncWebServer server(80);

int last_style_index = -1;
int last_change_index = -1;

const char* ssid = "test";
const char* password = "miawmiaw";

// Button state tracking
bool lastButtonState = HIGH;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  init_inputs();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected:");
  Serial.println(WiFi.localIP());

  if (!SD.begin(SD_CS)) {
    Serial.println("SD card failed!");
    return;
  }

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  gui.drawUI(0, 0);
  last_style_index = 0;
  last_change_index = 0;

  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "Upload OK");
  }, handleUpload);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "ESP32 Image Receiver Ready");
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  int pot1 = analogRead(POT1_PIN);
  int pot2 = analogRead(POT2_PIN);
  Serial.println(pot1);
  Serial.println(pot2);

  int style_index = map(pot1, 0, 4095, 0, 2);
  int change_index = map(pot2, 0, 4095, 0, 2); 
  

  if (style_index != last_style_index || change_index != last_change_index) {
    gui.drawUI(style_index, change_index);
    last_style_index = style_index;
    last_change_index = change_index;
  }

  bool buttonState = digitalRead(BUTT_PIN);
  if (buttonState == LOW && lastButtonState == HIGH) {
    Serial.println("Button was pressed");
    gui.drawGenerating();
    sendRequestToServer();
  }
  lastButtonState = buttonState;

  delay(50);
}


void sendRequestToServer() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Replace with the actual IP address and port of your Flask server.
    http.begin("http://10.48.162.95:5000/generate-image");

    // Set header to indicate JSON payload
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"change_index\": " + String(last_change_index) + ", \"style_index\": " + String(last_style_index) + "}";
    // Send POST request with the JSON body
    int httpResponseCode = http.POST(jsonPayload);  // Send the change_index as part of the body

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    // Print response body for debugging
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: ");
      Serial.println(response);
    } else {
      Serial.println("Error on sending POST request");
    }

    http.end();
  }
}


void handleUpload(AsyncWebServerRequest *request, String filename, size_t index,
                  uint8_t *data, size_t len, bool final) {
  static File uploadFile;

  if (index == 0) {
    // Ensure the filename starts with a slash
    if (!filename.startsWith("/")) {
      filename = "/" + filename;
    }
    
    Serial.printf("Upload Start: %s\n", filename.c_str());
    
    // Delete any previous file with the same name.
    if (SD.exists(filename.c_str())) {
      SD.remove(filename.c_str());
      Serial.println("Previous file deleted.");
    }
    
    // Open the file for writing
    uploadFile = SD.open(filename.c_str(), FILE_WRITE);
    if (!uploadFile) {
      Serial.println("Failed to open file for writing.");
      return;
    }
  }

  if (uploadFile) {
    size_t written = uploadFile.write(data, len);
    Serial.printf("Received chunk: index=%d, len=%d, written=%d\n", index, len, written);
  }

  if (final) {
    if (uploadFile) {
      uploadFile.flush();
      uploadFile.close();
      Serial.println("Upload finished");
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      Serial.println(filename);
      gui.setImage(filename.c_str());
      
    }
  }
}
