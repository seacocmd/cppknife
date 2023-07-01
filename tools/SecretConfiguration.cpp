/*
 * SecretConfiguration.cpp
 *
 *  Created on: 16.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "tools.hpp"

namespace cppknife {

SecretConfiguration::SecretConfiguration(const char *filename,
    const char *application, Logger &logger) :
    Configuration(&logger), _engine(application, logger) {
  read(filename);
}

SecretConfiguration::~SecretConfiguration() {
}

void SecretConfiguration::read(const char *filename) {
  auto lines = readAsList(filename, _logger);
  if (lines.size() > 1) {
    std::string line1 = lines[0];
    _engine.reset();
    auto line2 = _engine.unveil(line1.c_str());
    bool encrypted = line1.size() - 4 /* saltsize */== line2.size()
        && line2[0] == '#';
    if (!encrypted) {
      veil(filename, lines);
    } else {
      for (size_t ix = 0; ix < lines.size(); ix++) {
        _engine.reset();
        lines[ix] = _engine.unveil(lines[ix].c_str());
      }
    }
    auto total = joinVector(lines, "\n");
    populate(total.c_str());
  }
}
bool SecretConfiguration::veil(const char *filename,
    const std::vector<std::string> &lines) {
  bool rc = true;
  std::string filename2(filename);
  filename2 += '~';
  if (fileExists(filename2.c_str())) {
    if (unlink(filename2.c_str()) != 0) {
      _logger->say(LV_ERROR,
          formatCString("cannot remove %s (%d)", filename2.c_str(), errno));
      rc = false;
    }
  }
  if (rc && rename(filename, filename2.c_str()) != 0) {
    _logger->say(LV_ERROR,
        formatCString("cannot rename %s to %s (%d)", filename,
            filename2.c_str(), errno));
    rc = false;
  }
  if (rc) {
    std::string text;
    size_t length = 0;
    for (auto line : lines) {
      length += 4 + 1 + line.size();
    }
    text.reserve(length + 100);
    if (!startsWith(lines[0].c_str(), lines[0].size(), "# ")) {
      _engine.reset();
      text += _engine.veil("# created by dbknife") + "\n";
    }
    for (auto line : lines) {
      _engine.reset();
      text += _engine.veil(line.c_str()) + "\n";
    }
    writeText(filename, text.c_str());
    _logger->say(LV_DETAIL, formatCString("%s was encrypted", filename));
    unlink(filename2.c_str());
  }
  return rc;
}
} /* namespace cppknife */
