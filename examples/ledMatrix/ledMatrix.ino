#include <Arduino.h>
#include <WiFiS3.h>
#include <Arduino_LED_Matrix.h>
#include <WebServer.h>
#include "secrets.h"
#include "index_htm.h"


const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
int status = WL_IDLE_STATUS;

WebServer server(80);
ArduinoLEDMatrix matrix;

void handleMood() {
  String mood = server.arg("mood");
  if (mood == "happy") {
    matrix.loadFrame(LEDMATRIX_EMOJI_HAPPY);
  } else if (mood == "sad") {
    matrix.loadFrame(LEDMATRIX_EMOJI_SAD);
  } else if (mood == "rock") {
    matrix.loadFrame(LEDMATRIX_MUSIC_NOTE);
  } else if (mood == "dangerous") {
    matrix.loadFrame(LEDMATRIX_DANGER);
  } else if (mood == "normal") {
    matrix.loadFrame(LEDMATRIX_EMOJI_BASIC);
  }  
  server.send(200, "text/plain", "Mood set to: " + mood);
}

void setup(void) {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) ;  // wait for serial port to connect. Needed for native USB port only

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true) ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);  
  status = WiFi.begin(ssid, pass);

  if (status != WL_CONNECT_FAILED) {
    Serial.print("Connected. IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    server.on("/mood", handleMood);
    server.on("/", []() {
      server.send(200, "text/html", (const char *)index_html);
    });

    // Start HTTP webserver
    server.begin();
    Serial.println("HTTP server started");
  }
  else {
    Serial.println("Failed to connect to WiFi");
    // Optionally, you can enter an infinite loop or reset the device
    while (true) ;
  }

  matrix.begin();
}

void loop(void) {
  if (WiFi.status() == WL_CONNECTED) {
    server.handleClient();
  }
}
