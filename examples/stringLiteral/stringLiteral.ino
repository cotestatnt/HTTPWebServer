#include <WiFiS3.h>
#include <WebServer.h>

#include "secrets.h"
#include "web_files.h"

const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
int status = WL_IDLE_STATUS;

WebServer server(80);

float value1 = 45.5;
float value2 = 23.8;

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}


void handlegetSensor() {

  if (server.hasArg("sensorId")) {
    float value = random(0, 10) / 10.0;
    int id = server.arg("sensorId").toInt();
    switch (id) {
      case 101:
        value += value1;
        break;
      case 102:
        value += value2;
        break;
    }

    String response = "{\"id\": ";
    response += id;
    response += ", \"value\": ";
    response += value;
    response += "}";
    server.send(200, "application/json", response);
  }
  else {
    server.send(505, "text/plain", "args missing");
  }
}

void setup(void) {
  //Initialize serial and wait for port to open:
  Serial.begin(115200);
  while (!Serial) ; // wait for serial port to connect. Needed for native USB port only

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.print("Connected. IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  server.on("/", []() {
    server.send(200, "text/html", index_html);
  });

  server.on("/index.js", []() {
    server.send(200, F("text/html"), index_js);
  });

  server.on("/sensor", handlegetSensor);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
