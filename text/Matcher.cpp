/*
 * Matcher.cpp
 *
 *  Created on: 11.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

/**
 * Constructor.
 */
Matcher::Matcher(bool notPattern = false) :
    _findAll(false), _ignoreCase(true), _notPattern(notPattern) {
}

/**
 * Destructor
 */
Matcher::~Matcher() {
}

/**
 * Constructor.
 */
SimpleMatcher::SimpleMatcher() :
    Matcher(), _pattern(), _tokens() {
  _tokens.reserve(10);
}

SimpleMatcher::SimpleMatcher(const char *pattern, bool isNotPattern) :
    Matcher(isNotPattern), _pattern(), _tokens() {
  compile(pattern);
}
/**
 * Destructor.
 */
SimpleMatcher::~SimpleMatcher() {
}

bool SimpleMatcher::compile(const char *pattern) {
  bool rc = true;
  _pattern = pattern;
  if (pattern[0] == '*' && pattern[1] == '\0') {
    _findAll = true;
  } else {
    _tokens = splitCString(pattern, "*");
  }
  return rc;
}

/**
 * Writes the content to a stream.
 *
 * @param fp		target file pointer
 * @param prefix	NULL or a introduction
 */
void SimpleMatcher::dump(FILE *fp, const char *prefix) const {
  std::string tokens = joinVector(_tokens, " ");
  if (prefix != NULL) {
    fprintf(fp, "%s\n", prefix);
  }
  fprintf(fp, "pattern: %s token (%d): %s\n", _pattern.c_str(),
      static_cast<int>(_tokens.size()), tokens.c_str());
}

bool SimpleMatcher::match(const char *toTest, int toTestLength,
    Hit *hit) const {
  bool rc = _findAll;
  if (!rc) {
    do {
      size_t length0 = _tokens.at(0).size();
      // Does the anchor match?
      if (length0 > 0
          && !startsWith(toTest, toTestLength, _tokens.at(0).c_str(), length0,
              _ignoreCase)) {
        break;
      }
      // Does the tail match?
      int last = _tokens.size() - 1;
      if (last == 0) {
        rc = true;
        break;
      }
      size_t lengthLast = _tokens.at(last).size();
      if (lengthLast > 0
          && !endsWith(toTest, toTestLength, _tokens.at(last).c_str(),
              lengthLast, _ignoreCase)) {
        break;
      }
      // only anchor and tail?
      if (last == 1) {
        rc = true;
        break;
      }
      rc = searchTokens(toTest, toTestLength, 1, last - 1, hit, false);
    } while (false);
  }
  return rc;
}

bool SimpleMatcher::searchTokens(const char *toTest, int toTestLength, int from,
    int to, Hit *hit, bool greedy) const {
  bool rc = true;
  int current = 0;
  for (auto token : _tokens) {
    size_t length = token.size();
    if (length == 0)
      continue;
    current = indexOf(toTest, toTestLength, token.c_str(), length, current,
        _ignoreCase);
    int hitLength = length;
    if (current < 0) {
      rc = false;
      break;
    } else if (greedy) {
      int current2 = current;
      while ((current2 = indexOf(toTest, toTestLength, token.c_str(), length,
          current2, _ignoreCase)) >= 0) {
        hitLength = current2 + length - current;
      }
    }
    if (hit != NULL) {
      hit->setStart(current);
      hit->setEnd(current + hitLength);
    }
  }
  return rc;
}
bool SimpleMatcher::search(const char *toTest, int toTestLength, Hit *hit,
    bool greedy) const {
  bool rc = _findAll;
  if (!rc) {
    size_t last = _tokens.size() - 1;
    size_t length0 = _tokens.at(0).size();
    size_t lengthLast = _tokens.at(last).size();
    rc = searchTokens(toTest, toTestLength, length0 == 0 ? 1 : 0,
        lengthLast == 0 ? last - 1 : last, hit, greedy);
  }
  return rc;
}
/**
 * Constructor.
 */
PatternList::PatternList() :
    _patternString(), _patterns() {
}
/**
 * Destructor.
 */
PatternList::~PatternList() {
  clear();
}

void PatternList::clear() {
  for (auto item : _patterns) {
    delete item;
  }
  _patterns.clear();
}
/**
 * Writes the content to a stream.
 *
 * @param fp		target file pointer
 * @param prefix	NULL or a introduction
 */
void PatternList::dump(FILE *fp, const char *prefix) const {
  std::string buffer;
  if (prefix != NULL)
    fprintf(fp, "%s\n", prefix);
  for (size_t ix = 0; ix < _patterns.size(); ix++) {
    fprintf(fp, "%ld: ", ix);
    _patterns[ix]->dump(fp, NULL);
  }
}

bool PatternList::match(const char *toTest, int toTestLength) {
  int positives = 0;
  if (toTestLength < 0) {
    toTestLength = strlen(toTest);
  }
  bool rc = false;
  for (auto pattern : _patterns) {
    bool isNot = pattern->isNotPattern();
    bool rc2 =
        !isNot && positives > 0 ? true : pattern->match(toTest, toTestLength);
    if (isNot) {
      if (rc2) {
        rc = false;
        break;
      } else if (positives == 0) {
        rc = true;
      }
    } else {
      if (rc2) {
        ++positives;
        rc = true;
      }
    }
  }
  return rc;
}
bool PatternList::search(const char *toTest, int toTestLength, Hit *hit,
    bool greedy) {
  int positives = 0;
  if (toTestLength < 0) {
    toTestLength = strlen(toTest);
  }
  bool rc = false;
  for (auto pattern : _patterns) {
    bool isNot = pattern->isNotPattern();
    bool rc2 =
        !isNot && positives > 0 ?
            true : pattern->search(toTest, toTestLength, hit, greedy);
    if (isNot) {
      if (rc2) {
        rc = false;
        break;
      } else if (positives == 0) {
        rc = true;
      }
    } else {
      if (rc2) {
        ++positives;
        rc = true;
      }
    }
  }
  return rc;
}
void PatternList::set(const char *patterns, int patternsLength, bool ignoreCase,
    const char *separator, const char *notPrefix) {
  char buffer[2];
  clear();
  if (patternsLength < 0) {
    patternsLength = strlen(patterns);
  }
  if (separator == NULL) {
    buffer[0] = patterns[0];
    buffer[1] = '\0';
    separator = buffer;
    patterns++;
    patternsLength--;
  }
  setString(_patternString, patterns, patternsLength);
  auto patternList = splitCString(_patternString.c_str(), separator);
// Store the "positive" patterns:
  std::string theNotPattern(notPrefix == nullptr ? "" : notPrefix);
  for (auto pattern : patternList) {
    const char *toTest = pattern.c_str();
    bool isNot = notPrefix != nullptr
        && startsWith(pattern.c_str(), pattern.size(), theNotPattern.c_str(),
            theNotPattern.size());
    if (isNot) {
      toTest++;
    }
    // Will be deleted in destructor:
    auto matcher = new SimpleMatcher(toTest, isNot);
    matcher->setIgnoreCase(ignoreCase);
    _patterns.push_back(matcher);
  }
}
bool SimpleMatcher::matches(const char *pattern, const char *text,
    int patternLength, int textLength) {
  SimpleMatcher matcher(pattern);
  bool rc = matcher.match(text, textLength);
  return rc;
}

int SimpleMatcher::firstMatch(const char *pattern,
    const std::vector<std::string> &list, int start) {
  SimpleMatcher matcher(pattern);
  int rc = -1;
  for (size_t ix = start; ix < list.size(); ix++) {
    const std::string &item = list[ix];
    if (matcher.match(item.c_str(), list[ix].size())) {
      rc = ix;
      break;
    }
  }
  return rc;
}
std::vector<std::string> SimpleMatcher::filterMatches(const char *pattern,
    const std::vector<std::string> &list) {
  std::vector<std::string> rc;
  rc.reserve(list.size());
  SimpleMatcher matcher(pattern);
  for (auto text : list) {
    if (matcher.match(text.c_str(), text.size())) {
      rc.push_back(text);
    }
  }
  return rc;
}
} /* cppknife */
