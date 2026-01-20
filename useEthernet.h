#include <SPI.h>
#include <Ethernet.h>
#define NetworkClient EthernetClient
#define NetworkServer EthernetServer

#include "HTTPWebServer.h"


// // #define USE_HARDWARE USING_ETHERNET

// #ifndef USE_HARDWARE
//   #error "Definire USE_HARDWARE come USING_WIFI o USING_ETHERNET"
// #endif

// // Modalità supportate
// #define USING_WIFI      1
// #define USING_ETHERNET  2

// // Verifica la modalità selezionata
// #if USE_HARDWARE == USING_WIFI
//   #if defined(ARDUINO_UNOR4_WIFI)
//   #include "WiFi.h"
//   #include "WiFiClient.h"
//   #include "WiFiServer.h"
//   #define NetworkClient WiFiClient
//   #define NetworkServer WiFiServer
//   #endif
// #elif USE_HARDWARE == USING_ETHERNET
//   #include <SPI.h>
//   #include <Ethernet.h>
//   #define NetworkClient EthernetClient
//   #define NetworkServer EthernetServer
// #else
//   #error "Modalità USE_HARDWARE non valida! Usare USING_WIFI o USING_ETHERNET ."
// #endif
