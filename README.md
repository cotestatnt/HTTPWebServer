# HTTPWebServer
This project is a port of the library included in the ESP32 core for Arduino, designed to work with virtually any microcontroller that has WiFi or Ethernet connectivity. 
Development started primarily to provide a decent webserver library for the Arduino Uno R4 WiFi. 

The library makes it easy and flexible to implement AJAX (Asynchronous JavaScript and XML) for creating dynamic web pages that can update content without requiring a full page reload. This makes it ideal for building modern, responsive web interfaces for IoT devices. Additionally, it includes a WebSocket server for real-time bidirectional communication, enabling seamless data exchange between the microcontroller and the client. The library also supports RESTful techniques, allowing you to create APIs for interacting with your microcontroller in a structured and scalable way.

The project is open source and freely available to the community.
If you find this project useful, consider [sponsoring this project!](https://paypal.me/cotesta). Your sponsorship will help ensure the project's sustainability and growth.

# Key Features
- Cross-Platform Compatibility: works with microcontrollers beyond the ESP32, including the Arduino Uno R4 WiFi. More MCUs will be tested.
- WebSocket Support: integrates a WebSocket server for real-time data exchange, making it ideal for creating interactive web interfaces.
- Simple WiFi Manager: implements a light WiFi manager that allows scanning for available SSIDs and saving WiFi credentials without the need to hardcode them in the firmware.
- Ethernet Support: the library can also be used with Ethernet connectivity. A small modification to the [NetworkConfig.h](https://github.com/cotestatnt/HTTPWebServer/blob/main/src/NetworkConfig.h) file is required to enable this feature.
- Gzip Compression: reduces the size of HTML, CSS, and JavaScript resources, improving webserver efficiency.
  In the [gzipContent.ino](https://github.com/cotestatnt/HTTPWebServer/tree/main/examples/gzipContent) example, the web page content is served to the client in a compressed binary format using the gzip algorithm.
  
  This web tool https://cotestatnt.github.io/fsdata.html allows you to generate byte arrays from HTML, CSS, or JavaScript files.

# Hardware Support
At the current stage, the library has been tested and verified to work with the Arduino Uno R4 WiFi. However, support for other hardware platforms such as nRF52840, RP2040, and others is planned for the near future. Contributions from the community are greatly appreciated to help expand compatibility and add new features!
