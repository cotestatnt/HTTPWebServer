#include <WiFiS3.h>
#include <WebServer.h>

#include "secrets.h"
#include "index_html.h" // Source HTML page in /data/index.html

const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
int status = WL_IDLE_STATUS;

WebServer server(80);

float temperature = 45.5;
float pressure = 1500.0;

// Generate a random oscillation around default values
void handlegetSensor() {
  if (server.hasArg("element")) {
    float value = 0.0;

    if (server.arg("element").equals("Temperatura"))
      value += (random(-100, 100) / 10.0) + temperature;  // Start value +/- 10 °C

    else if (server.arg("element").equals("Pressione"))
      value += random(-200, 200) + pressure;             // Start value +/- 200 mBar

    String response = "{\"element\": \"";
    response += server.arg("element");
    response += "\", \"value\": ";
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
    server.sendHeader(F("Content-Encoding"), F("gzip"));
    server.send(200, "text/html", (const char*)index_html, sizeof(index_html));
  });

  server.on("/sensor", handlegetSensor);

  // If the user does not define a custom handler, the one included in the library will be used.
  // server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
