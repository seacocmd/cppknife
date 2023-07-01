/*
 * Logger.hpp
 *
 *  Created on: 02.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_LOGGER_HPP_
#define CORE_LOGGER_HPP_

namespace cppknife {
/*
 *
 */
class Logger;
enum LogLevel {
  LV_UNDEF_LEVEL,
  LV_FATAL,
  LV_ERROR,
  LV_WARNING,
  LV_INFO,
  LV_SUMMARY,
  LV_DETAIL,
  LV_FINE,
  LV_FINEST,
  LV_DEBUG
};
/**
 * Manages the output of a logger.
 */
class Appender {
public:
  Appender(const char *name);
  virtual
  ~Appender();
  Appender(const Appender &other);
  Appender&
  operator =(const Appender &other);
public:
  bool
  accept(LogLevel level);
  inline const char* name() const {
    return _name.c_str();
  }
  virtual void
  say(Logger *logger, const std::string &message) = 0;
  void
  setLevel(LogLevel level);
protected:
  LogLevel _level;
  std::string _name;
};

/**
 * Logs messages to a <em>FILE</em> stream.
 */
class MemoryAppender: public Appender {
public:
  MemoryAppender(size_t maxLines = 1000);
  virtual
  ~MemoryAppender();
  MemoryAppender(const MemoryAppender &other);
  MemoryAppender&
  operator =(const MemoryAppender &other);
public:
  inline const std::string&
  at(size_t index) const {
    return index < _lines.size() ? _lines[index] : _empty;
  }
  inline void clear() {
    _lines.clear();
  }
  inline size_t count() const {
    return _lines.size();
  }
  inline const std::deque<std::string>&
  lines() const {
    return _lines;
  }
  std::string linesAsString() const;
  virtual void
  say(Logger *logger, const std::string &message);
protected:
  size_t _maxLines;
  std::deque<std::string> _lines;
  static const std::string _empty;
};

/**
 * Logs messages to a <em>FILE</em> stream.
 */
class StreamAppender: public Appender {
public:
  StreamAppender(const char *name, FILE *stream, bool usePrefix = true);
  virtual
  ~StreamAppender();
  StreamAppender(const StreamAppender &other);
  StreamAppender&
  operator =(const StreamAppender &other);
public:
  virtual void
  say(Logger *logger, const std::string &message);
protected:
  FILE *_stream;
  bool _usePrefix;
};

/**
 * Implements a class which is writing log messages to a file.
 */
class FileAppender: public StreamAppender {
public:
  FileAppender();
  virtual
  ~FileAppender();
  FileAppender(const FileAppender &other);
  FileAppender&
  operator =(const FileAppender &other);
public:
  void
  setConfig(const char *filename, int maxFiles, int maxSize);
private:
  void
  changeFile();

protected:
  std::string _filePattern;
  size_t _maxFiles;
  size_t _maxSize;
  size_t _currentSize;
  size_t _currentFileNo;
};
/** This class allows the logging of messages.
 * The output itself is done by so called appenders.
 * This allows a flexible handling of different media: files, dialog boxes, console...
 * The message can be contain placeholders which will be replaced
 * by computed data similar sprintf(), but in a typesafe way (@see sayF()).
 */
class Logger {
public:
  Logger(bool isGlobal = false);
  virtual
  ~Logger();
  Logger(const Logger &source);
  Logger&
  operator =(const Logger &source);
public:
  void
  addAppender(Appender *appender);
  void
  addStandardAppenders(bool console, const char *file, int fileCount = 5,
      int fileSize = 1000100);
  inline bool error(const char *message) {
    return say(LV_ERROR, message);
  }
  inline bool error(const std::string &message) {
    return say(LV_ERROR, message);
  }
  /** Returns the current level of the logging call.
   * @return 	The current log level.
   */
  inline LogLevel currentLevel(void) const {
    return _level;
  }
  inline const std::string&
  currentMessage() const {
    return _currentMessage;
  }
  Appender* findAppender(const char *name);
  virtual void
  finish(void);
  inline bool log(const char *message) {
    return say(LV_INFO, message);
  }
  inline bool log(const std::string &message) {
    return say(LV_INFO, message);
  }
  inline bool say(LogLevel level, const char *message) {
    return say(level, std::string(message));
  }
  bool
  say(LogLevel level, const std::string &message);
  const std::string&
  standardPrefix(char prefix = '\0');
  void setLevel(LogLevel level);
protected:
  std::string _currentMessage;
  std::vector<Appender*> _appenderList;
  std::string _standardPrefix;
  LogLevel _level;
  char _charPrefix;
public:
  static Logger*
  globalLogger();
  static void
  freeGlobalLogger();
  static Logger *_globalLogger;
};

/**
 * Connect classes with multiple base classes to use only one logger.
 */
class LoggerOwner {
public:
  virtual ~LoggerOwner() {
  }
public:
  virtual Logger*
  logger() = 0;
};
/**
 * Returns the global logger.
 *
 * @param the global logger
 */
inline Logger*
globalLogger() {
  return Logger::globalLogger();
}
/**
 * Creates a logger storing the messages in a line list.
 * @param maxLines If more lines are logged, the oldest lines will be thrown away.
 * @param level Only messages with this or a higher level will be stored.
 * @param isGlobalLogger <em>true</em>: the logger becomes the global logger.
 * @return A logger. Note: that object must be deleted after life.
 */
Logger* buildMemoryLogger(size_t maxLines = 100, LogLevel level = LV_INFO,
    bool isGlobalLogger = false);
} /* cppknife */

#endif /* CORE_LOGGER_HPP_ */
