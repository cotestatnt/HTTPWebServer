#include "../Middlewares.h"
#include "../HTTPWebServer.h"

AuthenticationMiddleware &AuthenticationMiddleware::setUsername(const char *username) {
  _username = username;
  _callback = nullptr;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setPassword(const char *password) {
  _password = password;
  _hash = false;
  _callback = nullptr;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setPasswordHash(const char *sha1AsBase64orHex) {
  _password = sha1AsBase64orHex;
  _hash = true;
  _callback = nullptr;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setCallback(HTTPWebServer::THandlerFunctionAuthCheck fn) {
  assert(fn);
  _callback = fn;
  _hash = false;
  _username = HTTP_EMPTY_STRING;
  _password = HTTP_EMPTY_STRING;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setRealm(const char *realm) {
  _realm = realm;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setAuthMethod(HTTPAuthMethod method) {
  _method = method;
  return *this;
}

AuthenticationMiddleware &AuthenticationMiddleware::setAuthFailureMessage(const char *message) {
  _authFailMsg = message;
  return *this;
}

bool AuthenticationMiddleware::isAllowed(HTTPWebServer &server) const {
  if (_callback) {
    return server.authenticate(_callback);
  }

  if (_username.length() && _password.length()) {
    if (_hash) {
      return server.authenticateBasicSHA1(_username.c_str(), _password.c_str());
    } else {
      return server.authenticate(_username.c_str(), _password.c_str());
    }
  }

  return true;
}

bool AuthenticationMiddleware::run(HTTPWebServer &server, Middleware::Callback next) {
  bool authenticationRequired = false;

  if (_callback) {
    authenticationRequired = !server.authenticate(_callback);
  } else if (_username.length() && _password.length()) {
    if (_hash) {
      authenticationRequired = !server.authenticateBasicSHA1(_username.c_str(), _password.c_str());
    } else {
      authenticationRequired = !server.authenticate(_username.c_str(), _password.c_str());
    }
  }

  if (authenticationRequired) {
    server.requestAuthentication(_method, _realm, _authFailMsg);
    return true;
  } else {
    return next();
  }
}
