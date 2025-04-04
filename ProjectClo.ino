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

// Create placeholders for UI elements
lv_obj_t *style_label_text;
lv_obj_t *change_label_text;

// Options arrays
const char *style_options[] = {"Casual", "Formal", "Sport"};
const char *change_options[] = {"All", "Shirt", "Pants", "Shoes"};

// HTML template for the file manager
const char html[] PROGMEM = R"rawliteral(
  <!DOCTYPE HTML><html>
  <head>
    <title>SD Card File Manager</title>
  </head>
  <body>
    <h1>SD Card File Manager</h1>
    <h2>Upload File</h2>
    <form action="/upload" method="POST" enctype="multipart/form-data">
      <input type="file" name="fileToUpload">
      <input type="submit" value="Upload">
    </form>
    <h2>Delete File</h2>
    <form action="/delete" method="POST">
      <input type="text" name="filename" placeholder="Enter filename to delete">
      <input type="submit" value="Delete">
    </form>
    <h2>Files on SD Card:</h2>
    <ul>
      %FILES_LIST%
    </ul>
  </body>
  </html>
)rawliteral";

// Function to get a list of files on the SD card
String listFiles() {
  String fileList = "";
  File root = SD.open("/");
  while (true) {
    File file = root.openNextFile();
    if (!file) break;
    fileList += "<li>" + String(file.name()) + "</li>";
    file.close();
  }
  return fileList;
}

// Handle file upload
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) { // Start of the file upload
    Serial.println("Upload start: " + filename);
    File uploadFile = SD.open("/" + filename, FILE_WRITE);
    if (uploadFile) {
      uploadFile.close();
    } else {
      Serial.println("Error opening file for writing");
    }
  }

  // Write data to SD card
  File uploadFile = SD.open("/" + filename, FILE_WRITE);
  if (uploadFile) {
    uploadFile.write(data, len);
    uploadFile.close();
  }

  // If this is the last part of the upload, confirm the upload
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
  // Initialize Serial monitor
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  Serial.println(WiFi.localIP());

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("Initialization failed!");
    return;
  }
  Serial.println("SD card initialized.");

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String htmlContent = html;
    htmlContent.replace("%FILES_LIST%", listFiles());
    request->send(200, "text/html", htmlContent);
  });

  // Handle file upload
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", "<h1>Upload complete!</h1>");
  }, handleFileUpload);

  // Handle file deletion
  server.on("/delete", HTTP_POST, handleFileDeletion);

  // Start the web server
  server.begin();
}

void loop() {
  // Nothing to do here
}
