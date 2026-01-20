#ifndef URI_GLOB_H
#define URI_GLOB_H

#include "../HTTPUri.h"
#include <fnmatch.h>

class UriGlob : public HTTPUri {

public:
  explicit UriGlob(const char *uri) : HTTPUri(uri){};
  explicit UriGlob(const String &uri) : HTTPUri(uri){};

  HTTPUri *clone() const override final {
    return new UriGlob(_uri);
  };

  bool canHandle(const String &requestUri, __attribute__((unused)) std::vector<String> &pathArgs) override final {
    return fnmatch(_uri.c_str(), requestUri.c_str(), 0) == 0;
  }
};

#endif
