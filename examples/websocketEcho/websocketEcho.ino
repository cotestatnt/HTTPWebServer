#include <Arduino.h>
#include <WiFiS3.h>

#include <WebServer.h>

#include "secrets.h"
#include "index_htm.h"

const char *ssid = SECRET_SSID;
const char *pass = SECRET_PASS;
int status = WL_IDLE_STATUS;

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length) {
  (void) length;
  switch (type) {
    case WStype_CONNECTED:
      webSocket.sendTXT(num, "Welcome WS client!");
      break;
    case WStype_TEXT:
      Serial.print("[WS]: ");
      Serial.println((char*)payload);
      webSocket.sendTXT(num, (char*)payload);
      break;
    default:
      break;
  }
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
    server.send(200, "text/html", (const char *)index_html);
  });

  // Start HTTP webserver
  server.begin();
  Serial.println("HTTP server started");

  // Start websockets server.
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  Serial.println("Websockets server started");
}

void loop(void) {
  server.handleClient();
  webSocket.loop();
}
