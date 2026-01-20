#ifndef MIDDLEWARE_H
#define MIDDLEWARE_H

#include <assert.h>
#include <functional>

class MiddlewareChain;
class HTTPWebServer;

class Middleware {
public:
  typedef std::function<bool(void)> Callback;
  typedef std::function<bool(HTTPWebServer &server, Callback next)> Function;

  virtual ~Middleware() {}

  virtual bool run(HTTPWebServer &server, Callback next) {
    (void)server;
    return next();
  };

private:
  friend MiddlewareChain;
  Middleware *_next = nullptr;
  bool _freeOnRemoval = false;
};

class MiddlewareFunction : public Middleware {
public:
  explicit MiddlewareFunction(const Middleware::Function& fn) : _fn(fn) {}

  bool run(HTTPWebServer &server, Middleware::Callback next) override {
    return _fn(server, next);
  }

private:
  Middleware::Function _fn;
};

class MiddlewareChain {
public:
  ~MiddlewareChain();

  void addMiddleware(Middleware::Function fn);
  void addMiddleware(Middleware *middleware);
  bool removeMiddleware(Middleware *middleware);

  bool runChain(HTTPWebServer &server, Middleware::Callback finalizer);

private:
  Middleware *_root = nullptr;
  Middleware *_current = nullptr;
};

#endif
