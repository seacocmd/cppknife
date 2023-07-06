/*
 * SearchEngine.cpp
 *
 *  Created on: 05.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

LineRange::LineRange(SearchEngine &parent) :
    _start(0), _end(0), _parent(parent) {
}
bool LineRange::find(std::regex start, std::regex end) {
  bool rc = false;
  return rc;
}

LineBuffer::LineBuffer(Logger &logger, SearchEngine &engine) :
    LineList(100, &logger), _engine(engine) {
}
LineBuffer::~LineBuffer() {
}

SearchEngine::SearchEngine(Logger &logger) :
    LineBuffer(logger, *this), _buffers(), _scripts(), _scriptStack(), _lineNoStack(), _currentScript(
        nullptr), _trace(nullptr), _traceName(), _lastHit(), _globalVariables() {
  LineList::setName("_main");
}
SearchEngine::~SearchEngine() {
  setTrace(nullptr);
  for (auto item : _scripts) {
    delete item.second;
  }
  _scripts.clear();
  for (const auto& [key, buffer] : _buffers) {
    if (buffer->name() != "_main") {
      delete buffer;
    }
  }
  _buffers.clear();
}

void SearchEngine::addScript(Script *script) {
  _scripts[script->_name] = script;
}
LineBuffer* SearchEngine::createBuffer(const char *name) {
  LineBuffer *rc = nullptr;
  rc = currentScript("buffer", name)->createBuffer(name);
  return rc;
}
Script* SearchEngine::currentScript(const char *object, const char *name) {
  if (_currentScript == nullptr) {
    throw InternalError(
        formatCString("Cannot access %s %s (no current script)",
            object == nullptr ? "" : object, name == nullptr ? "" : name));
  }
  return _currentScript;
}
void SearchEngine::defineVariable(const char *name, const char *value) {
  if (_currentScript == nullptr) {
    throw InternalError("There is no current script. Cannot define ", name);
  }
  std::string key(name[0] == '$' ? name : formatCString("$(%s)", name).c_str());
  currentScript("variable", name)->setVariable(key, std::string(value));
}

LineBuffer* SearchEngine::getBuffer(const char *name) {
  if (name == nullptr) {
    name = "_main";
  } else if (name[0] == '!') {
    name++;
  }
  LineBuffer *rc = nullptr;
  if (name[0] != '_') {
    rc = currentScript("buffer", name)->getBuffer(name);
  } else if (strcmp(name, "_main") == 0) {
    rc = this;
  } else {
    auto it = _buffers.find(name);
    if (it != _buffers.end()) {
      rc = it->second;
    }
  }
  return rc;
}
const char* SearchEngine::globalVariable(const char *name) const {
  const char *rc = nullptr;
  auto it = _globalVariables.find(name);
  if (it != _globalVariables.end()) {
    rc = it->second.c_str();
  }
  return rc;
}

void SearchEngine::loadScript(const char *scriptName, const char *filename) {
  Script *script = new Script(scriptName, *this, _logger);
  if (_scripts.find(scriptName) != _scripts.end()) {
    throw InternalError(formatCString("script already loaded: %s", scriptName));
  }
  script->readFromFile(filename, true);
  _scripts[scriptName] = script;
}

void SearchEngine::popScript() {
  if (_scriptStack.size() == 0) {
    throw InternalError("popScript(): empty script stack");
  }
  _scriptStack.pop_back();
  auto lineNo = _lineNoStack.back();
  _lineNoStack.pop_back();
  if (_scriptStack.size() == 0) {
    _currentScript = nullptr;
  } else {
    _currentScript = _scripts[_scriptStack.back()];
    _currentScript->_indexNextStatement = lineNo;
  }
}
void SearchEngine::pushScript(Script *script, int lineNo) {
  _scriptStack.push_back(script->_name);
  _lineNoStack.push_back(lineNo);
  _scripts[script->_name] = script;
  _currentScript = script;
}

void SearchEngine::selectScript(const char *scriptName) {
  if (_scripts.find(scriptName) == _scripts.end()) {
    throw InternalError(
        formatCString("cannot select: unknown script: %s", scriptName));
  }
  int lineNo = 0;
  if (_currentScript != nullptr) {
    lineNo = _currentScript->indexNextStatement();
  }
  pushScript(_scripts[scriptName], lineNo);
}
void SearchEngine::setTrace(const char *filename, bool append) {
  if (_trace != nullptr) {
    if (_traceName != "-") {
      fclose(_trace);
    }
    _trace = nullptr;
  }
  if (filename != nullptr) {
    _traceName = filename;
    if (strcmp("-", filename) == 0) {
      _trace = stdout;
    } else {
      _trace = fopen(filename, append ? "a" : "w");
    }
  }
}
void SearchEngine::setGlobalVariable(const std::string &name,
    const std::string &value) {
  if (name[2] != '_') {
    throw InternalError(
        formatCString("not a global variable: %s", name.c_str()));
  }
  _globalVariables[name] = value;
}
Script* SearchEngine::scriptByName(const std::string &name) {
  auto it = _scripts.find(name);
  auto rc = it == _scripts.end() ? nullptr : it->second;
  return rc;
}
int SearchEngine::testAndRun(const char *scriptName) {
  int rc = 1;
  try {
    if (scriptName != nullptr) {
      selectScript(scriptName);
    }
    auto globalSafe = _globalVariables;
    _currentScript->check();
    _globalVariables = globalSafe;
    _currentScript->run();
    rc = 0;
  } catch (const ParserError &e) {
    _logger.say(LV_ERROR, e.message());
  } catch (const InternalError &e) {
    _logger.say(LV_ERROR, e.message());
  } catch (const ConfigurationException &e) {
    _logger.say(LV_ERROR, e.message());
  } catch (const std::regex_error &e) {
    _logger.say(LV_ERROR, e.what());
  } catch (const AbortException &e) {
    if (e._exitCode < 0) {
      _logger.say(LV_ERROR, "aborted");
    } else {
      rc = e._exitCode;
    }
  } catch (...) {
    _logger.say(LV_ERROR, "unknown exception");
  }
  return rc;
}
bool SearchEngine::variableExists(const char *name) {
  std::string name2(
      name[0] == '$' ? name : formatCString("$(%s)", name).c_str());
  bool rc = _globalVariables.find(name2) != _globalVariables.end();
  return rc;
}

} /* namespace cppknife */
