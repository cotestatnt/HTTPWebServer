#include <Arduino.h>
#include <SPI.h>
#include <WiFiNINA.h>
#include <WebServer.h>          // https://github.com/cotestatnt/HTTPWebServer
#include <FlashStorage_SAMD.h>  // https://github.com/khoih-prog/FlashStorage_SAMD

const char* ssidAP = "ARDUINO_AP";
const char* passAP = "123456789";

WebServer server(80);
WiFiManager wifiManager(server);

const int START_ADDRESS     = 0;


WifiConfig_t getWiFiConfig() {
  WifiConfig_t config;
  EEPROM.get(START_ADDRESS, config);
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
  while (!Serial) ; // wait for serial port to connect. Needed for native USB port only
  
  Serial.print(F("\nStart WiFi Manager on ")); Serial.println(BOARD_NAME);
  Serial.println(FLASH_STORAGE_SAMD_VERSION);

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
    delay(2000);

    if (millis() - timeout > 10000) {
      status = WL_IDLE_STATUS;
      break;
    }
  }

  if (status == WL_IDLE_STATUS) {
    // No WiFi connection avalaible, start Access Point
    // by default the local IP address will be 192.168.4.1
    status = WiFi.beginAP(ssidAP, passAP);
    Serial.print("Creating access point named: ");
    Serial.println(ssidAP);    
    if (status != WL_AP_LISTENING) {
      Serial.println("Creating access point failed");
    }
  } 
  else {
    printWiFiInfo();
  }

  // Configure WiFiManager callback in order to save wifi credentials usings preferences
  wifiManager.onConnected( [](WifiConfig_t& newConfig) {
    EEPROM.put(START_ADDRESS, newConfig);
    EEPROM.commit();

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
