#include <WiFiS3.h>
#include <WebServer.h>
#include <Preferences.h>

// #include "wifiManager.h"

const char* ssidAP = "ARDUINO_UNOR4_AP";
bool runCaptivePortal = false;

typedef struct {
  char ssid[32];
  char pass[64];
} wifiConfig_t;

Preferences prefs;
WebServer server(80);

size_t saveWifiConfig(const char* ssid, const char* pass) {
  wifiConfig_t wifiConfig;
  strcpy(wifiConfig.ssid, ssid);
  strcpy(wifiConfig.pass, pass);
  return prefs.putBytes("wifi", &wifiConfig, sizeof(wifiConfig));
}

wifiConfig_t getWiFiConfig() {
  wifiConfig_t wifiConfig;
  prefs.getBytes("wifi", &wifiConfig, sizeof(wifiConfig));
  return wifiConfig;
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

  wifiConfig_t config = getWiFiConfig();

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


  server.enableWifiManager();
  server.onWiFiConnected([](const char* ssid, const char* pass) {
    printWiFiInfo();
    if (saveWifiConfig(ssid, pass)) {
      Serial.print("WiFi credentials saved to preferences for SSID: ");
      Serial.println(ssid);
    }
  });
  
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
