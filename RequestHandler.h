#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <vector>
#include <assert.h>
#include "HTTP_Method.h"
#include "HTTPWebServer.h"

class RequestHandler {
public:
  virtual ~RequestHandler() {
    delete _chain;
  }

  /*
    note: old handler API for backward compatibility
  */

  virtual bool canHandle(HTTPMethod method, const String &uri) {
    (void)method;
    (void)uri;
    return false;
  }
  virtual bool canUpload(const String &uri) {
    (void)uri;
    return false;
  }
  virtual bool canRaw(const String &uri) {
    (void)uri;
    return false;
  }

  /*
    note: new handler API with support for filters etc.
  */

  virtual bool canHandle(HTTPWebServer &server, HTTPMethod method, const String &uri) {
    (void)server;
    (void)method;
    (void)uri;
    return false;
  }
  virtual bool canUpload(HTTPWebServer &server, const String &uri) {
    (void)server;
    (void)uri;
    return false;
  }
  virtual bool canRaw(HTTPWebServer &server, const String &uri) {
    (void)server;
    (void)uri;
    return false;
  }
  virtual bool handle(HTTPWebServer &server, HTTPMethod requestMethod, const String &requestUri) {
    (void)server;
    (void)requestMethod;
    (void)requestUri;
    return false;
  }
  virtual void upload(HTTPWebServer &server, const String &requestUri, HTTPUpload &upload) {
    (void)server;
    (void)requestUri;
    (void)upload;
  }
  virtual void raw(HTTPWebServer &server, const String &requestUri, HTTPRaw &raw) {
    (void)server;
    (void)requestUri;
    (void)raw;
  }

  virtual RequestHandler &setFilter(std::function<bool(HTTPWebServer &)> filter) {
    (void)filter;
    return *this;
  }

  RequestHandler *next() {
    return _next;
  }
  void next(RequestHandler *r) {
    _next = r;
  }

  RequestHandler &addMiddleware(Middleware *middleware);
  RequestHandler &addMiddleware(Middleware::Function fn);
  RequestHandler &removeMiddleware(Middleware *middleware);
  bool process(HTTPWebServer &server, HTTPMethod requestMethod, String requestUri);

private:
  RequestHandler *_next = nullptr;
  MiddlewareChain *_chain = nullptr;

protected:
  std::vector<String> pathArgs;

public:
  const String &pathArg(unsigned int i) {
    assert(i < pathArgs.size());
    return pathArgs[i];
  }
};

#endif  //REQUESTHANDLER_H
