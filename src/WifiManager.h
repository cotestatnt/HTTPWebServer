#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <vector>

#include "WebServer.h"
#include "wifi_min_html.h"


class WebServer;

struct NetworkInfo {
  String ssid;
  int32_t rssi;
  uint8_t encType;
  String encryptionTypeStr;
  bool isHidden;
};

class WiFiManager {
  friend class WebServer;

public:
  WiFiManager(WebServer* server);

  void begin();
  void handleClient();

  // Imposta credenziali di default (opzionali)
  void setDefaultCredentials(const String& ssid, const String& password);

  // Imposta configurazione IP statico (opzionale)
  void setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0, IPAddress dns2 = (uint32_t)0);

  // Callback che verrà chiamato quando la connessione WiFi è stabilita
  void onConnected(std::function<void(const char*, const char*)> callback) {
    _connectedCallback = callback;
  }

  // Callback che verrà chiamato quando la configurazione è cambiata
  void onConfigChanged(std::function<void(void)> callback) {
    _configChangedCallback = callback;
  }

  // Metodi per ottenere informazioni sulla connessione
  String getCurrentSSID() const {
    return WiFi.SSID();
  }
  IPAddress getLocalIP() const {
    return WiFi.localIP();
  }
  IPAddress getGateway() const {
    return WiFi.gatewayIP();
  }
  IPAddress getSubnet() const {
    return WiFi.subnetMask();
  }

private:
  WebServer* _server;

  String _defaultSSID;
  String _defaultPassword;

  bool _useStaticIP = false;
  IPAddress _staticIP;
  IPAddress _staticGateway;
  IPAddress _staticSubnet;
  IPAddress _staticDNS1;
  IPAddress _staticDNS2;

  std::function<void(const char*, const char*)> _connectedCallback = nullptr;
  std::function<void(void)> _configChangedCallback = nullptr;

  std::vector<NetworkInfo> _networks;

  // Handlers per le richieste API
  void handleRoot();
  void handleScan();
  void handleSave();
  void handleInfo();

  // Handler di sistema
  String getEncryptionTypeString(uint8_t encType);

  // Metodi di utilità
  bool connectWithStaticIP(const String& ssid, const String& password);
  bool connectWithDHCP(const String& ssid, const String& password);
};

#endif  // WIFI_MANAGER_H