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
    %SD_ERROR%
  </body>
  </html>
)rawliteral";

// Variable to store SD card error message
String sdError = "";

// Function to get a list of files on the SD card
String listFiles() {
  String fileList = "";
  if (SD.exists("/")) {
    File root = SD.open("/");
    while (true) {
      File file = root.openNextFile();
      if (!file) break;
      fileList += "<li>" + String(file.name()) + "</li>";
      file.close();
    }
  }
  return fileList;
}

// Handle file upload
void handleFileUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (index == 0) { // Start of the file upload
    Serial.println("Upload start: " + filename);
    if (!SD.exists("/")) {
      Serial.println("Error: SD card not initialized.");
      return;
    }
    File uploadFile = SD.open("/" + filename, FILE_WRITE);
    if (uploadFile) {
      uploadFile.close();
    } else {
      Serial.println("Error opening file for writing");
    }
  }

  // Write data to SD card
  if (SD.exists("/")) {
    File uploadFile = SD.open("/" + filename, FILE_WRITE);
    if (uploadFile) {
      uploadFile.write(data, len);
      uploadFile.close();
    }
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
    sdError = "<p><b>SD card initialization failed!</b></p>";
    Serial.println("Initialization failed!");
  } else {
    sdError = "<p><b>SD card initialized successfully!</b></p>";
  }

  // Serve the HTML page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String htmlContent = html;
    htmlContent.replace("%FILES_LIST%", listFiles());
    htmlContent.replace("%SD_ERROR%", sdError);
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
