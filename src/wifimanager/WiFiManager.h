#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <vector>

#include "WebServer.h"
#include "wifi_min_html.h"

#ifndef WIFI_MODE_STA
#define WIFI_MODE_STA 1
#define WIFI_MODE_AP 2
#define WIFI_MODE_APSTA 3
#endif

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////
// Template to check if a class has the begin method with the specified signature
template <typename, typename = void>
struct has_begin : std::false_type
{
};

template <typename T>
struct has_begin<T, std::void_t<decltype(std::declval<T>().begin(std::declval<const char *>(), std::declval<const char *>(), std::declval<uint8_t>()))>>
    : std::true_type
{
};

template <typename T>
constexpr bool has_begin_v = has_begin<T>::value;

template <typename T>
void callBeginIfExists(T &obj, const char *ssid, const char *passphrase, uint8_t mode = 1)
{
  if constexpr (has_begin_v<T>)
  {
    obj.begin(ssid, passphrase, mode);
  }
  else
  {
    obj.begin(ssid, passphrase);
  }
}
/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

class WebServer;

struct NetworkInfo
{
  String ssid;
  int32_t rssi;
  uint8_t encType;
  String encryptionTypeStr;
  bool isHidden;
};

/**
 * @class WiFiManager
 * @brief Manages WiFi connections and configurations for an embedded system.
 *
 * The WiFiManager class provides functionality to manage WiFi connections,
 * including setting default credentials, configuring static IP addresses,
 * and handling client requests. It also allows registering callbacks for
 * connection and configuration changes.
 */

class WiFiManager
{
  friend class WebServer;

public:
  /**
   * @brief Constructs a WiFiManager object.
   * @param server Reference to the WebServer instance used for handling HTTP requests.
   */
  explicit WiFiManager(WebServer &server);
  /**
   * @brief Destructor for the WiFiManager class.
   */
  virtual ~WiFiManager() {}
  /**
   * @brief Initializes the WiFiManager and starts managing WiFi connections.
   */
  void begin();
  /**
   * @brief Handles incoming client requests.
   */
  void handleClient();
  /**
   * @brief Sets default WiFi credentials.
   * @param ssid The default SSID to connect to.
   * @param password The default password for the WiFi network.
   */
  void setDefaultCredentials(const String &ssid, const String &password);

  /**
   * @brief Configures a static IP address for the WiFi connection.
   * @param ip The static IP address to use.
   * @param gateway The gateway address.
   * @param subnet The subnet mask.
   * @param dns1 (Optional) The primary DNS server address.
   * @param dns2 (Optional) The secondary DNS server address.
   */
  void setStaticIP(IPAddress ip, IPAddress gateway, IPAddress subnet, IPAddress dns1 = (uint32_t)0, IPAddress dns2 = (uint32_t)0);

  /**
   * @brief Registers a callback to be called when the WiFi connection is established.
   * @param callback A function that takes the SSID and password as parameters.
   */
  inline void onConnected(const std::function<void(const char *, const char *)> &callback)
  {
    _connectedCallback = callback;
  }
  /**
   * @brief Registers a callback to be called when the WiFi configuration changes.
   * @param callback A function with no parameters.
   */
  inline void onConfigChanged(const std::function<void(void)> &callback)
  {
    _configChangedCallback = callback;
  }

private:
  WebServer &_server;
  String _defaultSSID;
  String _defaultPassword;

  bool _useStaticIP = false;
  IPAddress _staticIP;
  IPAddress _staticGateway;
  IPAddress _staticSubnet;
  IPAddress _staticDNS1;
  IPAddress _staticDNS2;

  std::function<void(const char *, const char *)> _connectedCallback = nullptr;
  std::function<void(void)> _configChangedCallback = nullptr;
  std::vector<NetworkInfo> _networks;

  void handleRoot();
  void handleScan();
  void handleSave();
  void handleInfo();
  String getEncryptionTypeString(uint8_t encType);
  bool connectWithStaticIP(const String &ssid, const String &password);
  bool connectWithDHCP(const String &ssid, const String &password);
};

#endif // WIFI_MANAGER_H
