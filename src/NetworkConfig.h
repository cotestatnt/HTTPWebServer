// NetworkConfig.h
#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

// Supported modes
#define USING_WIFI      1
#define USING_ETHERNET  2
#define USING_WIFININA  3
#define USING_WIFIS3    4

// Modify this value to enable Ethernet
#if defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_SAMD_MKRWIFI1010) || defined(ARDUINO_SAMD_NANO_33_IOT)
  // Default to WiFiNINA on Arduino Nano RP2040 Connect
  #define HARDWARE_TYPE USING_WIFININA
#elif defined(ARDUINO_UNOWIFIR4)
  #define HARDWARE_TYPE USING_WIFIS3
#elif !defined(HARDWARE_TYPE)
  #define HARDWARE_TYPE USING_WIFI
#endif


#endif
