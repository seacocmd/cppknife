/*
 * SocketServer.hpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#include "../net/net.hpp"
namespace cppknife {

knifeToken_t AuthorizationChecker::buildToken(time_t time,
    const char *context) {
  if (context == nullptr) {
    context = "cppknife";
  }
  union {
    struct {
      uint32_t _crcContext;
      uint32_t _crcTime;
    } _input;
    knifeToken_t _token;
  } transformer;
  uint32_t crc2 = 0;
  // Time interval is 8 seconds:
  time = time / VALID_INTERVAL_SECONDS * VALID_INTERVAL_SECONDS;
  transformer._input._crcTime = crc2 = crc32((uint8_t*) &time, sizeof time,
      true);
  transformer._input._crcContext = crc32Update((uint8_t*) &time, sizeof time,
      crc2, true);
  return transformer._token;
}
bool AuthorizationChecker::checkToken(knifeToken_t token, time_t time,
    const char *context) {
  knifeToken_t token2 = buildToken(time, context);
  bool rc = token == token2;
  if (!rc) {
    token2 = buildToken(time - VALID_INTERVAL_SECONDS, context);
    rc = token == token2;
    if (!rc) {
      token2 = buildToken(time + VALID_INTERVAL_SECONDS, context);
      rc = token == token2;
    }
  }
  return rc;
}

/**
 * Constructor.
 * @param name IP: &lt;interface>:&lt;port> Unix socket: socket name.
 * @param logger The logging manager
 */
SocketServer::SocketServer(const char *address, Logger &logger) :
    _socketHandle(0), _logger(logger), _isTcp(strchr(address, ':') != nullptr), _address(
        address) {
}
SocketServer::~SocketServer() {
}

bool SocketServer::listen(SocketTaskHandler &handler, bool nonBlocking,
    int maxConnections) {
  bool rc = false;
  /* First call to socket() function */
  do {
    if (!_isTcp) {
      ::unlink(_address.c_str());
    }
    _socketHandle = socket(_isTcp ? AF_INET : AF_LOCAL, SOCK_STREAM, 0);
    if (_socketHandle < 0) {
      _logger.say(LV_ERROR,
          formatCString("listen(): cannot get socket (%d): %s", errno,
              strerror(errno)));
      break;
    }
    bool onOff = false;
    if (_isTcp
        && setsockopt(_socketHandle, SOL_SOCKET, SO_REUSEADDR,
            (const char*) &onOff, sizeof(onOff)) == -1) {
      _logger.say(LV_ERROR,
          formatCString("listen(): cannot set SO_REUSEADDR(%d): %s", errno,
              strerror(errno)));
      break;
    }
#ifdef BAD_IDEA
    if (nonBlocking) {
      auto options = fcntl(_socketHandle, F_GETFL);
      if (options >= 0) {
        fcntl(_socketHandle, F_SETFL, options | O_NONBLOCK);
      }
    }
#endif
    typedef union {
      struct sockaddr adress;
      struct sockaddr_in tcp;
      struct sockaddr_un sun;
    } SocketInfo_t;
    SocketInfo_t socketInfo;
    size_t sizeInfo = 0;
    bzero(static_cast<void*>(&socketInfo), sizeof socketInfo);
    if (_isTcp) {
      auto parts = splitCString(_address.c_str(), ":", 2);
      auto portNo = atoi(parts[1].c_str());
      if (portNo <= 0 || portNo > 65535) {
        _logger.say(LV_ERROR,
            formatCString("illegal port number: %s", parts[1].c_str()));
        break;
      }
      //auto interface = parts[0].c_str();
      socketInfo.tcp.sin_family = AF_INET;
      socketInfo.tcp.sin_addr.s_addr = INADDR_ANY;
      socketInfo.tcp.sin_port = htons(portNo);
      sizeInfo = sizeof socketInfo.tcp;
    } else {
      socketInfo.sun.sun_family = AF_LOCAL;
      sizeInfo = sizeof socketInfo.sun;
      if (_address.size() >= sizeof(socketInfo.sun.sun_path)) {
        _address.erase(sizeof socketInfo.sun.sun_path - 1);
        _logger.say(LV_ERROR,
            formatCString("listen: path length to large. Changed to %s",
                _address.c_str()));
      }
      size_t length = _address.size();
      // copy with EOS:
      memcpy(socketInfo.sun.sun_path, _address.c_str(), length + 1);
    }
    /* Now bind the host address using bind() call.*/
    if (bind(_socketHandle, &socketInfo.adress, sizeInfo) < 0) {
      _logger.say(LV_ERROR,
          formatCString("listen(): cannot bind to %s (%d): %s",
              _address.c_str(),
              errno, strerror(errno)));
      break;
    }
    if (::listen(_socketHandle, maxConnections) != 0) {
      _logger.say(LV_ERROR,
          formatCString("listen(): cannot listen on %s (%d): %s",
              _address.c_str(),
              errno, strerror(errno)));
      break;
    }
    rc = true;
    bool again = true;
    while (again) {
      SocketInfo_t client;
      bzero((void*) &client, sizeof client);
      socklen_t size = sizeof client;
      errno = 0;
      auto connectionHandle = ::accept(_socketHandle, &client.adress, &size);

      if (connectionHandle < 0) {
        _logger.say(LV_ERROR,
            formatCString("listen(): cannot accept on %s (%d): %s",
                _address.c_str(),
                errno, strerror(errno)));
        break;
      }
      handler.doIt(connectionHandle);
      close(connectionHandle);
    }
  } while (false);
  return rc;
}
bool SocketServer::send(uint8_t *buffer, size_t bufferLength) {
  auto rc = ::send(_socketHandle, buffer, bufferLength, 0);
  return rc > 0;
}

SocketTaskHandler::SocketTaskHandler(SocketServer &server) :
    _server(server) {
}
SocketTaskHandler::~SocketTaskHandler() {
}

bool SocketTaskHandler::send(uint8_t *buffer, size_t bufferLength) {
  bool rc = _server.send(buffer, bufferLength);
  return rc;
}
JobAgent::JobAgent(JobKind kind, const char *name, KnifeTaskHandler &parent) :
    _kind(kind), _name(name), _parent(parent) {
}
JobAgent::~JobAgent() {
}

bool JobAgent::sendData(uint8_t *buffer, size_t bufferLength) {
  bool rc = _parent.send(buffer, bufferLength);
  return rc;
}
StringJobAgent::StringJobAgent(const char *name, KnifeTaskHandler &parent) :
    JobAgent(JK_STRING, name, parent) {
}
StringJobAgent::~StringJobAgent() {
}
BinaryJobAgent::BinaryJobAgent(const char *name, KnifeTaskHandler &parent) :
    JobAgent(JK_STRING, name, parent) {
}
BinaryJobAgent::~BinaryJobAgent() {
}

KnifeTaskHandler::KnifeTaskHandler(SocketServer &server, size_t bufferSize) :
    SocketTaskHandler(server), _buffer(new uint8_t[bufferSize]), _bufferSize(
        bufferSize), _agents() {
}
KnifeTaskHandler::~KnifeTaskHandler() {
  for (auto agent : _agents) {
    delete agent;
  }
  _agents.clear();
  delete _buffer;
  _buffer = nullptr;
}
bool KnifeTaskHandler::doIt(int socketHandle) {
  bool again = true;
  uint8_t *buffer = nullptr;
  size_t bufferLength = 0;
  bool rc = true;
  while (again) {
    bufferLength = ::recv(socketHandle, _buffer, _bufferSize - 1, 0);
    // <4-byte-crc32-little-endian><4-byte-lengh-data-little-endian><8-byte-authentification-token><8-byte-scope><8-byte-task><data>

    KnifeHeader *header = (KnifeHeader*) _buffer;
    size_t headerSize = sizeof(KnifeHeader);
    std::string data;
    for (auto agent : _agents) {
      if (agent->isResponsible(header->_scope, header->_job)) {
        again = false;
        if (agent->kind() == JK_STRING) {
          if (data.size() == 0) {
            data = std::string((const char*) (buffer + headerSize),
                bufferLength - headerSize);
          }
          rc = (dynamic_cast<StringJobAgent*>(agent))->process(header->_scope,
              header->_job, data);
        } else {
          rc = (dynamic_cast<BinaryJobAgent*>(agent))->process(header->_scope,
              header->_job, buffer + headerSize, bufferLength - bufferLength);
        }
      }
    }
  }
  return rc;
}
void KnifeTaskHandler::registerAgent(JobAgent *agent) {
  _agents.push_back(agent);
}

bool KnifeTaskHandler::send(uint8_t *buffer, size_t bufferLength) {
  bool rc = false;
  KnifeAnswerHeader *header = (KnifeAnswerHeader*) _buffer;
  size_t headerSize = sizeof(*header);
  if (bufferLength + headerSize < _bufferSize) {
    knifeToken_t token = 0;
    header->_token = token;
    header->_length = bufferLength;
    memcpy((void*) (_buffer + headerSize), buffer, bufferLength);
    _server.send(_buffer, bufferLength + headerSize);
  }
  return rc;
}
BasicJobAgent::BasicJobAgent(KnifeTaskHandler &parent) :
    StringJobAgent("basic", parent) {
}
BasicJobAgent::~BasicJobAgent() {

}
bool BasicJobAgent::isResponsible(scope_t scope, job_t job) {
  bool rc = scope == SCOPE_BASIC;
  return rc;
}
bool BasicJobAgent::process(scope_t scope, job_t job, const std::string &data) {
  bool rc = true;
  if (job == JOB_ECHO) {
    _parent.send((uint8_t*) data.c_str(), data.size());
  } else if (job == JOB_TIME) {
    char buffer[64];
    snprintf(buffer, sizeof buffer, "%ld", time(nullptr));
    _parent.send((uint8_t*) buffer, strlen(buffer));
  } else {
    auto message = "<unknown job>";
    _parent.send((uint8_t*) message, strlen(message));
  }
  return rc;
}

} /* namespace */
