#include "WiFiManager.h"


WiFiManager::WiFiManager(WebServer& server)
  : _server(server) {
}

void WiFiManager::begin() {
  // Configure routes for WiFi management
  _server.on("/wifi", HTTP_GET, [this]() {
    this->handleRoot();
  });
  _server.on("/wifi/scan", HTTP_GET, [this]() {
    this->handleScan();
  });
  _server.on("/wifi/save", HTTP_POST, [this]() {
    this->handleSave();
  });
  _server.on("/wifi/info", HTTP_GET, [this]() {
    this->handleInfo();
  });
  _server.enableCORS(true);

  // If default credentials are provided, try to connect
  if (strlen(_config.ssid)) {
    if (_useStaticIP) {
      connectWithStaticIP(_config.ssid, _config.pass);
    } else {
      connectWithDHCP(_config.ssid, _config.pass);
    }
  }

  _connected = (WiFi.status() == WL_CONNECTED);

}
void WiFiManager::handleClient() {
  // The WebServer already handles clients, so nothing needs to be done here
}

void WiFiManager::setDefaultCredentials(const String& ssid, const String& password) {
  strcpy(_config.ssid, ssid.c_str());
  strcpy(_config.pass, password.c_str());
}

void WiFiManager::setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
  _useStaticIP = true;
  _config.ip = ip;
  _config.gateway = gateway;
  _config.subnet = subnet;
  _config.dns1 = dns1;
  _config.dns2 = dns2;
}

// Handlers for web pages
void WiFiManager::handleRoot() {
  _server.sendHeader(F("Content-Encoding"), F("gzip"));
  _server.send(200, "text/html", (const char*)wifi_min_html, sizeof(wifi_min_html));
}

void WiFiManager::handleScan() {
  int n = WiFi.scanNetworks();
  String jsonResponse = "{\"networks\":[";
  for (int i = 0; i < n; ++i) {
    if (i > 0) jsonResponse += ",";
    jsonResponse += "{";
    jsonResponse += "\"ssid\":\"" + String(WiFi.SSID(i)) + "\",";
    jsonResponse += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
    jsonResponse += "\"encryption\":\"" + getEncryptionTypeString(WiFi.encryptionType(i)) + "\",";
    jsonResponse += "\"channel\":" + String(WiFi.channel(i));
    jsonResponse += "}";
  }
  jsonResponse += "]}";
  _server.send(200, "application/json", jsonResponse);
}


void WiFiManager::handleInfo() {
  String ssid = WiFi.SSID();
  if (!ssid.length())
    ssid = _config.ssid;
  String jsonResponse = "{";
  jsonResponse += "\"connected\":" + String(_connected);
  jsonResponse += ",\"info\":{";
  jsonResponse += "\"ssid\":\"" + ssid + "\",";
  jsonResponse += "\"rssi\":" + String(WiFi.RSSI()) + ",";
  jsonResponse += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
  jsonResponse += "\"subnet\":\"" + WiFi.subnetMask().toString() + "\",";
  jsonResponse += "\"gateway\":\"" + WiFi.gatewayIP().toString() + "\",";
  jsonResponse += "\"dns\":\"" + WiFi.dnsIP().toString() + "\"";
  jsonResponse += "}}";
  _server.send(200, "application/json", jsonResponse);
}

void WiFiManager::handleSave() {
  String ssid = _server.arg("ssid");
  String password = _server.arg("password");
  String ip = _server.arg("ip");
  String gateway = _server.arg("gateway");
  String subnet = _server.arg("subnet");
  String dns1 = _server.arg("dns1");
  String dns2 = _server.arg("dns2");

  bool useStatic = _server.arg("useStatic").toInt();

  // Validate parameters
  if (ssid.length() == 0) {
    _server.send(400, "text/html", "SSID cannot be empty");
    return;
  }

  if (useStatic) {
    // Verify that IP parameters are valid
    IPAddress staticIP, staticGateway, staticSubnet, staticDNS1, staticDNS2;

    if (!staticIP.fromString(ip) || !staticGateway.fromString(gateway) || !staticSubnet.fromString(subnet)) {
      _server.send(400, "text/html", "Invalid IP parameters");
      return;
    }

    // DNS are optional
    if (dns1.length() > 0 && !staticDNS1.fromString(dns1)) {
      _server.send(400, "text/html", "Invalid DNS1");
      return;
    }

    if (dns2.length() > 0 && !staticDNS2.fromString(dns2)) {
      _server.send(400, "text/html", "Invalid DNS2");
      return;
    }

    // Save static configuration
    _useStaticIP = true;
    _config.ip = staticIP;
    _config.gateway = staticGateway;
    _config.subnet = staticSubnet;

    if (dns1.length() > 0) _config.dns1 = staticDNS1;
    if (dns2.length() > 0) _config.dns2 = staticDNS2;

    _connected = connectWithStaticIP(ssid, password);
  } else {
    // Use DHCP
    _useStaticIP = false;
    _connected = connectWithDHCP(ssid, password);
  }

  if (_connected) {
    // Save credentials as default for reboot
    strcpy(_config.ssid, ssid.c_str());
    strcpy(_config.pass, password.c_str());

    // Call the callback if it exists
    if (_configChangedCallback) {
      _configChangedCallback(_config);
    }

    // Connected, load the info page
    // _server.send(200, "text/plain", "{\"result\": \"ok\"}");
    handleInfo();
  } 
  else {
    _server.send(400, "text/html", "Unable to connect to the WiFi network");
  }
}

bool WiFiManager::connectWithStaticIP(const String& ssid, const String& password) {
  // Configure static IP
  WiFi.config(_config.ip, _config.dns1, _config.gateway, _config.subnet);
  WiFi.setDNS(_config.dns1, _config.dns2);
  return connectWithDHCP(ssid, password);
}

bool WiFiManager::connectWithDHCP(const String& ssid, const String& password) {
  // Wait up to 20 seconds for the connection
  int timeout = 10;
  while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    // Connect to the WiFi network using DHCP
    callBeginIfExists(WiFi, ssid.c_str(), password.c_str(), WIFI_MODE_APSTA);
    delay(2000);
    timeout--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    if (_connectedCallback) {
      _connectedCallback(_config);
    }
    return true;
  }
  return false;
}


String WiFiManager::getEncryptionTypeString(uint8_t encType) {
#ifdef ESP8266
  switch (encType) {
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_TKIP:
      return "WPA/TKIP";
    case ENC_TYPE_CCMP:
      return "WPA2/CCMP";
    case ENC_TYPE_NONE:
      return "Open";
    case ENC_TYPE_AUTO:
      return "Auto";
    default:
      return "Unknown";
  }
#elif defined(ESP32)
  switch (encType) {
    case WIFI_AUTH_OPEN:
      return "Open";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA/PSK";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2/PSK";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WPA2/PSK";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2/Enterprise";
    default:
      return "Unknown";
  }
#elif defined(ARDUINO_UNOR4_WIFI)
  switch (encType) {
    case ENC_TYPE_NONE:
      return "Open";
    case ENC_TYPE_WEP:
      return "WEP";
    case ENC_TYPE_WPA:
      return "WPA/PSK";
    case ENC_TYPE_WPA2:
      return "WPA2/PSK";
    case ENC_TYPE_WPA3:
      return "WPA2/PSK";
    case ENC_TYPE_AUTO:
      return "AUTO";
    default:
      return "Unknown";
  }
#endif
}
