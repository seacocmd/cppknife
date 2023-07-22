/*
 * SocketClient.hpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#ifndef OS_SOCKETCLIENT_HPP_
#define OS_SOCKETCLIENT_HPP_
namespace cppknife {

class SocketClient {
protected:
  int _socketHandle;
  Logger &_logger;
  bool _isTcp;
  /// &lt;ip>:&lt;port or /path/to/unixsocket
  std::string _address;
  std::string _ip;
  int _port;
  bool _isConnected;
public:
  SocketClient(const char *address, Logger &logger);
  virtual ~SocketClient();
public:
  bool connect(const char *address = nullptr);
  virtual bool receive(uint8_t *buffer, size_t bufferSize,
      ssize_t &bufferLength);
  virtual bool send(uint8_t *buffer, size_t bufferLength);
  bool setAddress(const char *address);
};

class KniveSocketClient: public SocketClient {
protected:
  uint8_t *_buffer;
  size_t _bufferSize;
public:
  KniveSocketClient(const char *address, Logger &logger,
      size_t bufferKnife = 64 * 1024 + sizeof(KnifeHeader));
  virtual ~KniveSocketClient();
public:
  virtual bool receive(uint8_t *buffer, size_t bufferSize,
      size_t &bufferLength);
  bool send(scope_t scope, job_t job, uint8_t *buffer, size_t bufferLength);
  bool send(scope_t scope, job_t job, const std::string &data);
};
} // namespace
#endif /* OS_SOCKETCLIENT_HPP_ */
