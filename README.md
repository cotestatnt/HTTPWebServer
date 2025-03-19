If you like this work, please consider [sponsoring this project!](https://paypal.me/cotesta)

# HTTPWebServer
This is a port of the library included in the ESP32 core for Arduino that should compile with virtually any microcontroller that has WiFi or Ethernet connectivity. Development started primarily to have a decent webserver library with Arduino Uno R4.

In the [gzipContent.ino](https://github.com/cotestatnt/HTTPWebServer/tree/main/examples/gzipContent) example, the web page content is served to the client in a compressed binary format using the gzip algorithm (which is supported by browsers).
To generate the byte array to include in the sketch, I created the web page https://cotestatnt.github.io/fsdata.html, where you can upload your HTML/CSS/JavaScript source code and get a byte array as output, ready to be copied and pasted into the sketch.


