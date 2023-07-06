/*
 * textknife.cpp
 *
 *  Created on: 07.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include "../os/os.hpp"
#include "textknife.hpp"
namespace cppknife {

void examples() {
  printf(
      R"""((# Adapt standard values for the PHP configuration:
textknife adapt --template=php /etc/php/8.2/php*.ini
# Show the adapting parameters:
cat /etc/cppknife/adapt/php.conf

# Adapt a configuration file: change the variable "max_memory" if it exists or set it otherwise.
textknife adapt --anchor=/#.*max_memory/i '--pattern=/^max_memory\s*=/' "--replacement=max_memory=512k" /etc/php/8.2/fpm/php.ini

# Find the strings in all sourcefiles (*.cpp and *.hpp) in the directory /home/ws and subdirs:
textknife strings /home/ws/*.cpp,*.hpp
# Find strings in files older than 7 days and maximum nesting depth of 3:
textknife strings /home/ws/*.cpp,*.hpp --days=+7 --max-depth=3

# Describe the usage:
textknife --help
)""");
}

std::string ensurePhpConfig(Logger *logger) {
  std::string rc = "/etc/cppknife/adapt/php";
  if (!fileExists(rc.c_str())) {
    writeText(rc.c_str(),
        R"""(memory_limit = 2048M
upload_max_filesize = 512M
max_file_uploads = 100
post_max_size = 512M
max_execution_time = 600
max_input_time = 600
default_socket_timeout = 600
session.save_handler = redis
_anchor_.session.save_handler = ^\[Session\]
session.save_path = "tcp://127.0.0.1:6379"
_anchor_.session.path = ^session.save_handler
opcache.enable=1
_anchor_.opcache.enable = ^\[opcache\]
opcache.memory_consumption=1024
_anchor_.opcache.memory_consumption = opcache.enable
opcache.interned_strings_buffer=512
_anchor_.opcache.interned_strings_buffer = ^opcache.memory_consumption
)""",
        -1, logger);
    if (!fileExists(rc.c_str())) {
      logger->say(LV_ERROR, formatCString("cannot create: %s", rc.c_str()));
    } else {
      logger->say(LV_DETAIL, formatCString("= created: %s", rc.c_str()));
    }
  }
  return rc;
}

void initTemplates(Logger *logger) {
  ensurePhpConfig(logger);
}
class AdaptCommandHandler: public CommandHandler {
private:
  Configuration *_templateConfiguration;
  std::regex _pattern;
  const char *_replacement;
  std::regex _anchor;
  bool _hasAnchor;
  bool _above;
public:
  AdaptCommandHandler(ArgumentParser &argumentParser, Logger *logger) :
      CommandHandler(argumentParser, logger), _templateConfiguration(nullptr), _pattern(), _replacement(), _anchor(), _hasAnchor(
          false), _above(false) {
  }
  virtual ~AdaptCommandHandler() {
    delete _templateConfiguration;
    _templateConfiguration = nullptr;
  }
  virtual bool check() {
    bool rc = false;
    do {
      auto theTemplate = _argumentParser.asString("template", nullptr);
      if (theTemplate != nullptr) {
        if (strcmp(theTemplate, "php") == 0) {
          ensurePhpConfig(_logger);
        }
        auto filename = std::string("/etc/cppknife/adapt/") + theTemplate;
        if (!fileExists(filename.c_str())) {
          _logger->error(
              formatCString("template file not found: %s", filename.c_str()));
          break;
        }
        if (_argumentParser.asString("anchor", "")[0] != '\0') {
          _logger->error("Use exactly one of --template and --anchor");
          break;
        }
        if (_argumentParser.asString("pattern", "")[0] != '\0') {
          _logger->error("Use exactly one of --template and --pattern");
          break;
        }
        if (_argumentParser.asString("replacement", "")[0] != '\0') {
          _logger->error("Use exactly one of --template and --replacement");
          break;
        }
        _templateConfiguration = new SimpleConfiguration(filename.c_str(),
            _logger);
      } else {
        _replacement = _argumentParser.asString("replacement", nullptr);
        if (_replacement == nullptr) {
          _logger->error("missing --replacement");
          break;
        }
        if (_argumentParser.asString("pattern")[0] == '\0') {
          _logger->error("missing --pattern");
          break;
        }
        _pattern = _argumentParser.asRegExpr("pattern");
        _anchor = _argumentParser.asRegExpr("anchor");
      }
      rc = true;
    } while (false);
    _above = _argumentParser.asBool("above-anchor");
    return rc;
  }
  virtual bool oneFile() {
    bool rc = true;
    auto filename = _status->accessFullName();
    LineList lines;
    lines.readFromFile(filename);
    bool hasChanged = false;
    if (_templateConfiguration != nullptr) {
      hasChanged = lines.adaptFromConfiguration(*_templateConfiguration,
          *_logger, _above);
    } else {
      ChangeType changeType = lines.adapt(_pattern, _replacement,
          _hasAnchor ? &_anchor : nullptr, _above);
      hasChanged = changeType == CT_CHANGED || changeType == CT_APPENDED
          || changeType == CT_INSERTED;
    }
    if (hasChanged) {
      lines.writeToFile(filename);
    }
    return rc;
  }
};

class ReplaceCommandHandler: public CommandHandler {
private:
  std::regex _pattern;
  std::string _replacement;
public:
  ReplaceCommandHandler(ArgumentParser &argumentParser, Logger *logger) :
      CommandHandler(argumentParser, logger), _replacement() {
    _replacement = argumentParser.asString("replacement");
  }
  virtual ~ReplaceCommandHandler() {
  }
  virtual bool check() {
    bool rc = true;
    return rc;
  }

  virtual bool oneFile() {
    bool rc = true;
    auto pattern = _argumentParser.asRegExpr("pattern");
    auto filename = _status->accessFullName();
    FileLinesStream stream(filename, *_logger);
    std::string line;
    bool found = false;
    while (stream.fetch(line)) {
      if (std::regex_search(line, pattern)) {
        found = true;
        break;
      }
    }
    return rc;
  }
}
;

class StringsCommandHandler: public CommandHandler {
private:
  std::map<std::string, int> _strings;
public:
  StringsCommandHandler(ArgumentParser &argumentParser, Logger *logger) :
      CommandHandler(argumentParser, logger), _strings() {
  }
  virtual ~StringsCommandHandler() {
  }
  virtual bool check() {
    bool rc = true;
    return rc;
  }

  virtual bool oneFile() {
    bool rc = true;
    auto filename = _status->accessFullName();
    const int MAX = 0xffff;
    char buffer[MAX + 1];
    buffer[MAX] = '\0';
    FILE *input = fopen(filename, "r");
    const char *ptr = nullptr;
    const char *start = nullptr;
    char cc = 0;
    while (fgets(buffer, sizeof buffer, input) != nullptr) {
      char delimiter = '\0';
      ptr = buffer;
      while ((cc = *ptr++) != '\0') {
        if (cc == '\\') {
          if (*ptr++ == '\0') {
            break;
          } else {
            continue;
          }
        }
        if (start == nullptr && (cc == '"' or cc == '\'')) {
          start = ptr - 1;
          delimiter = cc;
        } else if (start != nullptr && cc == delimiter) {
          std::string contents(start, ptr - start);
          if (_strings.find(contents) != _strings.end()) {
            _strings[contents]++;
          } else {
            _strings[contents] = 1;
          }
          start = nullptr;
          delimiter = '\0';
        }
      }
    }
    return rc;
  }
  void write(const char *filename, Logger &logger) {
    FILE *output = nullptr;
    if (strcmp(filename, "-") == 0) {
      output = stdout;
    } else {
      output = fopen(filename, "w");
    }
    if (output == nullptr) {
      logger.say(LV_ERROR, formatCString("cannot open %s", filename));
    } else {
      auto keys = keysOfMap<std::string, int>(_strings);
      std::sort(keys.begin(), keys.end());
      for (auto key : keys) {
        fprintf(output, "%s\n", key.c_str());
      }
      if (strcmp(filename, "-") != 0) {
        fclose(output);
      }
    }
  }
}
;

/**
 * Manages the "adapt" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int adapt(ArgumentParser &parser, Logger &logger) {
  AdaptCommandHandler handler(parser, &logger);
  int rc = handler.run("source");
  return rc;
}

/**
 * Manages the "adapt" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int replace(ArgumentParser &parser, Logger &logger) {
  ReplaceCommandHandler handler(parser, &logger);
  int rc = handler.run("source");
  return rc;
}

/**
 * Manages the "strings" sub command.
 * @param parser Contains the program argument info.
 * @param logger Manages the output.
 * @return 0: success Otherwise: the exit code.
 */
int strings(ArgumentParser &parser, Logger &logger) {
  StringsCommandHandler handler(parser, &logger);
  int rc = handler.run("source");
  handler.write(parser.asString("output"), logger);
  return rc;
}

int textKnife(int argc, char **argv, Logger *loggerExtern) {
  auto logger =
      loggerExtern == nullptr ?
          buildMemoryLogger(200, LV_SUMMARY) : loggerExtern;
  double start = nowAsDouble();
  int rc = 0;
  ArgumentParser parser("textknife", logger,
      "Managing text on files in a directory tree");
  parser.add("--log-level", "-l", DT_NAT,
      "Log level: 1=FATAL 2=ERROR 3=WARNING 4=INFO 5=SUMMARY 6=DETAIL 7=FINE 8=DEBUG",
      "5");
  parser.add("--verbose", "-v", DT_BOOL, "Show more information");
  parser.add("--examples", nullptr, DT_BOOL, "Show usage examples", "false");
  parser.addMode("mode", "What should be done:", "adapt,string");

  ArgumentParser adaptParser("adapt", logger, "Adapts configuration files.");
  parser.addSubParser("mode", "adapt", adaptParser);
  adaptParser.add("--template", "-t", DT_STRING, "Use a template: php", nullptr,
      "php");
  adaptParser.add("--pattern", "-P", DT_REGEXPR,
      "The pattern describing the key.", "", "/^max_memory\\s*=");
  adaptParser.add("--replacement", "-R", DT_STRING,
      "The replacement of the pattern", nullptr, "max_memory = 512k");
  adaptParser.add("--anchor", "-a", DT_REGEXPR,
      "If pattern is not found the replacement is inserted near that anchor.",
      "", "/#.*max_memory");
  adaptParser.add("--above-anchor", "-A", DT_BOOL,
      "Insert above the anchor line.", "false");
  adaptParser.add("source", nullptr, DT_FILE_PATTERN,
      "A directory with or without a list of file patterns.", ".",
      "/etc/php/8.4/*.conf", true);
  addTraverserOptions(adaptParser);
#ifdef REPLACE
  ArgumentParser replaceParser("replace", logger,
      "Replaces a pattern in files.");
  parser.addSubParser("mode", "replace", replaceParser);
  replaceParser.add("--pattern", "-P", DT_REGEXPR, "The pattern to replace.", "",
      "/Jenny Smith/i");
  replaceParser.add("--replacement", "-R", DT_STRING,
      "The replacement of the pattern", nullptr, "Jenny Miller");
  addTraverserOptions(replaceParser);
#endif

  ArgumentParser stringsParser("strings", logger,
      "Fetches the strings delimited by ' or \" from files.");
  parser.addSubParser("mode", "strings", stringsParser);
  stringsParser.add("source", nullptr, DT_FILE_PATTERN,
      "A directory with or without a list of file patterns.", ".", nullptr,
      true);
  stringsParser.add("--output", "-o", DT_STRING,
      "The strings will be put there, one per line, sorted. If '-' stdout is used.",
      "-");
  addTraverserOptions(stringsParser);

  auto verbose = parser.asBool("verbose");
  ArgVector argVector(argc, argv);
  if (!parser.parseAndCheck(argVector)) {
    rc = 2;
  } else if (parser.asBool("examples")) {
    examples();
  } else {
    auto level = static_cast<LogLevel>(parser.asInt("log-level", LV_SUMMARY));
    logger->setLevel(level);
    if (parser.isMode("mode", "adapt")) {
      rc = adapt(parser, *logger);
    } else if (parser.isMode("mode", "replace")) {
      rc = replace(parser, *logger);
    } else if (parser.isMode("mode", "strings")) {
      rc = strings(parser, *logger);
    } else {
      printf("%s\n", parser.usage("unknown mode", nullptr, false).c_str());
    }
    if (verbose) {
      logger->say(LV_SUMMARY,
          timeDifferenceToString(nowAsDouble() - start,
              "= runtime: %hh%mm%s.%3s"));
    }
  }
  if (logger != loggerExtern) {
    delete logger;
  }
  return rc;
}

} /* namespace cppknife */
