/*
 * Argument.cpp
 *
 *  Created on: 17.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../os/os.hpp"
#include "ArgumentParser.hpp"
namespace cppknife {
ArgumentException::ArgumentException(std::string message) :
    _message(message) {
}

ArgumentException::ArgumentException(const char *message, const char *arg1,
    const char *arg2) :
    _message() {
  appendString(_message, message);
  if (arg1 != nullptr) {
    appendString(_message, " ", arg1);
  }
  if (arg2 != nullptr) {
    appendString(_message, " ", arg2);
  }
}
Parameter::Parameter(const char *longname, const char *shortname,
    DataType dataType, const char *help, const char *defaultValue,
    const char *examples, bool multiple) :
    _name(), _longname(), _shortname(), _defaultValue(defaultValue), _help(), _dataType(
        dataType), _examples(buildExample(dataType, examples)), _multiple(
        multiple), _format(), _formatDescription() {
  appendString(_longname, longname);
  appendString(_shortname, shortname);
  appendString(_help, help);
  if (longname != nullptr) {
    _name = strncmp(longname, "--", 2) == 0 ? _longname.substr(2) : _longname;
  } else if (shortname != nullptr) {
    _name = strncmp(shortname, "-", 1) == 0 ? _shortname.substr(1) : _shortname;
  } else {
    throw ArgumentException(
        std::string("cannot find name: ")
            + std::string(help == nullptr ? "" : _help));
  }

}

Parameter::~Parameter() {
}
Parameter::Parameter(const Parameter &other) :
    _name(other._name), _longname(other._longname), _shortname(
        other._shortname), _defaultValue(other._defaultValue), _help(
        other._help), _dataType(other._dataType), _multiple(other._multiple), _format(
        other._format), _formatDescription(other._formatDescription) {
}

Parameter&
Parameter::operator=(const Parameter &other) {
  _name = other._name;
  _shortname = other._shortname;
  _longname = other._longname;
  _help = other._help;
  _defaultValue = other._defaultValue;
  _multiple = other._multiple;
  _format = other._format;
  _formatDescription = other._formatDescription;
  return *this;
}
const char*
Parameter::buildExample(DataType dataType, const char *examples) {
  const char *rc = examples;
  if (rc == nullptr) {
    switch (dataType) {
    case DT_UNDEF:
      break;
    case DT_NAT:
      rc = "123|0";
      break;
    case DT_INT:
      rc = "123|-1793|0";
      break;
    case DT_DOUBLE:
      rc = "1234|-0.4739|22.9E-3|-2.4E12";
      break;
    case DT_STRING:
      rc = "mydata";
      break;
    case DT_SIZE:
    case DT_SIZE_INT:
      rc = "1234|3k|93MiByte|3TIBIBYTE|22B";
      break;
    case DT_PATTERN:
      rc = "*.cpp;*.hpp";
      break;
    case DT_BOOL:
      rc = "true|T|FALSE|False|F";
      break;
    case DT_DATE:
      rc = "2022.10.30|1.02.2017|2022-10-30";
      break;
    case DT_DATETIME:
      rc = "2022.10.31 7:33:22|1999.01.20|1.02.2017 2:44";
      break;
    case DT_FILE:
      rc = "/data/myfile.txt|yourfile.pdf";
      break;
    case DT_DIRECTORY:
      rc = "/home/jonny|.|data|../mails";
      break;
    case DT_FILE_OR_DIRECTORY:
      rc = "/home/jonny|.|data|../mails|/home/jonny/you.png";
      break;
    case DT_FILE_PATTERN:
      rc = "*.cpp,*.hpp,src/test*|src1,src2|/home/joe";
      break;
    case DT_MODE:
      break;
    case DT_REGEXPR:
      rc = "/file\\d+\\.(te?xt~O!R~doc/i|!^/home/[a-z]!";
      break;
    }
  }
  if (rc == nullptr) {
    rc = "";
  }
  return rc;
}

std::string Parameter::usage(const char *indent) const {
  std::string rc;
  std::string parameterList;
  rc.reserve(1024);
  parameterList.reserve(256);
  appendString(rc, indent);
  if (!isOption()) {
    rc += toUpper(_name);
    parameterList += " ";
    parameterList += toUpper(_name);
  } else {
    if (!_shortname.empty()) {
      rc += _shortname;
      if (_dataType != DT_BOOL) {
        rc += ' ';
        rc += toUpper(_name);
      }

    }
    if (!_longname.empty()) {
      if (!_shortname.empty()) {
        rc += ',';
      }
      rc += _longname;
      if (_dataType != DT_BOOL) {
        rc += '=';
        rc += toUpper(_name);
      }
    }
  }
  appendString(rc, "\n");
  appendString(rc, indent, indent);
  rc += _help;
  if (!_examples.empty()) {
    appendString(rc, ", e.g.");
    auto parts = splitCString(_examples.c_str(), "|");
    int mode = 0;
    for (auto part : parts) {
      replaceString(part, "~O!R~", "|");
      if (isOption()) {
        if (mode++ % 2 == 0 || _shortname.empty()) {
          appendString(rc, " ", _longname.c_str());
          if (_dataType != DT_BOOL) {
            appendString(rc, "=", part.c_str());
          }
        } else {
          appendString(rc, " ", _shortname.c_str());
          if (_dataType != DT_BOOL) {
            rc += part;
          }
          break;
        }
      } else {
        appendString(rc, " ", part.c_str());
      }
    }
  }
  return rc;
}

Argument::Argument() :
    _name(), _parameter(nullptr), _values() {
}

Argument::Argument(const Parameter *parameter, std::string &value) :
    _name(parameter->name()), _parameter(parameter), _values() {
  _values.push_back(value);
}
int Argument::asInt(size_t index) const {
  checkIndex(index);
  int rc = 0;
  if (_parameter->dataType() != DT_INT && _parameter->dataType() != DT_NAT) {
    throw InternalError(
        formatCString("%s: parameter is not an int",
            _parameter->name().c_str()));
  }
  isInt(_values[index].c_str(), _values[index].size(), &rc);
  return rc;
}

const std::string&
Argument::asString(size_t index) const {
  checkIndex(index);
  return _values[index];
}

bool Argument::asBool() {
  bool rc = false;
  if (_parameter->dataType() != DT_BOOL) {
    throw InternalError(
        formatCString("%s: parameter is not a boolean",
            _parameter->name().c_str()));
  }
  isBool(_values[0].c_str(), _values[0].size(), &rc);
  return rc;
}

double Argument::asFloat(size_t index) const {
  checkIndex(index);
  double rc = 0;
  if (_parameter->dataType() != DT_DOUBLE) {
    throw InternalError(
        formatCString("%s: parameter is not a floating point",
            _parameter->name().c_str()));
  }
  isFloat(_values[index].c_str(), _values[index].size(), &rc);
  return rc;
}

int Argument::asNat(size_t index) const {
  checkIndex(index);
  if (_parameter->dataType() != DT_NAT) {
    throw InternalError(
        formatCString("%s: parameter is not a nat",
            _parameter->name().c_str()));
  }
  size_t rc2 = 0;
  isNat(_values[index].c_str(), _values[index].size(), &rc2);
  return rc2;
}

std::regex Argument::asRegExpr(size_t index) const {
  DataType type = _parameter->dataType();
  if (type != DT_SIZE && type != DT_REGEXPR) {
    throw InternalError(
        formatCString("%s: parameter is not a regular expression",
            _parameter->name().c_str()));
  }
  const char *value = _values[index].c_str();
  char delimiter = value[0];
  std::string pattern;
  std::string flags;
  if (delimiter == '\0') {
    pattern = ".*";
  } else {
    const char *ptr = strchr(value + 1, delimiter);
    if (ptr == nullptr) {
      throw InternalError("Argument::asRegExpr(): wrong index", value);
    }
    auto patternLength = (ptr - value) - 1;
    pattern = std::string(value + 1, patternLength);
    flags = ptr + 1;
  }
  if (strchr(flags.c_str(), 'w') != nullptr) {
    pattern = "\\b" + pattern + "\\b";
    if (strchr(pattern.c_str(), '[') != nullptr) {
      replaceString(pattern, ".", "\\B");
    }
  }
  std::regex_constants::syntax_option_type flags2 =
      std::regex_constants::ECMAScript;
  if (!flags.empty()) {
    if (strchr(flags.c_str(), 'i') != nullptr) {
      flags2 |= std::regex_constants::icase;
    }
#ifdef MULTILINE_KNOWN
    if (strchr(flags.c_str(), 'm') != nullptr) {
      flags2 = std::regex::multiline
    );
  }
#endif
  }
  std::regex rc(pattern, flags2);
  return rc;
}
int64_t Argument::asSize(size_t index) const {
  checkIndex(index);
  int64_t rc = 0;
  DataType type = _parameter->dataType();
  if (type != DT_SIZE && type != DT_SIZE_INT) {
    throw InternalError(
        formatCString("%s: parameter is not a size",
            _parameter->name().c_str()));
  }
  const char *value = _values[index].c_str();
  isSize(value, -1, &rc, nullptr, type == DT_SIZE_INT);
  return rc;
}

void Argument::checkIndex(size_t index) const {
  if (index >= _values.size()) {
    throw InternalError(
        formatCString("%s: argument index is too large: %d / %d",
            _parameter->name().c_str(), index, _values.size()));
  }
}
bool Argument::validate() {
  bool rc = true;
  std::string error;
  for (size_t ix = 0; ix < countValues(); ix++) {
    const char *value = _values[ix].c_str();
    size_t valueSize = _values[ix].size();
    const char *name = _parameter->name().c_str();
    bool exists = false;
    switch (_parameter->dataType()) {
    case DT_UNDEF:
      break;
    case DT_NAT:
      if (!isNat(value, valueSize)) {
        throw ArgumentException(
            formatCString("%s: not an unsigned decimal number: %s", name,
                value));
      }
      break;
    case DT_INT:
      if (!isInt(value, valueSize, nullptr, &error)) {
        throw ArgumentException(
            formatCString("%s: not a decimal number: %s", name, value));
      }
      break;
    case DT_DOUBLE:
      if (!isFloat(value, valueSize)) {
        throw ArgumentException(
            formatCString("%s: not a floating number: %s", name, value));
      }
      break;
    case DT_STRING:
      if (!_parameter->format().empty()) {
        std::regex regExpr(_parameter->format());
        if (!std::regex_search(value, regExpr)) {
          std::string desc(_parameter->formatDescription());
          replaceString(desc, "%v~", value);
          throw ArgumentException(
              formatCString("%s: wrong value (syntax): %s", name,
                  desc.c_str()));
        }
      }
      break;
    case DT_SIZE_INT:
    case DT_SIZE:
      if (!isSize(value, valueSize, nullptr, nullptr,
          _parameter->dataType() == DT_SIZE_INT)) {
        throw ArgumentException(
            formatCString("%: not a size (decimal number and unit): %s", name,
                value));
      }
      break;
    case DT_PATTERN:
      break;
    case DT_BOOL:
      if (!isBool(value, valueSize)) {
        throw ArgumentException(
            formatCString("%s: not a floating number: %s", name, value));
      }
      break;
    case DT_DATE:
    case DT_DATETIME:
    case DT_MODE:
      break;
    case DT_FILE:
      if (isDirectory(value, &exists)) {
        throw ArgumentException(
            formatCString("%s: %s is a directory, not a file", name, value));
      } else if (!exists) {
        throw ArgumentException(
            formatCString("%s: file %s does not exist", name, value));
      }
      break;
    case DT_FILE_PATTERN: {
      auto items = splitCString(value, ",");
      bool first = true;
      for (auto item : items) {
        PathInfo info;
        splitPath(item.c_str(), info);
        if (!info._path.empty()) {
          if (!first) {
            throw ArgumentException(
                formatCString("%s: only the first item can have a directory: ",
                    name, item.c_str()));
          } else if (!isDirectory(info._path.c_str(), &exists)) {
            throw ArgumentException(
                formatCString("%s: directory %s does not exist: ", name,
                    info._path.c_str()));
          }
        }
        first = false;
      }
      break;
    }
    case DT_DIRECTORY:
      if (!isDirectory(value, &exists)) {
        if (!exists) {
          throw ArgumentException(
              formatCString("%s: directory %s not found", name, value));
        } else {
          throw ArgumentException(
              formatCString("%s: %s is a file and not a directory", name,
                  value));
        }
      }
      break;
    case DT_FILE_OR_DIRECTORY:
      isDirectory(value, &exists);
      if (!exists) {
        throw ArgumentException(formatCString("%s: %s not found", name, value));
      }
      break;
    case DT_REGEXPR:
      if (value[0] != '\0') {
        std::string msg;
        try {
          if (isalnum(value[0])) {
            throw ArgumentException(
                formatCString(
                    "%s: missing starting delimiter like '/' for reg. expr.: %s",
                    name, value));
          }
          char delimiter[2] = { value[0], 0 };
          if (countCString(value, -1, delimiter, 1) != 2) {
            throw ArgumentException(
                formatCString("%s: not 2 delimiter %c for reg. expr.: %s", name,
                    value[0], value));
          }
          auto ix = strchr(value + 1, value[0]) - value;
          std::string flags(value + ix + 1);
          if (flags.size() > 0 && strspn(flags.c_str(), "im") != flags.size()) {
            throw ArgumentException(
                formatCString(
                    "%s: unknown flag(s) %s for reg. expr.: %s Use: i(gnore case) m(ultiline)",
                    name, flags.c_str(), value));
          }
          std::string pattern(value + 1, strlen(value) - 2 - flags.size());
          msg = pattern;
          std::regex test(pattern);
        } catch (const std::regex_error &e) {
          throw ArgumentException(
              formatCString("%s: error in regular expr: %s: %s", name,
                  msg.c_str(), e.what()));
        }
      }
    }
  }
  return rc;
}

ArgumentParser::ArgumentParser(const char *name, Logger *logger,
    const char *description) :
    _name(name), _description(description), _parameters(), _subParsers(), _arguments(), _usageMessage(), _argumentStorage(
        argumentRelease, 50, 50), _parameterStorage(parameterReleaser, 50, 50), _argumentParserStorage(
        argumentParserReleaser, 10, 10), _logger(logger), _activeSubParser(
        nullptr), _rootParser(this)

{
  add("--help", "-?", DT_BOOL, "Shows the usage information.");
}

ArgumentParser::~ArgumentParser() {
}

ArgumentParser::ArgumentParser(const ArgumentParser &other) :
    _name(other._name), _description(other._description), _parameters(
        other._parameters), _subParsers(other._subParsers), _arguments(
        other._arguments), _usageMessage(), _argumentStorage(argumentRelease,
        50, 50), _parameterStorage(parameterReleaser, 50, 50), _argumentParserStorage(
        argumentParserReleaser, 10, 10), _logger(other._logger), _activeSubParser(
        other._activeSubParser), _rootParser(other._rootParser) {
}

ArgumentParser&
ArgumentParser::operator =(const ArgumentParser &other) {
  _name = other._name;
  _description = other._description;
  _parameters = other._parameters;
  _subParsers = other._subParsers;
  _arguments = other._arguments;
  _logger = other._logger;
  _activeSubParser = other._activeSubParser;
  _rootParser = other._rootParser;
  return *this;
}

void ArgumentParser::add(const char *longname, const char *shortname,
    DataType dataType, const char *help, const char *defaultValue,
    const char *examples, bool multiple) {
  auto object = new Parameter(longname, shortname, dataType, help, defaultValue,
      examples, multiple);
  _parameterStorage.add(object);
  _parameters.push_back(object);
}

void ArgumentParser::addArgument(const Parameter *parameter,
    std::string value) {
  auto name = parameter->name();
  if (_arguments.find(name) != _arguments.end()) {
    throw ArgumentException("argument already defined:", name.c_str());
  }
  auto object = new Argument(parameter, value);
  _argumentStorage.add(object);
  _arguments[name] = object;
}

void ArgumentParser::addDefaultArguments() {
  for (auto parameter : _parameters) {
    const char *defaultValue = parameter->defaultValue();
    if (defaultValue != nullptr
        && findArgument(parameter->name().c_str()) == nullptr) {
      addArgument(parameter, defaultValue);
    }
  }
  if (_activeSubParser != nullptr) {
    _activeSubParser->addDefaultArguments();
  }
}

void ArgumentParser::addMode(const char *name, const char *help,
    const char *values) {
  auto object = new Parameter(name, nullptr, DT_MODE, help, values);
  _parameterStorage.add(object);
  _parameters.push_back(object);
}

void ArgumentParser::addSubParser(const char *mode, const char *value,
    ArgumentParser &subParser) {
  std::string key;
  appendString(key, mode, ".");
  appendString(key, value);

  if (_subParsers.find(key) != _subParsers.end()) {
    throw ArgumentException("sub parser already defined:", key.c_str());
  }
  subParser._rootParser = _rootParser;
  _subParsers[key] = &subParser;
}

int ArgumentParser::asBool(const char *name, int defaultValue) const {
  int rc = defaultValue;
  auto argument = findArgument(name);
  if (argument != nullptr) {
    rc = argument->asBool();
  }
  return rc;
}

double ArgumentParser::asDouble(const char *name, double defaultValue,
    size_t index) const {
  double rc = defaultValue;
  Argument *arg = findArgument(name);
  if (arg != nullptr) {
    rc = arg->asFloat(index);
  }
  return rc;
}

int ArgumentParser::asInt(const char *name, int defaultValue,
    size_t index) const {
  Argument *arg = findArgument(name);
  int rc = defaultValue;
  if (arg != nullptr) {
    rc = arg->asInt(index);
  }
  return rc;
}

std::regex ArgumentParser::asRegExpr(const char *name, const char *defaultValue,
    size_t index) const {
  Argument *arg = findArgument(name);
  std::regex rc;
  if (arg != nullptr) {
    rc = arg->asRegExpr(index);
  } else {
    rc = std::regex(defaultValue == nullptr ? ".*" : defaultValue);
  }
  return rc;
}

int64_t ArgumentParser::asSize(const char *name, int64_t defaultValue,
    size_t index) const {
  Argument *arg = findArgument(name);
  int rc = defaultValue;
  if (arg != nullptr) {
    rc = arg->asSize(index);
  }
  return rc;
}

const char*
ArgumentParser::asString(const char *name, const char *defaultValue,
    size_t index) const {
  const char *rc = nullptr;
  auto argument = findArgument(name);
  if (argument == nullptr) {
    rc = defaultValue;
  } else {
    rc = argument->asString(index).c_str();
  }
  return rc;
}

const Parameter*
ArgumentParser::buildOptionArgument(ArgVector &argVector, std::string &value) {
  const Parameter *parameter = nullptr;
  const char *arg0 = argVector._argv[0];
  size_t lengthArg0 = strlen(arg0);
  std::string name = arg0;
  const char *value2 = nullptr;
  bool eatArg = false;
  if (strncmp(arg0, "--", 2) == 0) {
    // longname:
    const char *ptr = strchr(arg0 + 2, '=');
    if (ptr != nullptr) {
      name.resize(ptr - arg0);
      value2 = ptr + 1;
    } else {
      eatArg = true;
    }
    parameter = findByLongname(name.c_str());
  } else {
    // shortname
    if (lengthArg0 != 2) {
      name.resize(2);
      value2 = arg0 + 2;
    } else {
      eatArg = true;
    }
    parameter = findByShortname(name.c_str());
  }
  if (parameter == nullptr) {
    throw ArgumentException("unknown option: ", name.c_str());
  }
  if (parameter->dataType() == DT_BOOL) {
    eatArg = false;
    value2 = "true";
  }
  if (eatArg && argVector._argc == 1) {
    throw ArgumentException("missing option value in", name.c_str());
  }
  if (eatArg) {
    argVector._argc--;
    argVector._argv++;
    value2 = argVector._argv[0];
  }
  if (value2 == nullptr) {
    throw ArgumentException("value2 is null", name.c_str());
  }
  value.clear();
  appendString(value, value2);
  return parameter;
}

bool ArgumentParser::checkArguments() {
  for (const auto& [name, argument] : _arguments) {
    argument->validate();
  }
  return true;
}

Argument*
ArgumentParser::findArgument(const char *name) const {
  Argument *rc = nullptr;
  std::string name2(name);
  if (_activeSubParser != nullptr) {
    rc = _activeSubParser->findArgument(name);
  }
  if (rc == nullptr) {
    if (_arguments.find(name2) != _arguments.end()) {
      rc = _arguments.at(name2);
    }
  }
#ifdef NewVersion
    for (const auto& [key, value] : _arguments) {

      if (startsWith(key.c_str(), key.size(), name2.c_str(), name2.size())) {
        if (rc != nullptr) {
          throw ArgumentException(formatCString("ambigous argument: %s. Use %s or %s"))
        })
      }
    }
  }
#endif
  return rc;
}

Parameter*
ArgumentParser::findByName(const char *name) const {
  auto it = std::find_if(_parameters.begin(), _parameters.end(),
      [&name](const Parameter *p) -> bool {
        return strcmp(p->name().c_str(), name) == 0;
      });
  Parameter *rc = it == _parameters.end() ? nullptr : *it;
  return rc;
}

const Parameter*
ArgumentParser::findByShortname(const char *name) const {
  auto it = std::find_if(_parameters.begin(), _parameters.end(),
      [&name](const Parameter *p) -> bool {
        return strcmp(p->shortname(), name) == 0;
      });
  const Parameter *rc = it == _parameters.end() ? nullptr : *it;
  return rc;
}

const Parameter*
ArgumentParser::findByLongname(const char *name) const {
  auto nameParts = splitCString(name + 2, "-");
  const Parameter *rc = nullptr;
  for (auto param : _parameters) {
    auto parts = splitCString(param->longname() + 2, "-");
    if (parts.size() == nameParts.size()) {
      bool found = true;
      for (size_t ix = 0; ix < parts.size(); ix++) {
        if (!startsWith(parts[ix].c_str(), parts[ix].size(),
            nameParts[ix].c_str(), nameParts[ix].size())) {
          found = false;
          break;
        }
      }
      if (found) {
        if (rc != nullptr) {
          throw ArgumentException(
              formatCString("ambigous option: %s: use %s or %s", name,
                  rc->longname(), param->longname()));
        } else {
          rc = param;
        }
      }
    }
  }
  return rc;
}

void ArgumentParser::findOptions(std::vector<Parameter*> &nonOptions,
    std::map<std::string, Parameter*> &options) {
  for (auto parameter : _parameters) {
    if (parameter->isOption()) {
      options[parameter->name()] = parameter;
    } else {
      nonOptions.push_back(parameter);
    }
  }
}
bool ArgumentParser::isMode(const char *name, const char *value) {
  Argument *argument = findArgument(name);
  if (argument == nullptr) {
    throw ArgumentException("isMode(): unknown mode:", name);
  }
  bool rc;
  rc = strcmp(argument->asString().c_str(), value) == 0;
  return rc;
}

std::string ArgumentParser::modeDescription(const char *indent) const {
  std::string rc;
  rc.reserve(2048);
  size_t indexDot = 0;
  for (auto pair : _subParsers) {
    auto key = pair.first;
    if (indexDot == 0) {
      indexDot = key.find(".");
    }
    appendString(rc, "\n", indent);
    appendString(rc, key.c_str() + indexDot + 1, "\n");
    appendString(rc, indent, "  ");
    rc += pair.second->_description;
  }
  return rc;
}

bool ArgumentParser::parse(ArgVector &argVector) {
  bool rc = true;
  std::vector<Parameter*> nonOptions;
  std::map<std::string, Parameter*> options;
  findOptions(nonOptions, options);
  size_t ixParameter = 0;
  const Parameter *parameter = nullptr;
  std::string value;
  while (argVector._argc > 0) {
    value.clear();
    const char *arg0 = argVector._argv[0];
    if (arg0[0] == '-') {
      // Option argument
      parameter = buildOptionArgument(argVector, value);
      if (strcmp(parameter->name().c_str(), "help") == 0) {
        throw ArgumentException("<help>");
      }
      auto argument = findArgument(parameter->name().c_str());
      if (argument != nullptr && parameter->multiple()) {
        argument->addValue(value.c_str());
      } else {
        addArgument(parameter, value);
      }
    } else {
      // Non option argument:
      if (ixParameter >= nonOptions.size()) {
        throw ArgumentException("too many positional parameters:", arg0);
      }
      value = arg0;
      parameter = nonOptions[ixParameter++];
      addArgument(parameter, value);
    }
    argVector._argc--;
    argVector._argv++;
    if (parameter->dataType() == DT_MODE) {
      std::string key = parameter->name();
      key += '.';
      key += value;
#ifdef OldVersion
      auto it = _subParsers.find(key);
      if (it == _subParsers.end()) {
        throw ArgumentException("subparser not defined for", key.c_str());
      }
#endif
      ArgumentParser *subParser = nullptr;
      std::string priorValue;
      for (const auto& [key2, value] : _subParsers) {
        if (startsWith(key2.c_str(), key2.size(), key.c_str(), key.size())) {
          if (subParser != nullptr) {
            int prefixLength = parameter->name().size() + 1;
            throw ArgumentException(
                formatCString("%s: ambigous value %s (%s/%s)",
                    parameter->name().c_str(), value,
                    priorValue.c_str() + prefixLength,
                    key2.c_str() + prefixLength));
          } else {
            priorValue = key2;
            subParser = value;
          }
        }
      }
      if (subParser == nullptr) {
        throw InternalError("sub parser not found:", key.c_str());
      }
      _activeSubParser = subParser;

      rc = _activeSubParser->parse(argVector);
      break;
    }
  }
  if (rc) {
    rc = checkArguments();
    addDefaultArguments();
  }
  return rc;
}
bool ArgumentParser::parseAndCheck(ArgVector &argVector) {
  bool rc = true;
  try {
    rc = parse(argVector);
  } catch (const ArgumentException &e) {
    if (strcmp("<help>", e.message().c_str()) == 0) {
      _usageMessage =
          _rootParser == this ?
              _rootParser->usage("help requested", nullptr, true) :
              _rootParser->usage("help requested", this, false);
    } else {
      _usageMessage = usage(e.message().c_str());
    }
    printf("%s\n", _usageMessage.c_str());
    rc = false;
  } catch (const InternalError &e2) {
    if (strcmp(e2.message(), "<silent>") == 0) {
      _logger->say(LV_ERROR, e2.message());
    } else {
      fprintf(stdout, "+++ %s\n", e2.message());
    }
    rc = false;
  }
  return rc;
}
void ArgumentParser::setParameterFormat(const char *name, const char *format,
    const char *formatDescription) {
  auto parameter = findByName(name);
  if (parameter == nullptr) {
    throw InternalError("ArgumentParser::setParameterFormat(): not found",
        name);
  }
  parameter->setFormat(format, formatDescription);
}
std::string ArgumentParser::usage(const char *message,
    ArgumentParser *subParser, bool allSubParsers) const {
  std::string rc;
  std::string params;
  rc.reserve(8192);
  params.reserve(256);
  if (!_description.empty()) {
    appendString(rc, "    ", _description.c_str());
  }
  bool hasOptions = false;
  bool first = true;
  for (auto parameter : _parameters) {
    if (parameter->isOption()) {
      hasOptions = true;
    } else {
      appendString(params, " ", toUpper(parameter->name()).c_str());
    }
    if (!first || !_description.empty()) {
      appendString(rc, "\n");
    }
    first = false;
    rc += parameter->usage("  ");
    if (parameter->dataType() == DT_MODE) {
      rc += modeDescription("    ");
    }
  }
  if (!params.empty()) {
    if (hasOptions) {
      params = " [<options>] " + params;
    }
    appendString(params, "\n");
  }
  rc = _name + params + rc;
  if (allSubParsers) {
    for (const auto& [key, parser] : _subParsers) {
      rc += "\n";
      appendString(rc, _name.c_str(), " ");
      rc += parser->usage(nullptr);
    }
  } else if (subParser != nullptr) {
    rc += "\n";
    rc += subParser->usage(nullptr, subParser);
  }
  if (message != nullptr) {
    appendString(rc, "\n+++ ", message);
  }
  return rc;
}

void parameterRelease(void *object) {
  Parameter *parameter = (Parameter*) object;
  delete parameter;
}

void argumentRelease(void *object) {
  Argument *parameter = (Argument*) object;
  delete parameter;
}

void argumentParserRelease(void *object) {
  ArgumentParser *parameter = (ArgumentParser*) object;
  delete parameter;
}
void argumentReleaser(void *object) {
  delete (Argument*) object;
}
void parameterReleaser(void *object) {
  delete (Parameter*) object;

}
void argumentParserReleaser(void *object) {
  delete (ArgumentParser*) object;

}

} /* namespace polygeo */
