#include "WiFiManager.h"

// Costruttore
WiFiManager::WiFiManager(WebServer& server) : _server(server) {
}

void WiFiManager::begin() {
    // Configura le route per la gestione WiFi
    _server.on("/wifi", HTTP_GET, [this](){ this->handleRoot(); });
    _server.on("/wifi/scan", HTTP_GET, [this](){ this->handleScan(); });
    _server.on("/wifi/save", HTTP_POST, [this](){ this->handleSave(); });
    _server.on("/wifi/info", HTTP_GET, [this](){ this->handleInfo(); });
    _server.enableCORS(true);

    // Se sono state fornite credenziali di default, prova a connettersi
    if (_defaultSSID.length() > 0) {
        if (_useStaticIP) {
            connectWithStaticIP(_defaultSSID, _defaultPassword);
        } else {
            connectWithDHCP(_defaultSSID, _defaultPassword);
        }
    }
}

void WiFiManager::handleClient() {
    // Il WebServer gestisce già i client, quindi non è necessario fare nulla qui
}

void WiFiManager::setDefaultCredentials(const String& ssid, const String& password) {
    _defaultSSID = ssid;
    _defaultPassword = password;
}

void WiFiManager::setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1, IPAddress dns2) {
    _useStaticIP = true;
    _staticIP = ip;
    _staticGateway = gateway;
    _staticSubnet = subnet;
    _staticDNS1 = dns1;
    _staticDNS2 = dns2;
}

// Handlers per le pagine web
void WiFiManager::handleRoot() {

  _server.sendHeader(F("Content-Encoding"), F("gzip"));
  _server.send(200, "text/html", (const char*)wifi_html, sizeof(wifi_html));    

}

void WiFiManager::handleScan() {
    int n = WiFi.scanNetworks();
    String jsonResponse = "{\"networks\":[";
    for (int i = 0; i < n; ++i) {
        if (i > 0) jsonResponse += ",";
        jsonResponse += "{";
        jsonResponse += "\"ssid\":\"" + String(WiFi.SSID(i))+ "\",";
        jsonResponse += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        jsonResponse += "\"encryption\":\"" + getEncryptionTypeString(WiFi.encryptionType(i)) + "\",";
        jsonResponse += "\"channel\":" + String(WiFi.channel(i));
        jsonResponse += "}";
    }
    jsonResponse += "]}";    
    _server.send(200, "application/json", jsonResponse);
}


void WiFiManager::handleInfo() {
    String jsonResponse = "{";
    jsonResponse += "\"connected\":" + String(WiFi.status() == WL_CONNECTED);

    if (WiFi.status() == WL_CONNECTED) {
      jsonResponse += ",\"info\":{";
      jsonResponse += "\"ssid\":\"" + String(WiFi.SSID()) + "\",";
      jsonResponse += "\"rssi\":" + String(WiFi.RSSI()) + ",";
      jsonResponse += "\"ip\":\"" + WiFi.localIP().toString() + "\",";
      jsonResponse += "\"subnet\":\"" + WiFi.subnetMask().toString() + "\",";
      jsonResponse += "\"gateway\":\"" + WiFi.gatewayIP().toString() + "\",";
      jsonResponse += "\"dns\":\"" + WiFi.dnsIP().toString() + "\",";
      jsonResponse += "}";
    }
    jsonResponse += "}";    
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

    bool useStatic = _server.hasArg("useStatic");

    // Valida i parametri
    if (ssid.length() == 0) {
        _server.send(400, "text/html", getHeader("Errore") + "<div class='container'><h2>SSID non può essere vuoto</h2><a href='/wifi' class='btn'>Indietro</a></div>" + getFooter());
        return;
    }

    bool success = false;

    if (useStatic) {
        // Verifica che i parametri IP siano validi
        IPAddress staticIP, staticGateway, staticSubnet, staticDNS1, staticDNS2;

        if (!staticIP.fromString(ip) || !staticGateway.fromString(gateway) || !staticSubnet.fromString(subnet)) {
            _server.send(400, "text/html", getHeader("Errore") +
                         "<div class='container'><h2>Parametri IP non validi</h2><a href='/wifi' class='btn'>Indietro</a></div>" +
                         getFooter());
            return;
        }

        // DNS sono opzionali
        if (dns1.length() > 0 && !staticDNS1.fromString(dns1)) {
            _server.send(400, "text/html", getHeader("Errore") +
                         "<div class='container'><h2>DNS1 non valido</h2><a href='/wifi' class='btn'>Indietro</a></div>" +
                         getFooter());
            return;
        }

        if (dns2.length() > 0 && !staticDNS2.fromString(dns2)) {
            _server.send(400, "text/html", getHeader("Errore") +
                         "<div class='container'><h2>DNS2 non valido</h2><a href='/wifi' class='btn'>Indietro</a></div>" +
                         getFooter());
            return;
        }

        // Salva la configurazione statica
        _useStaticIP = true;
        _staticIP = staticIP;
        _staticGateway = staticGateway;
        _staticSubnet = staticSubnet;

        if (dns1.length() > 0) _staticDNS1 = staticDNS1;
        if (dns2.length() > 0) _staticDNS2 = staticDNS2;

        success = connectWithStaticIP(ssid, password);
    } else {
        // Usa DHCP
        _useStaticIP = false;
        success = connectWithDHCP(ssid, password);
    }

    if (success) {
        // Salva le credenziali come default per il riavvio
        _defaultSSID = ssid;
        _defaultPassword = password;

        // Chiama il callback se esiste
        if (_configChangedCallback) {
            _configChangedCallback();
        }

        // Reindirizza alla pagina info
        _server.sendHeader("Location", "/wifi/info");
        _server.send(302, "text/plain", "");
    } else {
        _server.send(400, "text/html", getHeader("Errore di Connessione") +
                     "<div class='container'><h2>Impossibile connettersi alla rete WiFi</h2>" +
                     "<p>Verifica SSID e password.</p><a href='/wifi' class='btn'>Indietro</a></div>" +
                     getFooter());
    }
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

bool WiFiManager::connectWithStaticIP(const String& ssid, const String& password) {
    // WiFi.disconnect();
    // delay(100);

    // // Configura IP statico
    // WiFi.config(_staticIP, _staticGateway, _staticSubnet, _staticDNS1, _staticDNS2);

    // // Connetti alla rete WiFi
    // WiFi.begin(ssid.c_str(), password.c_str());

    // // Attendi fino a 20 secondi per la connessione
    // int timeout = 20;
    // while (WiFi.status() != WL_CONNECTED && timeout > 0) {
    //     delay(1000);
    //     timeout--;
    // }

    // if (WiFi.status() == WL_CONNECTED) {
    //     if (_connectedCallback) {
    //         _connectedCallback();
    //     }
    //     return true;
    // }

    // return false;
}

bool WiFiManager::connectWithDHCP(const String& ssid, const String& password) {
    WiFi.disconnect();
    delay(100);

    // Connetti alla rete WiFi con DHCP
    WiFi.begin(ssid.c_str(), password.c_str());

    // Attendi fino a 20 secondi per la connessione
    int timeout = 20;
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(1000);
        timeout--;
    }

    if (WiFi.status() == WL_CONNECTED) {
        if (_connectedCallback) {
            _connectedCallback(ssid.c_str(), password.c_str());
        }
        return true;
    }

    return false;
}

// Metodi per generare pagine HTML
String WiFiManager::getHeader(const String& title) {
    String html = F("<!DOCTYPE html>"
                    "<html lang='it'>"
                    "<head>"
                    "<meta charset='UTF-8'>"
                    "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                    "<title>");
    html += title;
    html += F("</title>"
              "<style>"
              "body{font-family:Arial,sans-serif;margin:0;padding:0;background:#f5f5f5;color:#333}"
              ".container{max-width:800px;margin:0 auto;padding:20px}"
              "h1{color:#0066cc;margin-bottom:20px}"
              "h2{color:#333;margin-bottom:15px}"
              "nav{background:#0066cc;padding:10px 0;color:white}"
              "nav .container{display:flex;justify-content:space-between;align-items:center}"
              "nav a{color:white;text-decoration:none;margin:0 10px}"
              ".btn{display:inline-block;background:#0066cc;color:white;padding:10px 15px;text-decoration:none;border-radius:4px;margin:10px 0}"
              ".btn:hover{background:#004c99}"
              "table{width:100%;border-collapse:collapse;margin:20px 0}"
              "table,th,td{border:1px solid #ddd}"
              "th,td{padding:10px;text-align:left}"
              "th{background:#f2f2f2}"
              "form{margin:20px 0}"
              "label{display:block;margin:10px 0 5px}"
              "input[type=text],input[type=password]{width:100%;padding:8px;margin-bottom:10px;border:1px solid #ddd;border-radius:4px}"
              ".static-ip{margin-top:15px;padding:15px;background:#f9f9f9;border:1px solid #ddd;border-radius:4px}"
              ".signal-strength{display:inline-block;width:25px;height:15px}"
              ".signal-excellent{background:green}"
              ".signal-good{background:yellowgreen}"
              ".signal-fair{background:orange}"
              ".signal-weak{background:red}"
              "</style>"
              "</head>"
              "<body>"
              "<nav>"
              "<div class='container'>"
              "<h1>WiFi Manager</h1>"
              "<div>"
              "<a href='/wifi'>Home</a>"
              "<a href='/wifi/scan'>Scan</a>"
              "<a href='/wifi/info'>Info</a>"
              "</div>"
              "</div>"
              "</nav>");
    return html;
}

String WiFiManager::getFooter() {
    return F("</body></html>");
}

String WiFiManager::getMainPage() {
    String html = getHeader("WiFi Manager");

    html += F("<div class='container'>"
              "<h2>Configura Rete WiFi</h2>");

    // Se connesso, mostra lo stato della connessione
    if (WiFi.status() == WL_CONNECTED) {
        html += F("<p>Stato: <strong>Connesso</strong></p>"
                  "<p>SSID: <strong>");
        html += WiFi.SSID();
        html += F("</strong></p>"
                  "<p>Indirizzo IP: <strong>");
        html += WiFi.localIP().toString();
        html += F("</strong></p>");
    } else {
        html += F("<p>Stato: <strong>Non connesso</strong></p>");
    }

    html += F("<div><a href='/wifi/scan' class='btn'>Scansiona Reti</a></div>"
              "<form action='/wifi/save' method='post'>"
              "<h3>Connessione Manuale</h3>"
              "<label for='ssid'>SSID:</label>"
              "<input type='text' id='ssid' name='ssid' required>"
              "<label for='password'>Password:</label>"
              "<input type='password' id='password' name='password'>"
              "<div>"
              "<input type='checkbox' id='useStatic' name='useStatic' onclick='toggleStaticIP()'>"
              "<label for='useStatic'>Usa IP Statico</label>"
              "</div>"
              "<div id='staticIPFields' class='static-ip' style='display:none'>"
              "<label for='ip'>Indirizzo IP:</label>"
              "<input type='text' id='ip' name='ip' placeholder='192.168.1.100'>"
              "<label for='gateway'>Gateway:</label>"
              "<input type='text' id='gateway' name='gateway' placeholder='192.168.1.1'>"
              "<label for='subnet'>Subnet Mask:</label>"
              "<input type='text' id='subnet' name='subnet' placeholder='255.255.255.0'>"
              "<label for='dns1'>DNS Primario (opzionale):</label>"
              "<input type='text' id='dns1' name='dns1' placeholder='8.8.8.8'>"
              "<label for='dns2'>DNS Secondario (opzionale):</label>"
              "<input type='text' id='dns2' name='dns2' placeholder='8.8.4.4'>"
              "</div>"
              "<button type='submit' class='btn'>Salva e Connetti</button>"
              "</form>"
              "<a href='/wifi/reset' class='btn' style='background:#cc0000'>Reset WiFi</a>"
              "</div>"
              "<script>"
              "function toggleStaticIP() {"
              "  var staticFields = document.getElementById('staticIPFields');"
              "  var useStaticCheckbox = document.getElementById('useStatic');"
              "  if (useStaticCheckbox.checked) {"
              "    staticFields.style.display = 'block';"
              "  } else {"
              "    staticFields.style.display = 'none';"
              "  }"
              "}"
              "</script>");

    html += getFooter();
    return html;
}
