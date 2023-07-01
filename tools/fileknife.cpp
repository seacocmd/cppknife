/*
 * fileknife.cpp
 *
 *  Created on: 09.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "fileknife.hpp"
namespace cppknife {
/**
 * Manages the "du" (Disk Usage) sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 */
int du(ArgumentParser &parser, Logger &logger) {
  DirEntryFilter filter;
  int level = 0;
  populateFilter(parser, filter);
  FsEntry *status;
  Traverser traverser(".", &filter, nullptr, &logger);
  auto count = parser.countValuesOf("base");
  bool baseIsPattern = false;
  for (size_t ix = 0; ix < count; ix++) {
    if (baseIsPattern) {
      delete filter._nodePatterns;
      filter._nodePatterns = nullptr;
    }
    auto base = parser.asString("base", ".", ix);
    if (strchr(base, ',') == nullptr) {
      traverser.changeBase(base);
    } else {
      traverser.changeBaseByPatterns(base, filter);
      baseIsPattern = true;
    }
    while ((status = traverser.nextFile(level)) != nullptr) {
      // do nothing
    }
  }
  std::string info;
  info.reserve(1024);
  appendString(info, "= ");
  logger.say(LV_SUMMARY, traverser.statisticAsString(info, true));
  return 0;
}

class Extrema {
public:
  Extrema(const char *name, bool reverse, size_t maxCount) :
      _name(name), _list(), _maxCount(maxCount), _reverse(reverse) {
    _list.reserve(maxCount);
  }
public:
  void addIfNeeded(const char *data) {
    if (_list.size() >= _maxCount) {
      _list.erase(_list.begin());
    }
    _list.push_back(data);
    sort(_list.begin(), _list.end());
  }
  bool isExtrema(const char *data) {
    bool rc = true;
    if (_list.size() >= _maxCount) {
      rc =
          _reverse ?
              strcmp(data, _list.back().c_str()) < 0 :
              strcmp(data, _list.front().c_str()) > 0;
    }
    return rc;
  }
  void show(Logger &logger, bool isDate) {
    logger.say(LV_INFO, formatCString("=== %s", _name));
    if (_reverse) {
      std::reverse(_list.begin(), _list.end());
    }
    for (auto entry : _list) {
      auto parts = splitCString(entry.c_str(), "\t");
      int64_t value = atoll(parts[0].c_str());
      if (isDate) {
        FileTime_t time2;
        setFiletime(time2, value, 0);
        auto time3 = filetimeToString(time2);
        logger.say(LV_INFO,
            formatCString("%s %s", time3.c_str(), parts[1].c_str()).c_str());
      } else {
        logger.say(LV_INFO,
            formatCString("%13.6f MB %s", static_cast<double>(value / 1E6),
                parts[1].c_str()).c_str());
      }
    }
  }
protected:
  const char *_name;
  std::vector<std::string> _list;
  size_t _maxCount;
  bool _reverse;
};

/**
 * Manages the "list" sub command.
 * @param parser Contains the program argument info.
 * @param logger The logging manager.
 */
int extrema(ArgumentParser &parser, Logger &logger) {
  auto base = parser.asString("base");
  DirEntryFilter filter;
  int level = 0;
  populateFilter(parser, filter);
  FsEntry *status;
  Traverser traverser(base, &filter, nullptr, &logger);
  char message[8192];
  auto count = parser.asInt("count");
  Extrema largest("largest", false, count);
  Extrema youngest("youngest", false, count);
  Extrema oldest("oldest", true, count);
  auto countPatterns = parser.countValuesOf("base");
  bool baseIsPattern = false;
  for (size_t ix = 0; ix < countPatterns; ix++) {
    if (baseIsPattern) {
      delete filter._nodePatterns;
      filter._nodePatterns = nullptr;
    }
    auto base = parser.asString("base", ".", ix);
    if (strchr(base, ',') == nullptr) {
      traverser.changeBase(base);
    } else {
      traverser.changeBaseByPatterns(base, filter);
      baseIsPattern = true;
    }
    while ((status = traverser.nextFile(level)) != nullptr) {
      std::string format;
      auto fileName = status->fullName();
      formatOnBuffer(message, sizeof message, "%012lld\t%s",
          status->modified()->tv_sec, fileName);
      if (youngest.isExtrema(message)) {
        youngest.addIfNeeded(message);
      }
      if (oldest.isExtrema(message)) {
        oldest.addIfNeeded(message);
      }
      if (status->isRegular()) {
        formatOnBuffer(message, sizeof message, "%012lld\t%s",
            status->fileSize(), fileName);
        if (largest.isExtrema(message)) {
          largest.addIfNeeded(message);
        }
      }
    }
  }
  largest.show(logger, false);
  oldest.show(logger, true);
  youngest.show(logger, true);
  std::string info;
  info.reserve(1024);
  appendString(info, "= ");
  logger.say(LV_SUMMARY, traverser.statisticAsString(info, true));
  return 0;
}
const char* wcOutput(const char *format, char *buffer, size_t bufferSize,
    size_t lines, size_t words, size_t bytes, size_t maxLength,
    const char *fullname) {
  const char *start = format;
  const char *end = nullptr;
  size_t rest = bufferSize - 1;
  char *endOfBuffer = buffer;
  ptrdiff_t length = 0;
  while ((end = strchr(start, '%')) != nullptr) {
    length = end - start;
    if (endOfBuffer - buffer + length + 20
        >= static_cast<ptrdiff_t>(bufferSize)) {
      throw InternalError("wcOutput: format buffer to small:", nullptr,
          static_cast<int>(bufferSize));
    }
    memcpy(endOfBuffer, start, length);
    endOfBuffer += length;
    const char *toCopy = nullptr;
    rest = bufferSize - (endOfBuffer - buffer) - 1;
    switch (end[1]) {
    case '%':
      *endOfBuffer = '%';
      break;
    case 'l':
      formatOnBuffer(endOfBuffer, rest, "%u", lines);
      endOfBuffer += strlen(endOfBuffer);
      break;
    case 'w':
      formatOnBuffer(endOfBuffer, rest, "%u", words);
      endOfBuffer += strlen(endOfBuffer);
      break;
    case 'b':
      formatOnBuffer(endOfBuffer, rest, "%u", bytes);
      endOfBuffer += strlen(endOfBuffer);
      break;
    case 'L':
      formatOnBuffer(endOfBuffer, rest, "%u", maxLength);
      endOfBuffer += strlen(endOfBuffer);
      break;
    case 'f':
      toCopy = fullname;
      break;
    case '\0':
      *endOfBuffer++ = '%';
      break;
    default:
      *endOfBuffer += '%';
      *endOfBuffer += end[1];
      break;
    }
    if (toCopy != nullptr) {
      length = strlen(toCopy);
      if (static_cast<size_t>(length) > rest) {
        throw InternalError("wcOutput(2): format buffer to small:", nullptr,
            static_cast<int>(bufferSize));
      }
      memcpy(endOfBuffer, toCopy, length);
      endOfBuffer += length;
    }
    start = end + 2;
  }
  if ((length = start - format) > 0) {
    if (static_cast<size_t>(length) > rest) {
      throw InternalError("wcOutput(3): format buffer to small:", nullptr,
          static_cast<int>(bufferSize));
    }
    memcpy(endOfBuffer, start, length);
    endOfBuffer += length;
  }
  *endOfBuffer = '\0';
  return buffer;
}
/**
 * Manages the "wc" (Word Count) sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 */
int wc(ArgumentParser &parser, Logger &logger) {
  DirEntryFilter filter;
  int level = 0;
  populateFilter(parser, filter);
  filter._types = FsEntry::TC_TEXT;
  FsEntry *status;
  char buffer[8192];
  LineList lines(8128);
  LineAgent lineAgent(&logger);
  size_t length;
  const char *line;
  const char *wordSeparators = " \t";
  size_t bytesTotal = 0;
  size_t wordsTotal = 0;
  size_t linesTotal = 0;
  size_t maxLengthTotal = 0;
  size_t files = 0;
  const char *format = parser.asString("format");
  if (format[0] == '\0') {
    if (parser.asBool("lines")) {
      format = "%l %f";
    } else if (parser.asBool("words")) {
      format = "%w %f";
    } else if (parser.asBool("bytes")) {
      format = "%b %f";
    } else if (parser.asBool("max-line-length")) {
      format = "%L %f";
    } else {
      format = "%l %w %b %f";
    }
  }
  bool countWords = strstr(format, "%w") != nullptr;

  Traverser traverser(".", &filter, nullptr, &logger);
  auto count = parser.countValuesOf("base");
  bool baseIsPattern = false;
  for (size_t ix = 0; ix < count; ix++) {
    if (baseIsPattern) {
      delete filter._nodePatterns;
      filter._nodePatterns = nullptr;
    }
    auto base = parser.asString("base", ".", ix);
    if (strchr(base, ',') == nullptr) {
      traverser.changeBase(base);
    } else {
      traverser.changeBaseByPatterns(base, filter);
      baseIsPattern = true;
    }
    while ((status = traverser.nextFile(level)) != nullptr) {
      size_t bytes = 0;
      size_t words = 0;
      size_t lines = 0;
      size_t maxLength = 0;
      if (lineAgent.openFile(status->accessFullName())) {
        bool abort = false;
        while ((line = lineAgent.nextLine(length)) != nullptr) {
          if (lineAgent.hasBinaryData()) {
            abort = true;
            break;
          }
          if (length > maxLength) {
            maxLength = length;
          }
          lines++;
          bytes += length + 1;
          if (countWords) {
            const char *ptr = line + strspn(line, wordSeparators);
            size_t len = 0;
            while ((len = strcspn(ptr, wordSeparators)) != 0) {
              words++;
              ptr += len;
              ptr += strspn(ptr, wordSeparators);
            }
          }
        }
        if (!abort) {
          files++;
          bytesTotal += bytes;
          wordsTotal += words;
          linesTotal += lines;
          if (maxLength > maxLengthTotal) {
            maxLengthTotal = maxLength;
          }
          logger.say(LV_INFO,
              wcOutput(format, buffer, sizeof buffer - 1, lines, words, bytes,
                  maxLength, status->fullName()));
        }
      }
    }
  }
  if (files > 0) {
    const char *titleSummary = "<summary>";
    logger.say(LV_INFO,
        wcOutput(format, buffer, sizeof buffer - 1, linesTotal, wordsTotal,
            bytesTotal, maxLengthTotal, titleSummary));
  }
  std::string info;
  info.reserve(1024);
  appendString(info, "= ");
  logger.say(LV_SUMMARY, traverser.statisticAsString(info, true));
  return 0;
}

/**
 * Manages the "list" sub command.
 * @param parser Contains the program argument info.
 * @param logger The logging manager.
 */
int list(ArgumentParser &parser, Logger &logger) {
  DirEntryFilter filter;
  int level = 0;
  populateFilter(parser, filter);
  FsEntry *status;
  Traverser traverser("", &filter, nullptr, &logger);
  char buffer[8192];
  auto count = parser.countValuesOf("base");
  bool baseIsPattern = false;
  for (size_t ix = 0; ix < count; ix++) {
    if (baseIsPattern) {
      delete filter._nodePatterns;
      filter._nodePatterns = nullptr;
    }
    auto base = parser.asString("base", ".", ix);
    if (strchr(base, ',') == nullptr) {
      traverser.changeBase(base);
    } else {
      traverser.changeBaseByPatterns(base, filter);
      baseIsPattern = true;
    }
    while ((status = traverser.nextFile(level)) != nullptr) {
      std::string format;
      auto fileTime = status->filetimeAsString();
      auto fileName = status->fullName();
      if (status->isDirectory()) {
        // 2022.10.22 12:44:49 2048.123456 MB /home/my.txt
        logger.log(
            formatOnBuffer(buffer, sizeof buffer, "d%s      directory %s",
                fileTime.c_str(), fileName));
      } else if ((status->type() & (FsEntry::TF_LINK_DIR | FsEntry::TF_LINK))
          != 0) {
        logger.log(
            formatOnBuffer(buffer, sizeof buffer, "%c%s %11.6f MB %s -> %s",
                status->typeAsChar(), fileTime.c_str(),
                status->fileSize() / 1E6, fileName,
                status->linkReference().c_str()));
      } else {
        logger.log(
            formatOnBuffer(buffer, sizeof buffer, "%c%s %11.6f MB %s",
                status->typeAsChar(), fileTime.c_str(),
                status->fileSize() / 1E6, fileName));
      }
    }
  }
  std::string info;
  info.reserve(1024);
  appendString(info, "= ");
  logger.say(LV_SUMMARY, traverser.statisticAsString(info, true));
  return 0;
}

int fileKnife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  double start = nowAsDouble();
  int rc = 0;
  ArgumentParser parser("fileknife", logger,
      "Version " __DATE__ "\n    A universal tool working on files and directories");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.addMode("mode", "What should be done:", "du,extrema,list,wc");
  ArgumentParser listParser("list", logger, "Lists some files/directories");
  parser.addSubParser("mode", "list", listParser);
  addTraverserOptions(listParser);
  ArgumentParser extramaParser("extrema", logger,
      "Creates a statistic about the youngest/oldest/largest files");
  extramaParser.add("--count", "-c", DT_NAT,
      "Maximal count of entries per extremum will be collected", "20", "5|100");
  parser.addSubParser("mode", "extrema", extramaParser);
  addTraverserOptions(extramaParser);
  ArgumentParser wcParser("wc", logger, "Counts words, lines and characters");
  wcParser.add("--bytes", "-b", DT_BOOL, "Only count bytes", "F");
  wcParser.add("--lines", "-l", DT_BOOL, "Only count lines", "F");
  wcParser.add("--words", "-w", DT_BOOL, "Only count words", "F");
  wcParser.add("--max-line-length", "-L", DT_BOOL,
      "Only return maximal line length", "F");
  wcParser.add("--format", "-f", DT_STRING,
      "A template for the output format: placeholders: %% %w(ords) %l(ines) %c(chars) %M(axlength) %f(ullname) %n(ode) %p(ath)",
      "", "'lines: %l max-line: %L percent: %%");
  wcParser.add("--summary", "-y", DT_STRING,
      "The replacement of the filename in the summary line"
          "<summary>", "=== summary");
  parser.addSubParser("mode", "wc", wcParser);
  addTraverserOptions(wcParser);
  ArgumentParser duParser("du", logger, "Counts files, directories, bytes");
  parser.addSubParser("mode", "du", duParser);
  addTraverserOptions(duParser);
  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    if (parser.isMode("mode", "list")) {
      rc = list(parser, *logger);
    } else if (parser.isMode("mode", "du")) {
      rc = du(parser, *logger);
    } else if (parser.isMode("mode", "extrema")) {
      rc = extrema(parser, *logger);
    } else if (parser.isMode("mode", "wc")) {
      rc = wc(parser, *logger);
    } else {
      printf("%s\n", parser.usage("unknown mode", nullptr, false).c_str());
    }

  }
  if (verbose) {
    logger->say(LV_SUMMARY,
        timeDifferenceToString(nowAsDouble() - start,
            "= runtime: %hh%mm%s.%3s"));
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}
}
