/*
 * ToolsCommons.cpp
 *
 *  Created on: 20.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "ToolsCommons.hpp"

namespace cppknife {

void addTraverserOptions(ArgumentParser &parser) {
  parser.add("base", nullptr, DT_FILE_PATTERN,
      "The start directory or a list of file patterns delimited by ',', first with path. Preceding '-' defines a NOT pattern",
      ".", ".|/home/jonny/*.c,*.h,-*tmp*", true);
  parser.add("--files", "-f", DT_PATTERN,
      "Only files matching that patterns will be found", "*",
      ";*.cpp;*.hpp;-test*");
  parser.add("--directories", "-p", DT_PATTERN,
      "Only directories matching that patterns will inspected", "*",
      ";-.git;-*tmp*;-*temp*");
  parser.add("--min-depth", "-d", DT_NAT,
      "The minimum path depth (0 is the depth of the start directory)", "0",
      "0|3");
  parser.add("--max-depth", "-D", DT_NAT,
      "The maximum path depth (0 is the depth of the start directory)", "99",
      "1|99");
  parser.add("--minutes", "-m", DT_INT,
      "The found files must be older (if < 0) or newer (if > 0) than that amount of minutes",
      "0");
  parser.add("--days", "-y", DT_INT,
      "The found files must be older (if < 0) or newer (if > 0) than that amount of days",
      "0", "10|-60|+30");
  parser.add("--size", "-s", DT_SIZE_INT,
      "The found files must have a size lower  (if < 0) or larger (if > 0) than that size. Units: [kmgt]",
      nullptr, nullptr, true);
  parser.add("--type", "-t", DT_STRING,
      "The file type: f(ile) d(irectory) l(ink) s(ocket) b(lock) p(ipe) c(har)",
      "", "f,d,l|d");
}

/**
 * Takes the filter settings from the program arguments.
 * @param parser Contains the program arguments.
 * @param filter: OUT: The filter to populate.
 */
void populateFilter(const ArgumentParser &parser, DirEntryFilter &filter) {
  auto types = splitCString(parser.asString("types", ""), ",");
  if (types[0].empty()) {
    filter._types = FsEntry::TC_ALL;
  } else {
    int typeMap = 0;
    for (auto type : types) {
      if (startsWith("file", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_REGULAR;
      } else if (startsWith("directory", 10, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_SUBDIR;
      } else if (startsWith("link", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_LINK;
      } else if (startsWith("LinkDirectory", 14, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_LINK_DIR;
      } else if (startsWith("block", 5, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_BLOCK;
      } else if (startsWith("pipe", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_PIPE;
      } else if (startsWith("char", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_CHAR;
      } else if (startsWith("text", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TC_TEXT;
      } else if (startsWith("other", 4, type.c_str(), type.size())) {
        typeMap |= FsEntry::TF_OTHER;
      } else {
        throw ArgumentException(formatCString("unknown file type: %s "
            "correct: file|link|LinkDir|block|pipe|char|other", type.c_str()));
      }
      filter._types = static_cast<FsEntry::Type_t>(typeMap);
    }
  }
  /*
   bool _allDirectories;
   *
   */
  filter._minDepth = parser.asInt("min-depth", 0);
  filter._maxDepth = parser.asInt("max-depth", 99);
  for (size_t ix = 0; ix < parser.countValuesOf("size"); ix++) {
    auto value = parser.asSize("size", 0, ix);
    auto value2 = parser.asString("size", "0", ix);
    if (value2[0] == '-') {
      filter._maxSize = -value;
    } else if (value2[0] == '+') {
      filter._minSize = value;
    } else {
      filter._minSize = filter._maxSize = value;
    }
  }
  ;
  int value = parser.asInt("minutes", 0) * 60;
  if (value == 0) {
    value = parser.asInt("days", 0) * 60 * 24;
  }
  if (value != 0) {
    time_t now = time(nullptr);
    if (value < 0) {
      setFiletime(filter._minAge, now + value);
    } else {
      setFiletime(filter._maxAge, now - value);
    }
  }
  auto stringValue = parser.asString("files", nullptr);
  if (stringValue != nullptr && strcmp(stringValue, "*") != 0) {
    filter._nodePatterns = new PatternList();
    filter._nodePatterns->set(stringValue, -1, true);
  }
  stringValue = parser.asString("directories", nullptr);
  if (stringValue != nullptr && strcmp(stringValue, "*") != 0) {
    filter._pathPatterns = new PatternList();
    filter._pathPatterns->set(stringValue, -1, true, ",");
  }
}
CommandHandler::CommandHandler(ArgumentParser &argumentParser, Logger *logger) :
    _argumentParser(argumentParser), _logger(logger), _filter(), _level(0), _status(
        nullptr), _traverser(nullptr, &_filter, nullptr, _logger) {
}
CommandHandler::~CommandHandler() {
}
bool CommandHandler::check() {
  return true;
}
void CommandHandler::finish() {
  // do nothing
}
void CommandHandler::initialize() {
  populateFilter(_argumentParser, _filter);
}
int CommandHandler::run(const char *nameSources) {
  int rc = 0;
  initialize();
  if (!check()) {
    rc = 2;
  } else {
    traverse(nameSources);
    finish();
  }
  return rc;
}
void CommandHandler::traverse(const char *nameSources) {
  auto countPatterns = _argumentParser.countValuesOf(nameSources);
  bool baseIsPattern = false;
  for (size_t ix = 0; ix < countPatterns; ix++) {
    if (baseIsPattern) {
      delete _filter._nodePatterns;
      _filter._nodePatterns = nullptr;
    }
    auto base = _argumentParser.asString(nameSources, ".", ix);
    bool exists = false;
    if (!isDirectory(base, &exists) && exists) {
      FsEntryLinux status;
      _status = &status;
      _status->setCertainFile(base);
      bool stop = !oneFile();
      _status->finishCertainFile();
      if (stop) {
        break;
      }
    } else {
      if (strchr(base, ',') == nullptr) {
        _traverser.changeBase(base);
      } else {
        _traverser.changeBaseByPatterns(base, _filter);
        baseIsPattern = true;
      }
      while ((_status = _traverser.nextFile(_level)) != nullptr) {
        std::string format;
        if (!oneFile()) {
          break;
        }
      }
    }
  }
}
}
