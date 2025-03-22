#include <Arduino.h>
#include <WiFiS3.h>
#include <WebServer.h>
#include <Preferences.h>

const char* ssidAP = "ARDUINO_UNOR4_AP";

Preferences prefs;
WebServer server(80);
WiFiManager wifiManager(server);

WifiConfig_t getWiFiConfig() {
  WifiConfig_t config;
  prefs.getBytes("wifi", &config, sizeof(config));
  return config;
}

void printWiFiInfo() {
  Serial.print("Connected to SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(115200);

  if (!prefs.begin("wifi")) {  // use "wifi" namespace
    Serial.println("Cannot initialize preferences");
    Serial.println("Make sure your WiFi firmware version is greater than 0.3.0");
    while (1) {};
  }

  WifiConfig_t config = getWiFiConfig();

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true) ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // Attempt to connect to WiFi network:
  int status = WL_IDLE_STATUS;
  uint32_t timeout = millis();
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(config.ssid);
    status = WiFi.begin(config.ssid, config.pass);
    delay(5000);

    if (millis() - timeout > 15000) {
      status = WL_IDLE_STATUS;
      break;
    }
  }

  if (status == WL_IDLE_STATUS) {
    // No WiFi connection avalaible, start Access Point
    // by default the local IP address will be 192.168.4.1
    WiFi.beginAP(ssidAP, "");
    Serial.print("Creating access point named: ");
    Serial.println(ssidAP);    
  } 
  else {
    printWiFiInfo();
  }

  // Configure WiFiManager callback in order to save wifi credentials usings preferences
  wifiManager.onConnected( [](WifiConfig_t& newConfig) {
    prefs.putBytes("wifi", &newConfig, sizeof(newConfig));
    Serial.print("WiFi credentials saved to preferences for SSID: ");
    Serial.println(newConfig.ssid);

    printWiFiInfo();
  });

  // Start the WiFiManager
  wifiManager.begin();
  
  // Reindirizza a /wifi
  server.on("/", []() {
    server.sendHeader("Location", "/wifi");
    server.send(302, "text/plain", "");
  });

 
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Gestisci le richieste HTTP
  server.handleClient();
}
