#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

// WiFi credentials
const char* ssid = "test";
const char* password = "miawmiaw";

// SD card pin definitions
#define SD_CS    5    
#define SD_MOSI  23   
#define SD_MISO  19   
#define SD_SCK   18   

// Web server
AsyncWebServer server(80);
File uploadFile; // Global file for upload session

// HTML template
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
  %SD_ERROR%
</body>
</html>
)rawliteral";

String sdError = "";

// List files on SD card
String listFiles() {
  String fileList = "";
  File root = SD.open("/");
  if (!root) {
    return "<li>Error opening root directory.</li>";
  }
  File file = root.openNextFile();
  while (file) {
    fileList += "<li>" + String(file.name()) + " (" + file.size() + " bytes)</li>";
    file = root.openNextFile();
  }
  return fileList;
}

// Handle file upload
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) {
    Serial.println("Upload started: " + filename);
    if (SD.exists("/" + filename)) {
      SD.remove("/" + filename);
    }
    uploadFile = SD.open("/" + filename, FILE_WRITE);
    if (!uploadFile) {
      Serial.println("Failed to open file for writing");
      return;
    }
  }

  if (uploadFile) {
    uploadFile.write(data, len);
  }

  if (final) {
    if (uploadFile) {
      uploadFile.close();
    }
    Serial.println("Upload complete: " + filename);
    request->send(200, "text/html", "<h1>File uploaded successfully!</h1><a href='/'>Go back</a>");
  }
}

// Handle file deletion
void handleFileDeletion(AsyncWebServerRequest *request) {
  if (request->hasParam("filename", true)) {
    String filename = request->getParam("filename", true)->value();
    if (SD.exists(filename)) {
      SD.remove(filename);
      Serial.println("Deleted: " + filename);
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
  Serial.println("Connected. IP address: " + WiFi.localIP().toString());

  // Init SD card
  if (!SD.begin(SD_CS)) {
    sdError = "<p><b>SD card initialization failed!</b></p>";
    Serial.println("SD initialization failed!");
  } else {
    sdError = "<p><b>SD card initialized successfully!</b></p>";
    Serial.println("SD card initialized.");
  }

  // Main page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String htmlContent = html;
    htmlContent.replace("%FILES_LIST%", listFiles());
    htmlContent.replace("%SD_ERROR%", sdError);
    request->send(200, "text/html", htmlContent);
  });

  // File upload handler
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request){
    // nothing here, handled by upload callback
  }, handleFileUpload);

  // File deletion handler
  server.on("/delete", HTTP_POST, handleFileDeletion);

  // Start server
  server.begin();
}

void loop() {
  // Nothing here
}
