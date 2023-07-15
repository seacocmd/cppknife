/*
 * Function.cpp
 *
 *  Created on: 17.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"
#include "../os/os.hpp"

namespace cppknife {

std::vector<std::string> FunctionEngine::_methods = { "buffer.difference",
    "buffer.join", "buffer.pop", "buffer.shift", "buffer.sort", "buffer.split",
    "math.random", "os.basename", "os.cd", "os.changeextension", "os.copy",
    "os.dirname", "os.exists", "os.isdir", "os.listfiles", "os.mkdir",
    "os.popd", "os.pushd", "os.pwd", "os.tempname", "string.index",
    "string.length", "string.piece", "string.replace", "string.search",
    "string.substring" };
FunctionEngine::FunctionEngine(Script *script, SearchEngine *searchEngine,
    SearchParser &parser, Logger &logger) :
    _script(script), _engine(searchEngine), _parser(parser), _logger(logger), _directories() {
  parser.setInput("string.substring");
  parser.parseSpecial(&SearchParser::_regexFunction);
  auto index = 1 + parser.tokenAsIndex(_methods);
  assert(index == M_S_SUBSTRING);
}

FunctionEngine::~FunctionEngine() {
}

double FunctionEngine::asNumeric(bool testOnly, const std::string &variable) {

  Methods index = static_cast<Methods>(1 + _parser.tokenAsIndex(_methods));
  double rc = 0;
  switch (index) {
  case M_B_JOIN:
  case M_B_POP:
  case M_B_SHIFT:
  case M_O_BASENAME:
  case M_O_CD:
  case M_O_CHANGEEXTENSION:
  case M_O_DIRNAME:
  case M_O_PWD:
  case M_O_TEMPNAME:
  case M_S_REPLACE:
  case M_S_SEARCH:
  case M_S_SUBSTRING:
  case M_S_PIECE:
    throw ParserError(
        formatCString("%s is not a numeric function.",
            _parser.tokenAsCString()), _parser);
    break;
  case M_B_DIFFERENCE:
    rc = bufferDifference(testOnly);
    break;
  case M_B_SORT:
    rc = bufferSort(testOnly);
    break;
  case M_B_SPLIT:
    rc = bufferSplit(testOnly);
    break;
  case M_M_RANDOM:
    rc = mathRandom(testOnly);
    break;
  case M_O_EXISTS:
    rc = osExists(testOnly) ? 1 : 0;
    break;
  case M_O_ISDIR:
    rc = osExists(testOnly) ? 1 : 0;
    break;
  case M_O_LISTFILES:
    rc = osListfiles(testOnly);
    break;
  case M_O_COPY:
    rc = osCopy(testOnly) ? 1 : 0;
    break;
  case M_O_MKDIR:
    rc = osMkdir(testOnly) ? 1 : 0;
    break;
  case M_O_POPD:
    rc = osPopd(testOnly) ? 1 : 0;
    break;
  case M_O_PUSHD:
    rc = osPushd(testOnly) ? 1 : 0;
    break;
  case M_S_INDEX:
    rc = stringIndex(testOnly);
    break;
  case M_S_LENGTH:
    rc = stringLength(testOnly);
    break;
  default:
    throw InternalError("unknown method: ", _parser.tokenAsCString());
    break;
  }
  return rc;
}
std::string FunctionEngine::asString(bool testOnly,
    const std::string &variable) {
  Methods index = static_cast<Methods>(1 + _parser.tokenAsIndex(_methods));
  std::string rc;
  switch (index) {
  case M_B_JOIN:
    rc = bufferJoin(testOnly);
    break;
  case M_B_POP:
    rc = bufferPop(testOnly);
    break;
  case M_B_SHIFT:
    rc = bufferShift(testOnly);
    break;
  case M_O_BASENAME:
    rc = osBasename(testOnly);
    break;
  case M_O_CD:
    rc = osCd(testOnly);
    break;
  case M_O_CHANGEEXTENSION:
    rc = osChangeextension(testOnly);
    break;
  case M_O_DIRNAME:
    rc = osDirname(testOnly);
    break;
  case M_O_PWD:
    rc = osPwd(testOnly);
    break;
  case M_O_TEMPNAME:
    rc = osTempname(testOnly);
    break;
  case M_S_PIECE:
    rc = stringPiece(testOnly);
    break;
  case M_S_REPLACE:
    rc = stringReplace(testOnly);
    break;
  case M_S_SEARCH:
    rc = stringSearch(testOnly);
    break;
  case M_S_SUBSTRING:
    rc = stringSubstring(testOnly);
    break;
  case M_B_SORT:
  case M_B_SPLIT:
  case M_M_RANDOM:
  case M_O_COPY:
  case M_O_EXISTS:
  case M_O_ISDIR:
  case M_O_LISTFILES:
  case M_O_MKDIR:
  case M_O_POPD:
  case M_O_PUSHD:
  case M_S_INDEX:
  case M_S_LENGTH: {
    int rc2 = asNumeric(testOnly, variable);
    rc = formatCString("%d", rc2);
    break;
  }
  default:
    throw InternalError("unknown method: ", _parser.tokenAsCString());
    break;
  }
  return rc;
}

int FunctionEngine::bufferDifference(bool testOnly) {
  // buffer.pop <buffer>
  int rc = 0;
  LineBuffer *buffer1 = _parser.parseBuffer(testOnly, _engine, true, true);
  LineBuffer *buffer2 = _parser.parseBuffer(testOnly, _engine, true, true);

  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    size_t lineNo = 0;
    for (auto line : buffer1->constLines()) {
      lineNo++;
      if (lineNo > buffer2->constLines().size()) {
        rc = lineNo;
        break;
      }
      if (line != buffer2->constLines()[lineNo - 1]) {
        rc = lineNo;
        break;
      }
    }
    if (lineNo < buffer2->constLines().size()) {
      rc = lineNo + 1;
    }
  }
  return rc;
}

std::string FunctionEngine::bufferJoin(bool testOnly) {
  // buffer.pop <buffer>
  std::string rc;
  LineBuffer *buffer = _parser.parseBuffer(testOnly, _engine, true, true);
  auto separator = _parser.parseString(testOnly, "separator", "");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = joinVector(buffer->constLines(), separator.c_str());
  }
  return rc;
}

std::string FunctionEngine::bufferPop(bool testOnly) {
  // buffer.pop <buffer>
  std::string rc;
  LineBuffer *buffer = _parser.parseBuffer(testOnly, _engine, true, true);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    size_t size;
    while ((size = buffer->lines().size()) > 0) {
      rc = buffer->lines()[size - 1];
      buffer->lines().erase(buffer->lines().end() - 1, buffer->lines().end());
      if (!rc.empty()) {
        break;
      }
    }
  }
  return rc;
}
std::string FunctionEngine::bufferShift(bool testOnly) {
  // buffer.shift <buffer>
  std::string rc;
  LineBuffer *buffer = _parser.parseBuffer(testOnly, _engine, true, true);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    size_t size;
    while ((size = buffer->lines().size()) > 0) {
      rc = buffer->lines()[0];
      buffer->lines().erase(buffer->lines().begin(),
          buffer->lines().begin() + 1);
      if (!rc.empty()) {
        break;
      }
    }
  }
  return rc;
}
int FunctionEngine::bufferSort(bool testOnly) {
  // buffer.sort <buffer>
  LineBuffer *buffer = _parser.parseBuffer(testOnly, _engine, true, true);
  int rc = -1;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    std::sort(buffer->lines().begin(), buffer->lines().end());
    rc = buffer->lines().size();
  }
  return rc;
}
int FunctionEngine::bufferSplit(bool testOnly) {
  // buffer.split <buffer> <string> [separator]
  int rc = 0;
  LineBuffer *buffer = _parser.parseBuffer(testOnly, _engine, true, false);
  auto text(_parser.parseString(testOnly, "text"));
  auto separator(_parser.parseString(testOnly, "separator", " "));
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    auto lines = splitCString(text.c_str(), separator.c_str());
    buffer->setLines(lines);
    rc = lines.size();
  }
  return rc;
}
int FunctionEngine::mathRandom(bool testOnly) {
  // math.random <maximum> [<minimum>]
  int rc = 0;
  int maximum = _parser.parseInt(testOnly, "maximum", true);
  int minimum = _parser.parseInt(testOnly, "minimum", false, 0);
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (maximum < minimum) {
      int tmp = maximum;
      maximum = minimum;
      minimum = tmp;
    }
    srand(time(nullptr));
    rc = minimum + rand() % (maximum - minimum + 1);
  }
  return rc;
}
std::string FunctionEngine::osBasename(bool testOnly) {
  // os.basename <path>
  auto filename = _parser.parseString(testOnly, "filename");
  std::string rc;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = (basename(filename.c_str()));
  }
  return rc;
}
std::string FunctionEngine::osCd(bool testOnly) {
  // os.cd <path>
  std::string rc;
  auto path = _parser.parseString(testOnly, "path");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = currentDirectory();
    if (::chdir(path.c_str()) != 0) {
      rc.clear();
    }
  }
  return rc;
}
std::string FunctionEngine::osChangeextension(bool testOnly) {
  // os.changeextension <path> <extension>
  std::string rc;
  auto path = _parser.parseString(testOnly, "path");
  auto extension = _parser.parseString(testOnly, "extension");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = replaceExtension(path.c_str(), extension.c_str());
  }
  return rc;
}
int FunctionEngine::osCopy(bool testOnly) {
  // os.copy <source> <target> [unique]
  int rc = 0;
  auto source = _parser.parseString(testOnly, "source");
  auto target = _parser.parseString(testOnly, "target");
  bool unique = _parser.hasWaitingWord("unique") > 0;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (isDirectory(target.c_str())) {
      target = joinPath(target.c_str(), basename(source.c_str()).c_str());
    }
    std::string error;
    if (unique) {
      target = uniqueFilename(target.c_str());
    }
    if (!copyFile(source.c_str(), target.c_str(), &error)) {
      throw ParserError(error, _parser);
    }
  }
  return rc;
}
std::string FunctionEngine::osDirname(bool testOnly) {
  auto filename = _parser.parseString(testOnly, "filename");
  std::string rc;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = (dirname(filename.c_str()));
  }
  return rc;
}
bool FunctionEngine::osExists(bool testOnly) {
  // os.exists <filename>
  auto filename = _parser.parseString(testOnly, "filename");
  bool rc = false;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = fileExists(filename.c_str());
  }
  return rc;
}
bool FunctionEngine::osIsdir(bool testOnly) {
  // os.exists <filename>
  auto filename = _parser.parseString(testOnly, "filename");
  bool rc = false;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = isDirectory(filename.c_str());
  }
  return rc;
}
int FunctionEngine::osListfiles(bool testOnly) {
  // os.listfiles <buffer> <directory> [include <search_expression>] [exclude <search_expression> [files] [dirs] [links]
  int rc = 0;
  auto buffer = _parser.parseBuffer(testOnly, _engine, true, false);
  auto directory = _parser.parseString(testOnly, "directory");
  bool selectFiles = false;
  bool selectDirs = false;
  bool selectLinks = false;
  bool all = true;
  SearchExpression include;
  SearchExpression exclude;
  while (_parser.parse() == TT_IDENTIFIER) {
    if (_parser.isWord("including")) {
      _parser.parsePattern(testOnly, "including", true, include);
    } else if (_parser.isWord("excluding")) {
      _parser.parsePattern(testOnly, "excluding", true, exclude);
    } else if (_parser.isWord("files")) {
      selectFiles = true;
      all = false;
    } else if (_parser.isWord("dirs")) {
      selectDirs = true;
      all = false;
    } else if (_parser.isWord("links")) {
      selectLinks = true;
      all = false;
    }
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    auto lines = listFiles(directory.c_str(), nullptr, false, false);
    std::vector<std::string> lines2;
    for (auto node : lines) {
      if (include.isDefined() && !include.search(node.c_str())) {
        continue;
      }
      if (exclude.isDefined() && exclude.search(node.c_str())) {
        continue;
      }
      if (node == "." || node == "..") {
        continue;
      }
      if (!all) {
        auto full = joinPath(directory.c_str(), node.c_str());
        if (!selectFiles && isDirectory(full.c_str())) {
          continue;
        }
        if (!selectDirs && isDirectory(full.c_str())) {
          continue;
        }
        if (!selectLinks && isSymbolicLink(full.c_str())) {
          continue;
        }
      }
      lines2.push_back(node);
    }
    buffer->setLines(lines2);
    rc = lines2.size();
  }
  return rc;
}

bool FunctionEngine::osMkdir(bool testOnly) {
// os.mkdir <path>
  auto path = _parser.parseString(testOnly, "path");
  bool rc = false;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    ::mkdir(path.c_str(), 0755);
    rc = isDirectory(path.c_str());
  }
  return rc;
}

bool FunctionEngine::osPopd(bool testOnly) {
// // os.popd
  bool rc = false;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = _directories.size() > 0;
    if (rc) {
      auto path = _directories.back();
      _directories.pop_back();
      if (!isDirectory(path.c_str())) {
        throw ParserError(formatCString("is not a directory: %s", path.c_str()),
            _parser);
      }
      rc = ::chdir(path.c_str()) == 0;
    }
  }
  return rc;
}
bool FunctionEngine::osPushd(bool testOnly) {
// os.pushd <path>
  bool rc = false;
  auto path = _parser.parseString(testOnly, "path");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (!isDirectory(path.c_str())) {
      throw ParserError(formatCString("is not a directory: %s", path.c_str()),
          _parser);
    }
    _directories.push_back(currentDirectory());
    rc = ::chdir(path.c_str()) == 0;
  }
  return rc;
}
std::string FunctionEngine::osPwd(bool testOnly) {
  std::string rc;
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = currentDirectory();
  }
  return rc;
}
std::string FunctionEngine::osTempname(bool testOnly) {
// os.tempfile <name> [<suffix>] [in <sub-directories>]
  std::string rc = 0;

  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    auto path = currentDirectory();
  }
  return rc;
  return rc;
}
int FunctionEngine::stringIndex(bool testOnly) {
// string.index <element> <list> [<separator>]
  int rc = 0;
  auto element = _parser.parseString(testOnly, "element");
  auto list = _parser.parseString(testOnly, "list");
  auto separator = _parser.parseString(testOnly, "separator", " ");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    if (countCString(list.c_str(), list.size(), separator.c_str(),
        separator.size()) < 2) {
      throw ParserError(
          formatCString("list '%s' has too few items (separator: '%s')",
              list.c_str(), separator.c_str()), _parser);
    }
    std::vector<std::string> list2 = splitCString(list.c_str(),
        separator.c_str());
    rc = indexOf(list2, element.c_str(), element.size());
  }
  return rc;
}
int FunctionEngine::stringLength(bool testOnly) {
// string.length <string>
  int rc = 0;
  auto text = _parser.parseString(testOnly, "text");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = static_cast<int>(text.size());
  }
  return rc;
}
std::string FunctionEngine::stringPiece(bool testOnly) {
// string.piece <index> <list> <separator>
  std::string rc;
  _parser.assertToken(TT_NUMBER);
  size_t index = atol(_parser.tokenAsCString());
  auto list = _parser.parseString(testOnly, "list");
  auto separator = _parser.parseString(testOnly, "separator", " ");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    auto list2 = splitCString(list.c_str(), separator.c_str());
    if (index >= 0 and index < list2.size()) {
      rc = list2[index];
    }
  }
  return rc;
}
std::string FunctionEngine::stringReplace(bool testOnly) {
// string.replace <string> <pattern> <replacement> [count <count>]
  std::string rc;
  SearchExpression searchExpression;
  auto text = _parser.parseString(testOnly, "text");
  _parser.parsePattern(testOnly, "search", true, searchExpression);
  std::string replacement = _parser.parseString(testOnly, "replacement");
  int count = 0x7fffffff;
  if (_parser.hasWaitingWord("count") > 0) {
    count = _parser.parseInt(testOnly, "count", true);
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = text;
    LineBuffer::replaceString(rc, searchExpression, replacement.c_str(), count,
        &SearchParser::_regexBackReference);
  }
  return rc;
}
std::string FunctionEngine::stringSearch(bool testOnly) {
// string.search <string> <pattern> [<separator>]
  std::string rc;
  SearchExpression searchExpression;
  auto text = _parser.parseString(testOnly, "string");
  _parser.parsePattern(testOnly, "pattern", true, searchExpression);
  std::string separator = _parser.parseString(testOnly, "separator", " ");
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    rc = "";
    std::smatch matches;
    if (searchExpression.regExpr() == nullptr) {
      if (strstr(text.c_str(), searchExpression.pattern().c_str()) != nullptr) {
        rc = searchExpression.pattern();
      }
    } else if (std::regex_search(text, matches, *searchExpression.regExpr())) {
      rc = matches[0].str();
      for (size_t no = 1; no <= matches.size(); no++) {
        rc += separator + matches[no].str();
      }
    }
  }
  return rc;
}
std::string FunctionEngine::stringSubstring(bool testOnly) {
// string.substring <string> [ { from | behind } <start> [{ excluding | including | length } <end> ]
  std::string rc;
  int end = 0x7fffffff;
  auto text = _parser.parseString(testOnly, "text");
  int start = 1;
  int startMode = _parser.hasWaitingWord("from", "behind");
  if (startMode > 0) {
    start = _parser.parseInt(testOnly, "start", true);
    if (startMode == 2) {
      start++;
    }
  }
  int endMode = _parser.hasWaitingWord("excluding", "including", "count");
  if (endMode > 0) {
    end = _parser.parseInt(testOnly, endMode != 3 ? "end" : "count", true);
    switch (endMode) {
    case 1:
      break;
    case 2:
      end++;
      break;
    case 3:
      end = start + end;
      break;
    }
  }
  if (testOnly) {
    _parser.assertToken(TT_EOF);
  } else {
    end = min(text.size() + 1, end);
    if (start >= 0 && start < static_cast<int>(text.size())) {
      rc = text.substr(start - 1, end - start);
    }
  }
  return rc;
}

} /* namespace cppknife */
