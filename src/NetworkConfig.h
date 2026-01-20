// NetworkConfig.h
#ifndef NETWORK_CONFIG_H
#define NETWORK_CONFIG_H

// Supported modes
#define USING_WIFI      1
#define USING_ETHERNET  2
#define USING_WIFININA  3

// Modify this value to enable Ethernet
#if defined(ARDUINO_NANO_RP2040_CONNECT)
  // Default to WiFiNINA on Arduino Nano RP2040 Connect
  #define HARDWARE_TYPE USING_WIFININA
#elif !defined(HARDWARE_TYPE)
  #define HARDWARE_TYPE USING_WIFI
#endif


#endif
