/*
 * Logger.cpp
 *
 *  Created on: 02.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../core/core.hpp"
#include "../os/os.hpp"
namespace cppknife {

const std::string MemoryAppender::_empty;

Logger *Logger::_globalLogger = NULL;

/** Returns the global logger.
 *
 * If no global logger exists a standard logger will be initialized
 * and set to the global logger.
 *
 * @return The global logger.
 */
Logger*
Logger::globalLogger() {
  if (_globalLogger == NULL) {
    _globalLogger = new Logger();
  }
  return _globalLogger;
}

Appender*
Logger::findAppender(const char *name) {
  Appender *rc = nullptr;
  for (auto appender : _appenderList) {
    if (strcmp(appender->name(), name) == 0) {
      rc = appender;
      break;
    }
  }
  return rc;
}

/**
 * Frees the resources of the global logger (if it exists).
 */
void Logger::freeGlobalLogger() {
  if (_globalLogger != NULL) {
    delete _globalLogger;
    _globalLogger = NULL;
  }
}
/** Constructor.
 */
Appender::Appender(const char *name) :
    _level(LV_INFO), _name(name) {
}
/** Destructor.
 */
Appender::~Appender() {

}
Appender::Appender(const Appender &other) :
    _level(other._level) {
}
Appender&
Appender::operator =(const Appender &other) {
  _level = other._level;
  return *this;
}
/** Tests whether the log message should be logged.
 *
 * @param mode		The specification of the current log message.
 *
 * @return true: The
 */
bool Appender::accept(LogLevel mode) {
  return _level >= mode;
}
/** Sets the mode parameters.
 *
 * @param level Only messages with a level equal or higher than that will be logged.
 */
void Appender::setLevel(LogLevel level) {
  _level = level;
}

/** Constructor.
 *
 * @param name The name of the appender.
 * @param stream An open stream used for the output.
 * @param usePrefix <em>true</em>: the appender adds a given prefix to each logged line.
 */
StreamAppender::StreamAppender(const char *name, FILE *stream, bool usePrefix) :
    Appender(name), _stream(stream), _usePrefix(usePrefix) {
}
/** Destructor.
 */
StreamAppender::~StreamAppender() {
  if (_stream != NULL) {
    if (_stream != stdout && _stream != stderr)
      fclose(_stream);
    _stream = NULL;
  }
}

StreamAppender::StreamAppender(const StreamAppender &other) :
    Appender(other), _stream(NULL), _usePrefix(other._usePrefix) {
}

StreamAppender&
StreamAppender::operator =(const StreamAppender &other) {
  Appender::operator =(other);
  _name = other._name;
  _stream = NULL;
  _usePrefix = other._usePrefix;
  return *this;
}

/** Puts a message into the internal stream.
 *
 * @param logger		The logger calling the method.
 * @param message		The message to write.
 */
void StreamAppender::say(Logger *logger, const std::string &message) {
  if (_stream != NULL) {
    if (_usePrefix) {
      fputs(logger->standardPrefix().c_str(), _stream);
    }
    if (!message.empty())
      fputs(message.c_str(), _stream);
    else
      fputs(logger->currentMessage().c_str(), _stream);
    fputc('\n', _stream);
    fflush(_stream);
  }
}
/** Constructor.
 */
FileAppender::FileAppender() :
    StreamAppender("FileAppender", nullptr), _filePattern("logger.*"), _maxFiles(
        10), _maxSize(1000000), _currentSize(0), _currentFileNo(0) {
}
/** Destructor.
 */
FileAppender::~FileAppender() {
}

/** Sets the configuration data.
 *
 * @param pattern	A filename pattern. Should contain a '*' which will replaced
 * 					with a version number.
 * @param maxFiles	Maximum number of logfiles. If more files exist they will be deleted.
 * @param maxSize	Maximum size of one logfile.
 *
 */
void FileAppender::setConfig(const char *pattern, int maxFiles, int maxSize) {
  const char *placeholder = "%04d";
  _maxFiles = maxFiles < 2 ? 2 : maxFiles;
  _maxSize = maxSize < 10 ? 10 : maxSize;

  const char *first = strchr(pattern, '*');
  std::string patternString(pattern);
  std::string placeholderString(placeholder);
  size_t len;
  if (first == NULL) {
    _filePattern = formatCString("%s.%s.log", pattern, placeholder);
  } else {
    len = first - pattern;
    _filePattern = pattern;
    _filePattern = _filePattern.replace(len, 1, placeholder);
  }

  PathInfo info;
  splitPath(_filePattern.c_str(), info);
  auto path = info._path;
  ensureDirectory(path.c_str());
  if (!isDirectory(path.c_str())) {
    throw InternalError("Not a valid directory:", path.c_str());
  } else {
    auto fn = joinPath(nullptr, info._name.c_str(), info._extension.c_str());
    int ix = fn.find(placeholder);
    fn.replace(ix, 4, "0001");
    ///@todo:
    /*
     // Looking for the current logfile:
     if (!dir.findFirst (fn.str (), true))
     {
     fn.splice (ix, 8, "0001", 4);
     m_currentFileNo = 1;
     }
     else
     {
     dir.findYoungest (fn);
     m_currentFileNo = fn.atoi (ix, 4);
     }
     struct stat info;
     ReByteArray fullname;
     dir.fullpath (fullname, fn.str ());
     #ifdef __WIN32__
     #define lstat stat
     #endif
     int rc = lstat (fullname.str (), &info);

     if (rc == 0 && (m_currentSize = info.st_size) > m_maxSize)
     changeFile ();
     else
     {
     m_stream = fopen (fullname.str (), "a");
     if (m_stream == NULL)
     {
     fprintf (stderr,
     "+++ ReFileAppender::setConfig(): cannot open: '%s'\n",
     fullname.str ());
     //assert(m_stream != NULL);
     }
     }
     */
  }
}

FileAppender::FileAppender(const FileAppender &other) :
    StreamAppender(other), _filePattern(other._filePattern), _maxFiles(
        other._maxFiles), _maxSize(other._maxSize), _currentSize(0), _currentFileNo(
        -1) {
}

FileAppender&
FileAppender::operator =(const FileAppender &other) {
  StreamAppender::operator =(other);
  _filePattern = other._filePattern;
  _maxFiles = other._maxFiles;
  _maxSize = other._maxSize;
  _currentSize = 0;
  _currentFileNo = -1;
  return *this;
}

/** Opens a new file for logging.
 */
void FileAppender::changeFile() {
  if (_stream != NULL) {
    fclose(_stream);
    _stream = NULL;
  }

  ///@ToDo
  /*
   char filename[512];
   const size_t FILENO_UBOUND = 10000;
   // Delete some of the oldest files to make room for the new:
   for (int ii = 0; ii < int (FILENO_UBOUND - m_maxFiles); ii++)
   {
   int fileNo = (FILENO_UBOUND + m_currentFileNo - m_maxFiles - ii)
   % m_maxFiles;
   _snprintf (filename, sizeof filename, m_filePattern, fileNo);
   struct stat info;
   if (lstat (filename, &info) == 0)
   _unlink (filename);
   else
   break;
   }
   if (++m_currentFileNo >= FILENO_UBOUND)
   m_currentFileNo = 0;
   _snprintf (filename, sizeof filename, m_filePattern, m_currentFileNo);
   m_stream = fopen (filename, "w");
   m_currentSize = 0;
   */
}

/** Constructor.
 *
 * @param isGlobal	<code>true</code>: the local will be the global logger
 */
Logger::Logger(bool isGlobal) :
    _currentMessage(), _appenderList(), _standardPrefix(), _level(LV_INFO) {
  _appenderList.push_back(new StreamAppender("console", stdout, false));
  if (isGlobal) {
    delete _globalLogger;
    _globalLogger = this;
  }
  _standardPrefix[0] = '\0';
  setLevel(LV_INFO);
}
/** Destructor.
 */
Logger::~Logger() {
  finish();
}
/** Issues a log message.
 *
 * The message will be put to all appenders which accept the log message.
 *
 * @param mode		the mode controlling the issuing of the logging.
 * 					this mode will be compared to the settings of the appenders
 * @param message	the message to log
 *
 * @return true
 */
bool Logger::say(LogLevel mode, const std::string &message) {
  _level = mode;
  _standardPrefix[0] = '\0';

  for (auto appender : _appenderList) {
    if (appender->accept(mode))
      appender->say(this, message);
  }
  //m_charPrefix = safePrefix;
  return true;
}

/** Adds an appender to the appender list.
 *
 * @param appender		the appender.
 */
void Logger::addAppender(Appender *appender) {
  _appenderList.push_back(appender);
}

void Logger::setLevel(LogLevel level) {
  _level = level;
  for (auto appender : _appenderList) {
    appender->setLevel(level);
  }
}
/** Returns the standard prefix of a log line.
 *
 * The standard prefix is log class, date, time, and location).
 * Most of the appenders will use this prefix.
 * It will be build on demand.
 *
 * @return the standard prefix
 */
const std::string&
Logger::standardPrefix(char charPrefix) {
  if (_standardPrefix.empty()) {
    char cc;
    switch (_level) {
    case LV_FATAL:
      cc = '#';
      break;
    case LV_ERROR:
      cc = '!';
      break;
    case LV_WARNING:
      cc = '+';
      break;
    case LV_INFO:
      cc = ' ';
      break;
    case LV_DEBUG:
      cc = '_';
      break;
    default:
      cc = ' ';
      break;
    }
    _standardPrefix[0] = cc;
    if (charPrefix == '\0')
      charPrefix = _charPrefix;
    if (charPrefix != '\0')
      _standardPrefix[1] = charPrefix;
    time_t now = time(NULL);
    struct tm *now1 = localtime(&now);
    char buffer[255];
    strftime(buffer, sizeof buffer, "%Y.%m.%d %H:%M:%S ", now1);
    appendString(_standardPrefix, buffer);
  }
  return _standardPrefix;
}
/** Terminates the chain of <code>arg()</code> calls.
 *
 * This method puts the data to the (accepting) appenders.
 *
 */
void Logger::finish(void) {
  for (auto item : _appenderList) {
    delete item;
  }
  _appenderList.clear();
  if (_globalLogger == this)
    _globalLogger = NULL;
}

MemoryAppender::MemoryAppender(size_t maxLines) :
    Appender("memory"), _maxLines(maxLines), _lines() {
}

MemoryAppender::~MemoryAppender() {
}

MemoryAppender::MemoryAppender(const MemoryAppender &other) :
    Appender(other), _maxLines(other._maxLines), _lines(other._lines) {
}

MemoryAppender&
MemoryAppender::operator =(const MemoryAppender &other) {
  Appender::operator =(other);
  _maxLines = other._maxLines;
  _lines = other._lines;
  return *this;
}

std::string MemoryAppender::linesAsString() const {
  int count = _lines.size();
  for (auto line : _lines) {
    count += line.size();
  }
  std::string rc;
  rc.reserve(count);
  for (auto line : _lines) {
    if (!rc.empty()) {
      rc += "\n";
    }
    rc += line;
  }
  return rc;
}

void MemoryAppender::say(Logger *logger, const std::string &message) {
  if (_lines.size() >= _maxLines) {
    _lines.pop_front();
  }
  _lines.push_back(message);
}

Logger*
buildMemoryLogger(size_t maxLines, LogLevel level, bool isGlobalLogger) {
  Logger *rc = new Logger(isGlobalLogger);
  rc->addAppender(new MemoryAppender(maxLines));
  return rc;
}

} /* cppknife */
