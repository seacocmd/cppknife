/*
 * functions.hpp
 *
 *  Created on: 17.05.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_FUNCTIONENGINE_HPP_
#define TEXT_FUNCTIONENGINE_HPP_

namespace cppknife {

/**
 * @brief Parses the functions of the <em>Search Engine Script Language</em>.
 */
class FunctionEngine {
protected:
  Script *_script;
  SearchEngine *_engine;
  SearchParser &_parser;
  Logger &_logger;
  std::vector<std::string> _directories;
  static std::vector<std::string> _methods;
  enum Methods {
    M_UNDEF,
    M_B_DIFFERENCE,
    M_B_JOIN,
    M_B_POP,
    M_B_SHIFT,
    M_B_SORT,
    M_B_SPLIT,
    M_M_RANDOM,
    M_O_BASENAME,
    M_O_CD,
    M_O_CHANGEEXTENSION, // 10
    M_O_COPY,
    M_O_DIRNAME,
    M_O_EXISTS,
    M_O_ISDIR,
    M_O_LISTFILES,
    M_O_MKDIR,
    M_O_POPD,
    M_O_PUSHD,
    M_O_PWD,
    M_O_TEMPNAME, // 20
    M_S_INDEX,
    M_S_LENGTH,
    M_S_PIECE,
    M_S_REPLACE,
    M_S_SEARCH,
    M_S_SUBSTRING
  };
public:
  FunctionEngine(Script *script, SearchEngine *searchEngine,
      SearchParser &parser, Logger &logger);
  virtual ~FunctionEngine();
public:
  double asNumeric(bool testOnly, const std::string &variable);
  std::string asString(bool testOnly, const std::string &variable);
  int bufferDifference(bool testOnly);
  std::string bufferJoin(bool testOnly);
  std::string bufferPop(bool testOnly);
  std::string bufferShift(bool testOnly);
  int bufferSort(bool testOnly);
  int bufferSplit(bool testOnly);
  int mathRandom(bool testOnly);
  std::string osBasename(bool testOnly);
  std::string osCd(bool testOnly);
  std::string osChangeextension(bool testOnly);
  int osCopy(bool testOnly);
  std::string osDirname(bool testOnly);
  bool osExists(bool testOnly);
  bool osIsdir(bool testOnly);
  int osListfiles(bool testOnly);
  bool osMkdir(bool testOnly);
  bool osPopd(bool testOnly);
  bool osPushd(bool testOnly);
  std::string osPwd(bool testOnly);
  std::string osTempname(bool testOnly);
  int stringIndex(bool testOnly);
  int stringLength(bool testOnly);
  std::string stringPiece(bool testOnly);
  std::string stringReplace(bool testOnly);
  std::string stringSearch(bool testOnly);
  std::string stringSubstring(bool testOnly);
};

} /* namespace cppknife */

#endif /* TEXT_FUNCTIONENGINE_HPP_ */
