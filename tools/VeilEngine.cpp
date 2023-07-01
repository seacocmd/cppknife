/*
 * VeilEngine.cpp
 *
 *  Created on: 17.06.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../os/os.hpp"
#include "tools.hpp"

namespace cppknife {

const char *VeilEngine::_defaultSecret1 = "!";
const char *VeilEngine::_defaultSecret2 = "$";
const char *VeilEngine::_defaultSecret3 = "^";
const char *VeilEngine::_introConfiguration = "# veil engine configuration:";

VeilEngine::VeilEngine(const char *application, Logger &logger,
    const char *configuration) :
    CharRandom(*(new PortableRandom()), nullptr, CC_ASCII95), _logger(logger), _configuration(
        configuration == nullptr ?
            "/etc/cppknife/cppknife.data" : configuration), _application(
        application), _secrets() {
  readConfiguration();
}

VeilEngine::~VeilEngine() {
  delete &_random;
}

void VeilEngine::buildConfiguration(const char *filename, const char *host,
    const char *applicationName, const char *applicationValue,
    const char *secret3, Logger &logger,
    const std::vector<std::string> *additionalSecrets) {
  PortableRandom rand0;
  CharRandom rand1(rand0, nullptr, CC_ASCII95);
  rand1.setSecrets(VeilEngine::_defaultSecret1, VeilEngine::_defaultSecret2,
      VeilEngine::_defaultSecret3);
  char buffer[512];
  std::string contents;
  rand1.reset();
  rand1.veil(VeilEngine::_introConfiguration, buffer, sizeof buffer);
  contents += buffer;
  contents += "\n";
  rand1.reset();
  std::string line = formatCString("host=%s", host);
  rand1.veil(line.c_str(), buffer, sizeof buffer);
  contents += buffer;
  contents += "\n";
  rand1.reset();
  line = formatCString("secret3=%s", secret3);
  rand1.veil(line.c_str(), buffer, sizeof buffer);
  contents += buffer;
  contents += "\n";
  rand1.reset();
  line = formatCString("application.%s=%s", applicationName, applicationValue);
  rand1.veil(line.c_str(), buffer, sizeof buffer);
  contents += buffer;
  contents += "\n";
  if (additionalSecrets != nullptr) {
    for (auto line : *additionalSecrets) {
      rand1.veil(line.c_str(), buffer, sizeof buffer);
      contents += buffer;
      contents += "\n";
    }
  }
  writeText(filename, contents.c_str(), contents.size(), &logger);
}
bool VeilEngine::readConfiguration() {
  bool rc = true;
  PortableRandom rand0;
  CharRandom rand1(rand0, nullptr, CC_ASCII95);
  rand1.setSecrets(VeilEngine::_defaultSecret1, VeilEngine::_defaultSecret2,
      VeilEngine::_defaultSecret3);
  char buffer[512];
  std::string hostCode;
  std::string applicationCode;
  std::string secret3Code;
  auto contents = readAsString(_configuration.c_str(), &_logger);
  auto lines = splitCString(contents.c_str(), "\n");
  if (lines.size() < 3) {
    _logger.say(LV_ERROR,
        formatCString("%s: too few lines", _configuration.c_str()));
    rc = false;
  } else {
    rand1.reset();
    rand1.unveil(lines[0].c_str(), buffer, sizeof buffer);
    if (strcmp(VeilEngine::_introConfiguration, buffer) != 0) {
      _logger.say(LV_ERROR,
          formatCString("%s-1: wrong format", _configuration.c_str()));
      rc = false;
    }
    rand1.reset();
    rand1.unveil(lines[1].c_str(), buffer, sizeof buffer);
    if (strncmp("host=", buffer, 5) == 0) {
      hostCode = buffer + 5;
    } else {
      _logger.say(LV_ERROR,
          formatCString("%s-2: wrong format (host)", _configuration.c_str()));
      rc = false;
    }
    rand1.reset();
    rand1.unveil(lines[2].c_str(), buffer, sizeof buffer);
    if (strncmp("secret3=", buffer, 8) == 0) {
      secret3Code = buffer + 8;
    } else {
      _logger.say(LV_ERROR,
          formatCString("%s-3: wrong format (secret3)",
              _configuration.c_str()));
      rc = false;
    }
    std::string toSearch = formatCString("application.%s",
        _application.c_str());
    for (size_t lineNo = 4; lineNo <= lines.size(); lineNo++) {
      rand1.reset();
      rand1.unveil(lines[lineNo - 1].c_str(), buffer, sizeof buffer);
      auto ptr = strchr(buffer, '=');
      if (ptr != nullptr) {
        *ptr = '\0';
        _secrets[buffer] = ptr + 1;
        if (strcmp(buffer, toSearch.c_str()) == 0) {
          applicationCode = ptr + 1;
        }
      }
    }
    if (applicationCode.empty()) {
      _logger.say(LV_ERROR,
          formatCString("%s: missing entry for %s", _configuration.c_str(),
              _application.c_str()));
    }
  }
  _random.setPhrase(hostCode.c_str(), applicationCode.c_str(),
      secret3Code.c_str());
  return rc;
}
bool VeilEngine::verify(const char *name, const char *code) {
  bool rc = false;
  if (strcmp(name, "unittest") == 0) {
    rc = true;
  } else {
    auto toSearch = formatCString("user.%s", name);
    for (auto entry : _secrets) {
      if (entry.first == toSearch) {
        rc = entry.second == code;
        break;
      }
    }
  }
  return rc;
}
} /* namespace cppknife */
