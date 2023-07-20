/*
 * SocketServer.hpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#ifndef OS_SOCKETSERVER_HPP_
#define OS_SOCKETSERVER_HPP_

namespace cppknife {

typedef uint64_t knifeToken_t;

/**
 * @brief Very simple tester for authorization.
 * Overwrite it with your own secure version.
 *
 * Tokens are 64 bit data used in the client server communication.
 * Both partners (server and client) must put valid tokens in the transported data.
 * The receiver tests the token for validity to ensure that a authorized partner has sent it.
 * The token is valid in a certain time range: that is needed because the clocks of the partners may have different times.
 */
class AuthorizationChecker {
public:
  const int VALID_INTERVAL_SECONDS = 8;
public:
  /**
   * Builds a time related token
   */
  virtual knifeToken_t buildToken(time_t time, const char *context = nullptr);
  virtual bool checkToken(knifeToken_t token, time_t time, const char *context =
      nullptr);
};

class SocketTaskHandler;
/**
 * @brief The server part of communication with Unix sockets.
 */
class SocketServer {
protected:
  int _socketHandle;
  Logger &_logger;
  bool _isTcp;
  std::string _name;
public:
  SocketServer(const char *name, Logger &logger);
  virtual ~SocketServer();
public:
  const std::string name() const {
    return _name;
  }
  /**
   * Listens for jobs and process them.
   * @param handler
   */
  bool listen(SocketTaskHandler &handler, bool nonBlocking = true,
      int maxConnections = 64);
  /**
   * Sends an answer to the client.
   * @param buffer The buffer containing the data to send.
   * @param bufferLength The length of <em>buffer</em>.
   * @return <em>true</em>: Success.
   */
  bool send(uint8_t *buffer, size_t bufferLength);
};

/**
 * @brief Handles a request from a client in the server.
 */
class SocketTaskHandler {
protected:
  SocketServer &_server;
public:
  SocketTaskHandler(SocketServer &server);
  virtual ~SocketTaskHandler();
protected:
  virtual bool doIt(int socketHandle) = 0;
  virtual bool send(uint8_t *buffer, size_t bufferLength);
public:
  SocketServer& server() const {
    return _server;
  }
};
enum JobKind {
  JK_UNDEF, JK_STRING, JK_BINARY
};
typedef uint64_t scope_t;
typedef uint64_t job_t;
class KnifeTaskHandler;
/**
 * @brief The base class of an agent for <em>KnifeTaskHandler</em>.
 */
class JobAgent {
protected:
  JobKind _kind;
  std::string _name;
  std::string _scope;
  KnifeTaskHandler &_parent;
public:
  JobAgent(JobKind kind, const char *name, KnifeTaskHandler &parent);
  virtual ~JobAgent();
public:
  inline JobKind kind() const {
    return _kind;
  }
  /**
   * Returns whether the job can be processed by the agent.
   * @param scope The scope of the message.
   * @param job The job name of the message
   * @return <em>true</em>: the agent can process that message.
   */
  virtual bool isResponsible(scope_t scope, job_t job) = 0;
  /**
   * Sends an answer to the client.
   * @param buffer The data to send.
   * @param bufferLength The length of <em>buffer</em>.
   */
  virtual bool sendData(uint8_t *buffer, size_t bufferLength);
};
/**
 * @brief Base class for agents dealing with string data.
 */
class StringJobAgent: public JobAgent {
public:
  StringJobAgent(const char *name, KnifeTaskHandler &parent);
  virtual ~StringJobAgent();
public:
  /**
   * Process the data sent from the client.
   * @param scope Normally an agent process all tasks belonging to a certain scope.
   * @param task Specifies the task to do.
   * @param data The data to process.
   * @return <em>true</em>OK <false>: stop the server process.
   */
  virtual bool process(scope_t scope, job_t job,
      const std::string &data) = 0;
};
/**
 * @brief Base class for agents dealing with binary data.
 */
class BinaryJobAgent: public JobAgent {
public:
  BinaryJobAgent(const char *name, KnifeTaskHandler &parent);
  virtual ~BinaryJobAgent();
public:
  /**
   * Process the data sent from the client.
   * @param scope Normally an agent process all tasks belonging to a certain scope.
   * @param task Specifies the task to do.
   * @param buffer The binary data to process.
   * @param bufferLength The length of the <em>buffer</em>.
   * @return <em>true</em>OK <false>: stop the server process.
   */
  virtual bool process(scope_t scope, job_t job,
      const uint8_t *buffer, size_t bufferLength) = 0;
};
struct KnifeHeader {
  uint32_t _crc;
  uint32_t _dataLength;
  knifeToken_t _token;
  scope_t _scope;
  job_t _job;
};
/**
 * @brief Handles a request from a client in the server.
 *
 * Glossar:
 * <ul><li><strong>Agent</strong>: A high level class processing some data from a client request.</li>
 * <li><strong>Job</strong>: A exactly defined work requested by the client with one call and identified by an task id (string).</li>
 * <li><strong>Scope</strong>: A group of tasks processed by one agent</li>
 * </ul>
 *
 * Data format in the socket stream:
 * <pre>
 * <4-byte-crc32-little-endian><4-byte-lengh-data-little-endian><8-byte-authentification-token><8-byte-scope><8-byte-task><data>
 * </pre>
 *
 */
class KnifeTaskHandler: public SocketTaskHandler {
protected:
  uint8_t *_buffer;
  size_t _bufferSize;
  std::vector<JobAgent*> _agents;
public:
  KnifeTaskHandler(SocketServer &server, size_t bufferSize = 64 * 1024);
  virtual ~KnifeTaskHandler();
private:
  virtual bool doIt(int socketHandle);
public:
  void registerAgent(JobAgent *agent);
  bool send(uint8_t *buffer, size_t bufferLength);
};

/**
 * @brief A job agent which implements a time server (returns the date/time) and an echo server (returns the sent data).
 */
class BasicJobHandler: public StringJobAgent {
  static const uint64_t SCOPE_BASIC = 0x6261736963202020;
  static const uint64_t JOB_ECHO = 0x6563686f20202020;
  static const uint64_t JOB_TIME = 0x20202020656d6974;
public:
  BasicJobHandler(KnifeTaskHandler &parent);
  virtual ~BasicJobHandler();
public:
  virtual bool process(scope_t scope, job_t job, const std::string &data);
  virtual bool isResponsible(scope_t scope, job_t job);
};
}
#endif /* OS_SOCKETSERVER_HPP_ */
