#include <Arduino.h>
#include <SPI.h>
#include <Ethernet.h>

// To enable Ethernet support, the NetworkConfig.h file must be modified
#include <NetworkConfig.h>
#include <HTTPWebServer.h>

// Source HTML page in /data/index.html
#include "index_html.h" 

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 177);

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

  Serial.println("Ethernet WebServer Example");
  Ethernet.init(10);
  Ethernet.begin(mac, ip);

  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  server.on("/", []() {
    server.sendHeader(F("Content-Encoding"), F("gzip"));
    server.send(200, "text/html", (const char*)index_html, sizeof(index_html));
  });

  server.on("/sensor", handlegetSensor);

  // If the user does not define a custom handler, the one included in the library will be used.
  // server.onNotFound(handleNotFound);

  server.begin();
  Serial.print("HTTP server started at ");
  Serial.println(Ethernet.localIP());;
}

void loop(void) {
  server.handleClient();
}
