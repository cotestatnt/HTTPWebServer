#ifndef HTTP_URI_H
#define HTTP_URI_H

#include <Arduino.h>
#include <vector>

class HTTPUri {

protected:
  const String _uri;

public:
  HTTPUri(const char *uri) : _uri(uri) {}
  HTTPUri(const String &uri) : _uri(uri) {}
  HTTPUri(const __FlashStringHelper *uri) : _uri((const char *)uri) {}
  virtual ~HTTPUri() {}

  virtual HTTPUri *clone() const {
    return new HTTPUri(_uri);
  }

  virtual void initPathArgs(std::vector<String> &pathArgs) {
    pathArgs.clear();
  }

  virtual bool canHandle(const String &requestUri, std::vector<String> &pathArgs) {
    if (_uri == requestUri) {
      pathArgs.clear();
      return true;
    }
    return false;
  }
};

#endif
