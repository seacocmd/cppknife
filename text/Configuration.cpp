/*
 * Configuration.cpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

ConfigurationException::ConfigurationException(const char *message) :
    _message(message) {
}

ConfigurationException::ConfigurationException(const std::string &message) :
    _message(message) {
}

Configuration::Configuration(Logger *logger) :
    _map(), _logger(logger), _internalLogger(logger == nullptr) {
  if (logger == nullptr) {
    _logger = buildMemoryLogger();
  }
}

Configuration::~Configuration() {
  if (_internalLogger) {
    delete _logger;
    _logger = nullptr;
  }
}

int Configuration::asBool(const char *name, int defaultValue) {
  const char *stringValue = asString(name, nullptr);
  int rc = defaultValue;
  if (stringValue != nullptr) {
    if (stricmp("true", stringValue) == 0 || stricmp("t", stringValue) == 0) {
      rc = true;
    } else if (stricmp("false", stringValue) == 0
        || stricmp("f", stringValue) == 0) {
      rc = false;
    } else {
      throw ConfigurationException(
          formatCString("not a bool: %s (%s)", name, stringValue));
    }
  }
  return rc;
}

int Configuration::asInt(const char *name, int defaultValue) {
  const char *stringValue = asString(name, nullptr);
  int rc = defaultValue;
  std::string error;
  if (stringValue != nullptr) {
    if (!isInt(stringValue, -1, &rc, &error)) {
      throw ConfigurationException(
          formatCString("%s: not a integer: %s (%s)", name, stringValue,
              error.c_str()));
    }
  }
  return rc;
}

size_t Configuration::asNat(const char *name, size_t defaultValue) {
  const char *stringValue = asString(name, nullptr);
  size_t rc = defaultValue;
  std::string error;
  if (stringValue != nullptr) {
    if (!isNat(stringValue, -1, &rc, &error)) {
      throw ConfigurationException(
          formatCString("%s: not a integer: %s (%s)", name, stringValue,
              error.c_str()));
    }
  }
  return rc;
}

const char* Configuration::asString(const char *name,
    const char *defaultValue) {
  std::string value;
  const char *rc = defaultValue;
  if (_map.find(name) != _map.end()) {
    rc = _map.at(name).c_str();
  }
  return rc;
}

std::vector<std::string> Configuration::names(const char *included,
    const char *excluded) const {
  std::vector<std::string> rc;
  std::regex filter(included == nullptr ? ".*" : included);
  std::regex notFilter(excluded == nullptr ? "\v" : excluded);
  rc.reserve(_map.size());
  for (auto pair : _map) {
    auto key = pair.first;
    if (std::regex_search(key, filter) && !std::regex_search(key, notFilter)) {
      rc.push_back(pair.first);
    }
  }
  return rc;
}

void Configuration::populate(const char *lines) {
  auto lines2 = splitCString(lines, "\n");
  std::regex regExpr("^([\\w.-]+)\\s*=\\s*(.*)$");
  std::smatch matches;
  for (auto line : lines2) {
    if (std::regex_search(line, matches, regExpr)) {
      _map[matches[1]] = matches[2];
    }
  }
}

SimpleConfiguration::SimpleConfiguration(const char *filename, Logger *logger) :
    Configuration(logger), _filename(filename == nullptr ? filename : "") {
  if (filename != nullptr) {
    readFromFile(filename);
  }
}

SimpleConfiguration::~SimpleConfiguration() {
}

bool SimpleConfiguration::readFromFile(const char *filename) {
  const char *fn = filename == nullptr ? _filename.c_str() : filename;
  if (fn == nullptr) {
    throw InternalError(
        "SimpleConfiguration::readFromFile(): missing filename");
  }
  FILE *fp = fopen(fn, "r");
  if (fp == nullptr) {
    throw ConfigurationException(
        formatCString("%s: cannot open (%d)", fn, errno));
  }
  char buffer[64000];
  buffer[sizeof buffer - 1] = '\0';
  size_t lineNo = 0;
  bool rc = true;
  while (fgets(buffer, sizeof buffer - 1, fp) != nullptr) {
    lineNo++;
    const char *ptr = buffer + strspn(buffer, " \t\n\r");
    // comment or empty line:
    if (*ptr == '#' || *ptr == ';' || *ptr == '\0') {
      continue;
    }
    const char *ptrValue;
    if ((ptrValue = strstr(buffer, "=")) == nullptr) {
      _logger->say(LV_WARNING,
          formatCString("%s-%d: unrecognized input: %s", fn, lineNo, buffer));
      rc = false;
    } else {
      std::string key(ptr, ptrValue - ptr);
      std::string value(ptrValue + 1);
      trimString(key);
      trimString(value);
      if (_map.find(key) != _map.end()) {
        _logger->say(LV_WARNING,
            formatCString("%s-%d: multiple definition: %s", fn, lineNo,
                key.c_str()));
        rc = false;
      }
      _map[key] = value;
    }
  }
  fclose(fp);
  _logger->say(LV_DETAIL, formatCString("= configuration read: %s", fn));
  return rc;
}

} /* namespace cppknife */
