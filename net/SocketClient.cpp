/*
 * SocketClient.cpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#include "net.hpp"

namespace cppknife {

const std::string JobAgent::_ok("OK");

SocketClient::SocketClient(const char *address, Logger &logger) :
    _socketHandle(0), _logger(logger), _isTcp(false), _address(address), _ip(), _port(
        0), _isConnected(false) {
  if (!setAddress(address)) {
    throw InternalError("Illegal ip address:", address);
  }
}

SocketClient::~SocketClient() {
}

bool SocketClient::connect(const char *address) {
  bool rc = false;
  // A pseudo loop: So we can use break to leave:
  do {
    if (address != nullptr) {
      if (!setAddress(address)) {
        break;
      }
      _address = address;
    }
    /* Create a socket point */
    _socketHandle = socket(_isTcp ? AF_INET : AF_LOCAL, SOCK_STREAM, 0);

    if (_socketHandle < 0) {
      _logger.say(LV_ERROR,
          formatCString("error opening socket (%d): %s", errno,
              strerror(errno)));
      break;
    }
    union {
      struct sockaddr_in _tcp;
      struct sockaddr_un _sun;
    } address;
    bzero((void*) &address, sizeof address);
    if (_isTcp) {
      struct hostent *server = nullptr;
      static const std::regex regexprIp("(\\d{1-3}}\\.){3}}\\d{1-3}");
      if (std::regex_match(_ip, regexprIp)) {
        struct in_addr addr;
        bzero((void*) &addr, sizeof addr);
        inet_aton(_ip.c_str(), &addr);
        server = gethostbyaddr(&addr, sizeof(addr), AF_INET);
      } else {
        server = gethostbyname(_ip.c_str());
      }

      if (server == NULL) {
        _logger.say(LV_ERROR,
            formatCString("unknown address: %s (%d) %s", _ip.c_str(), errno,
                strerror(errno)));
        break;
      }

      address._tcp.sin_family = AF_INET;
      bcopy((char*) server->h_addr, (char*) &address._tcp.sin_addr.s_addr,
      server->h_length);
      address._tcp.sin_port = htons(_port);
    } else {
      address._sun.sun_family = AF_LOCAL;
      size_t maxSize = sizeof address._sun.sun_path - 1;
      if (_address.size() < maxSize) {
        _logger.say(LV_ERROR,
            formatCString("socket path length cutted: %s (%ld) -> %*s",
                _address, maxSize, maxSize, _address));
        _address.erase(maxSize);
      }
      memcpy((void*) address._sun.sun_path, _address.c_str(),
          _address.size() + 1);
    }
    /* Now connect to the server */
    if (::connect(_socketHandle, (struct sockaddr*) &address, sizeof(address))
        < 0) {
      _logger.say(LV_ERROR,
          formatCString("cannot connect to %s (%d) %s", _address.c_str(), errno,
              strerror(errno)));
      break;
    }
    rc = true;
  } while (false);
  _isConnected = rc;
  return rc;
}
bool SocketClient::receive(uint8_t *buffer, size_t bufferSize,
    ssize_t &bufferLength) {
  bool rc = false;
  if (!_isConnected) {
    _logger.say(LV_ERROR, "server is not connected");
  } else {
    bufferLength = ::recv(_socketHandle, buffer, bufferSize, 0);
    if (bufferLength < 0) {
      _logger.say(LV_ERROR,
          formatCString("cannot receive from  %s (%d) %s", _address.c_str(),
          errno, strerror(errno)));
    }
    rc = true;
  }
  return rc;
}
bool SocketClient::send(uint8_t *buffer, size_t bufferLength) {
  bool rc = false;
  if (!_isConnected) {
    _logger.say(LV_ERROR, "server is not connected");
  } else {
    /* Send message to the server */
    auto count = write(_socketHandle, buffer, bufferLength);

    if (count > 0) {
      rc = true;
    } else {
      _logger.say(LV_ERROR,
          formatCString("cannot write to %s (%d) %s", _address.c_str(), errno,
              strerror(errno)));
    }
  }
  return rc;
}

bool SocketClient::setAddress(const char *address) {
  bool rc = true;
  _isTcp = strchr(address, ':') != nullptr;
  if (_isTcp) {
    auto parts = splitCString(_address.c_str(), ":", 2);
    _ip = parts[0];
    _port = atoi(parts[1].c_str());
    if (_port <= 0 || _port > 65535) {
      auto msg = formatCString("illegal port number: %s", parts[1].c_str());
      _logger.say(LV_ERROR, msg);
      rc = false;
    }
  }
  return rc;
}

KniveSocketClient::KniveSocketClient(const char *address, Logger &logger,
    size_t bufferSize) :
    SocketClient(address, logger), _buffer(new uint8_t[bufferSize]), _bufferSize(
        bufferSize) {
}
KniveSocketClient::~KniveSocketClient() {
  delete _buffer;
  _buffer = nullptr;
}
bool KniveSocketClient::receive(uint8_t *buffer, size_t bufferSize,
    size_t &bufferLength) {
  bool rc = false;
  return rc;
}
bool KniveSocketClient::send(scope_t scope, job_t job, uint8_t *buffer,
    size_t bufferLength) {
  bool rc = false;
  size_t headerSize = sizeof(KnifeHeader);
  if (bufferLength + headerSize <= _bufferSize) {
    knifeToken_t token = 0;
    KnifeHeader *header = (KnifeHeader*) _buffer;
    header->_dataLength = bufferLength;
    header->_job = job;
    header->_scope = scope;
    header->_token = token;
    memcpy((void*) (_buffer + headerSize), buffer, bufferLength);
    rc = SocketClient::send(_buffer, bufferLength + headerSize);
  }
  return rc;
}
bool KniveSocketClient::send(scope_t scope, job_t job,
    const std::string &data) {
  auto rc = send(scope, job, (uint8_t*) data.c_str(), data.size());
  return rc;
}
} // namespace
