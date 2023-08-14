/*
 * StringTool.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../core/core.hpp"

namespace cppknife {
const std::regex stringToolRegexWhitespaces = std::regex("[\\s]+");
const std::regex stringToolRegexIdentifier = std::regex("[_A-Za-z]\\w*");

static const char *decimalChars = "0123456789";
static const char *signChars = "+-";

bool StringComparism::operator()(char const *a, char const *b) const {
  return strcmp(a, b) < 0;
}

char*
appendCString(char *buffer, size_t &bufferLength, size_t bufferSize,
    const char *tail, int tailLength) {
  if (bufferLength < bufferSize) {
    if (tailLength < 0) {
      tailLength = strlen(tail);
    }
    size_t length =
        bufferLength + tailLength < bufferSize ?
            tailLength : bufferSize - bufferLength - 1;
    memcpy(buffer + bufferLength, tail, length);
    bufferLength += length;
    buffer[bufferLength] = '\0';
  }
  return buffer;
}
std::string& appendDouble(std::string &string, double data,
    const char *format) {
  char buffer[256];
  sprintf(buffer, format, data);
  string += buffer;
  return string;
}

std::string& appendInt(std::string &string, int data, const char *format) {
  char buffer[256];
  sprintf(buffer, format, data);
  string += buffer;
  return string;
}

std::string& appendString(std::string &string, const char *source1,
    const char *source2) {
  if (source1 != nullptr) {
    string += std::string(source1);
  }
  if (source2 != nullptr) {
    string += std::string(source2);
  }
  return string;
}
int charToNibble(char cc) {
  int rc = -1;
  if (cc >= '0' && cc <= '9') {
    rc = cc - '0';
  } else if (cc >= 'A' && cc <= 'F') {
    rc = 10 + cc - 'A';
  } else if (cc >= 'a' && cc <= 'f') {
    rc = 10 + cc - 'a';
  }
  return rc;
}
const char*
copyCString(const char *source, int sourceLength) {
  if (source == nullptr) {
    source = "";
  }
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  char *rc = new char[sourceLength + 1];
  memcpy(rc, source, sourceLength);
  rc[sourceLength] = '\0';
  return (const char*) rc;
}
const char*
copyNCString(char *target, size_t targetSize, const char *source,
    int sourceLength) {
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  if (targetSize > 0) {
    size_t length = min(sourceLength, targetSize - 1);
    memcpy(target, source, length);
    target[length] = '\0';
  }
  return target;
}
size_t countCharInCString(const char *source, char toSearch) {
  size_t rc = 0;
  source--;
  while ((source = strchr(source + 1, toSearch)) != nullptr) {
    rc++;
  }
  return rc;
}

size_t countCString(const char *source, int sourceLength, const char *subString,
    int subStringLength) {
  size_t rc = 0;
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  if (subStringLength < 0) {
    subStringLength = strlen(subString);
  }
  if (subStringLength > 0) {
    int subStringLength2 = subStringLength - 1;
    char firstChar = subString[0];
    while (sourceLength >= subStringLength) {
      while (sourceLength >= subStringLength && *source != firstChar) {
        source++;
        sourceLength--;
      }
      if (strncmp(source, subString, subStringLength) == 0) {
        rc++;
        source += subStringLength2;
        sourceLength -= subStringLength2;
      }
      source++;
      sourceLength--;
    }
  }
  return rc;
}
uint32_t crc32(uint8_t *buffer, size_t bufferLength, bool lastCall) {
  uint32_t checkSum = 0xFFFFFFFF;
  crc32Update(buffer, bufferLength, checkSum, lastCall);
  return checkSum;
}
uint32_t crc32Update(uint8_t *buffer, size_t bufferLength, uint32_t &checkSum,
    bool lastCall) {
  // use a local variable:
  uint32_t localSum = checkSum;
  for (size_t ix = 0; ix < bufferLength; ix++) {
    uint8_t byte = buffer[ix];
    localSum = localSum ^ byte;
    // Eight times:
    unsigned int mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
    mask = -(localSum & 1);
    localSum = (localSum >> 1) ^ (0xedb88320 & mask);
  }
  checkSum = lastCall ? ~localSum : localSum;
  return checkSum;
}

bool endsWith(const char *source, int sourceLength, const char *tail,
    int tailLength, bool ignoreCase) {
  bool rc = false;
  assert(source != nullptr && tail != nullptr);
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  if (tailLength < 0) {
    tailLength = strlen(tail);
  }
  if (sourceLength >= tailLength) {
    rc = true;
    int ixSource = sourceLength - tailLength;
    if (!ignoreCase) {
      rc = strncmp(source + ixSource, tail, tailLength) == 0;
    } else {
      for (int ix = 0; ix < tailLength; ix++) {
        if (tolower(source[ixSource + ix]) != tolower(tail[ix])) {
          rc = false;
          break;
        }
      }
    }
  }
  return rc;
}
std::string escapeMetaCharacters(const char *string) {
  std::string rc(string);
  escapeMetaCharacters(rc);
  return rc;
}
std::string& escapeMetaCharacters(std::string &string) {
  unsigned char cc;
  int additional = escapeMetaCharactersCount(string.c_str());
  if (additional > 0) {
    string.reserve(string.size() + additional);
    for (size_t ix = 0; ix < string.size(); ix++) {
      switch (cc = string[ix]) {
      case '"':
      case '\\':
        string.insert(ix++, "\\");
        break;
      case '\n':
        string[ix] = 'n';
        string.insert(ix++, "\\");
        break;
      case '\r':
        string[ix] = 'r';
        string.insert(ix++, "\\");
        break;
      case '\t':
        string[ix] = 't';
        string.insert(ix++, "\\");
        break;
      case '\v':
        string[ix] = 'v';
        string.insert(ix++, "\\");
        break;
      case '\f':
        string[ix] = 'f';
        string.insert(ix++, "\\");
        break;
      default:
        if (cc < ' ') {
          string.insert(ix++, "\\xA");
          int nibble1 = (cc / 16);
          int nibble2 = (cc % 16);
          string[++ix] = nibble1 + (nibble1 < 10 ? '0' : 'a');
          string[ix + 1] = nibble2 + (nibble2 < 10 ? '0' : 'a');
        }
        break;
      }
    }
  }
  return string;
}

int escapeMetaCharactersCount(const char *string) {
  int rc = 0;
  unsigned char cc;
  while ((cc = static_cast<unsigned char>(*string++)) != '\0') {
    switch (cc) {
    case '"':
    case '\\':
    case '\n':
    case '\r':
    case '\t':
    case '\v':
    case '\f':
      rc++;
      break;
    default:
      if (cc < ' ') {
        rc += 3;
      }
      break;
    }
  }
  return rc;
}

std::string formatCString(const char *fmt, ...) {
  char buffer[8192];
  assert(fmt != nullptr);
  va_list args1;
  va_start(args1, fmt);
  int count = std::vsnprintf(buffer, sizeof buffer, fmt, args1);
  va_end(args1);
  if (count < 0) {
    throw InternalError("formatString(): format error (buffer size?)", fmt);
  }
  return std::string(buffer);
}
const char*
formatOnBuffer(char *buffer, size_t bufferSize, const char *fmt, ...) {
  assert(fmt != nullptr);
  va_list args1;
  va_start(args1, fmt);
  int count = std::vsnprintf(buffer, bufferSize, fmt, args1);
  va_end(args1);
  if (count < 0) {
    throw InternalError("formatString(): format error (buffer size?)", fmt);
  }
  return buffer;
}
std::string globToRegularExpression(const char *globPattern, int length) {
  std::string rc;
  if (length < 0) {
    length = strlen(globPattern);
  }
  rc.reserve(length * 2);
  char cc = 0;
  char inClass = false;
  while (length-- > 0) {
    switch (cc = *globPattern++) {
    case '[':
      rc.push_back(cc);
      inClass = true;
      break;
    case ']':
      rc.push_back(cc);
      inClass = false;
      break;
    case '(':
    case ')':
    case '{':
    case '}':
    case '\\':
    case '.':
      if (!inClass) {
        rc.push_back('\\');
      }
      rc.push_back(cc);
      break;
    case '*':
      if (!inClass) {
        rc.push_back('.');
      }
      rc.push_back('*');
      break;
    case '?':
      if (!inClass) {
        rc.push_back('.');
      }
      break;
    default:
      rc.push_back(cc);
      break;
    }
  }
  return rc;
}
int indexOf(const char *source, int sourceLength, const char *part,
    int lengthPart, int start, bool ignoreCase) {
  int rc = -1;
  assert(source != nullptr && part != nullptr);
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  if (lengthPart < 0) {
    lengthPart = strlen(part);
  }
  if (sourceLength - start >= lengthPart) {
    if (!ignoreCase) {
      while (start <= sourceLength - lengthPart) {
        if (strncmp(source + start, part, lengthPart) == 0) {
          rc = start;
          break;
        } else {
          start++;
        }
      }
    } else {
      while (start <= sourceLength - lengthPart) {
        if (strnicmp(source + start, part, lengthPart) == 0) {
          rc = start;
          break;
        } else {
          start++;
        }
      }
    }
  }
  return rc;
}
int indexOf(const std::vector<std::string> &list, const char *element,
    int elementLength, bool isSorted) {
  int rc = -1;
  if (elementLength == -1) {
    elementLength = strlen(element);
  }
  if (isSorted) {
    std::string element2(element, elementLength);
    auto it = std::lower_bound(list.begin(), list.end(), element2);
    if (it != list.end() && *it == element2) {
      rc = std::distance(list.begin(), it);
    }
  } else {
    for (size_t ix = 0; ix < list.size(); ix++) {
      if (list[ix].size() == static_cast<size_t>(elementLength)
          && strncmp(list[ix].c_str(), element, elementLength) == 0) {
        rc = ix;
        break;
      }
    }
  }
  return rc;
}
bool isBool(const char *source, int sourceLength, bool *value,
    std::string *error) {
  bool isTrue = false;
  bool isFalse = false;
  bool rc = (isTrue = startsWith("true", 4, source, sourceLength, true))
      || (isFalse = startsWith("false", 5, source, sourceLength, true));
  if (!rc && error != nullptr) {
    *error = formatCString("not a boolean value ('true', 'false'): %s", source);
  }
  if (value != nullptr) {
    *value = isTrue;
  }
  return rc;
}

bool isSize(const char *source, int sourceLength, int64_t *value,
    std::string *error, bool mayBeNegative) {
  if (error != nullptr) {
    error->clear();
  }
  int64_t current = 0;
  double factor = 1.0;
  char buffer[128];
  bool rc = true;
  copyNCString(buffer, sizeof buffer, source, sourceLength);
  const char *ptr = buffer + strspn(buffer, "+");
  bool isNegative = false;
  if (mayBeNegative && buffer[0] == '-') {
    ptr++;
    isNegative = true;
  }
  auto count = strspn(ptr, decimalChars);
  if (count <= 0) {
    rc = false;
    if (error != nullptr) {
      *error = formatCString("missing decimal digit(s): %s", ptr);
    }
  } else {
    current = atoi(ptr);
    ptr += count;
    float factor2 = 1.0;
    bool checkSuffix = true;
    switch (tolower(*ptr)) {
    case 'b':
      ptr++;
      if (startsWith(ptr, -1, "yte", 3, true)) {
        ptr += 3;
      }
      checkSuffix = false;
      break;
    case 'k':
      factor = 1E3;
      factor2 = 1024.0;
      break;
    case 'm':
      factor = 1E6;
      factor2 = 1024.0 * 1024;
      break;
    case 'g':
      factor = 1E9;
      factor2 = 1024.0 * 1024 * 1024;
      break;
    case 't':
      factor = 1E12;
      factor2 = 1024.0 * 1024 * 1024 * 1024;
      break;
    case '\0':
      checkSuffix = false;
      break;
    default:
      rc = false;
      if (error != nullptr) {
        *error = formatCString("unknown unit ([kmgt]: %s", ptr);
      }
      break;
    }
    if (checkSuffix) {
      ptr++;
      if (startsWith(ptr, -1, "byte", 4, true)) {
        ptr += 4;
      } else if (startsWith(ptr, -1, "ibyte", 5, true)) {
        factor = factor2;
        ptr += 5;
      } else if (startsWith(ptr, -1, "ibibyte", 7, true)) {
        factor = factor2;
        ptr += 7;
      } else if (tolower(*ptr) == 'i') {
        factor = factor2;
        ptr++;
      }
    }
    if (*ptr != '\0') {
      rc = false;
      if (error != nullptr) {
        *error = formatCString("unexpected chars: %s", ptr);
      }
    }
  }
  if (rc && value != nullptr) {
    *value = static_cast<int64_t>((isNegative ? -current : current) * factor);
  }
  return rc;
}
bool isFloat(const char *source, int sourceLength, double *value,
    std::string *error) {
  if (error != nullptr) {
    error->clear();
  }
  bool rc = true;
  if (*source == '\0') {
    rc = false;
  } else {
    char buffer[128];
    copyNCString(buffer, sizeof buffer, source, sourceLength);
    char *endPtr = nullptr;
    double value2 = ::strtod(buffer, &endPtr);
    if (value != nullptr) {
      *value = value2;
    }
    if (*endPtr != '\0') {
      rc = false;
      if (error != nullptr) {
        if (endPtr == buffer) {
          *error = formatCString("not a floating value: %.20s", buffer);
        } else {
          *error = formatCString(
              "invalid data behind the floating number: %.20s", endPtr);
        }
      }
    }
  }
  return rc;
}

bool isInt(const char *source, int sourceLength, int *value,
    std::string *error) {
  if (error != nullptr) {
    error->clear();
  }
  char buffer[128];
  bool rc = true;
  copyNCString(buffer, sizeof buffer, source, sourceLength);
  const char *ptr = buffer + strspn(buffer, signChars);
  auto count = strspn(ptr, decimalChars);
  if (count <= 0) {
    rc = false;
    if (error != nullptr) {
      *error = formatCString("missing decimal digit(s): %s (%s)", ptr,
          error == nullptr ? "" : error->c_str());
    }
  } else {
    ptr += count;
    if (*ptr != '\0') {
      rc = false;
      if (error != nullptr) {
        *error = formatCString("unexpected chars in integer: %s", ptr);
      }
    }
  }
  if (rc && value != nullptr) {
    *value = atoi(source);
  }
  return rc;
}

bool isNat(const char *source, int sourceLength, size_t *value,
    std::string *error) {
  if (error != nullptr) {
    error->clear();
  }
  char buffer[128];
  bool rc = true;
  copyNCString(buffer, sizeof buffer, source, sourceLength);
  const char *ptr = buffer;
  auto count = strspn(ptr, decimalChars);
  if (count <= 0) {
    rc = false;
    if (error != nullptr) {
      *error = formatCString("missing decimal digit(s): %s", ptr);
    }
  } else {
    ptr += count;
    if (*ptr != '\0') {
      rc = false;
      if (error != nullptr) {
        *error = formatCString("unexpected chars in unsigned int: %s", ptr);
      }
    }
  }
  if (rc && value != nullptr) {
    *value = atoi(source);
  }
  return rc;
}
char*
joinCStrings(char *buffer, size_t bufferSize, const char *string1,
    const char *string2, const char *string3, const char *string4) {
  size_t length1 = string1 == nullptr ? 0 : strlen(string1);
  size_t length2 = string2 == nullptr ? 0 : strlen(string2);
  size_t length3 = string3 == nullptr ? 0 : strlen(string3);
  size_t length4 = string4 == nullptr ? 0 : strlen(string4);
  size_t sumLength = length1 + length2 + length3 + length4;
  if (sumLength >= bufferSize) {
    throw InternalError("joinStrings(): too small buffer: ", nullptr,
        (int) sumLength);
  } else {
    auto ptr = buffer;
    if (length1 > 0) {
      memcpy(ptr, string1, length1);
      ptr += length1;
    }
    if (length2 > 0) {
      memcpy(ptr, string2, length2);
      ptr += length2;
    }
    if (length3 > 0) {
      memcpy(ptr, string3, length3);
      ptr += length3;
    }
    if (length4 > 0) {
      memcpy(ptr, string4, length4);
    }
    buffer[sumLength] = '\0';
    return buffer;
  }
}
std::string joinVector(const std::vector<std::string> &array,
    const char *separator) {
  size_t length = 0;
  size_t lengthSeparator = separator == nullptr ? 0 : strlen(separator);
  std::string rc;
  for (auto item : array) {
    length += item.size() + lengthSeparator;
  }
  if (length > 0) {
    rc.reserve(length);
    for (auto item : array) {
      if (!rc.empty() && lengthSeparator > 0) {
        rc += separator;
      }
      rc += item;
    }
  }
  return rc;
}

std::string& replaceString(std::string &string, const std::string &what,
    const std::string &to, int count) {
  if (!what.empty()) {
    if (count < 0) {
      count = 0x7fffffff;
    }
    size_t start = 0;
    while (count-- > 0
        && (start = string.find(what, start)) != std::string::npos) {
      string.replace(start, what.length(), to);
      start += to.length();
    }
  }
  return string;
}
const std::string& setString(std::string &string, const char *source,
    int sourceLength) {
  if (sourceLength < 0) {
    string = source;
  } else {
    string = std::string(source, sourceLength);
  }
  return string;
}
std::vector<std::string> splitCString(const char *text, const char *delimiter,
    int maxCount) {
  if (maxCount <= 0) {
    maxCount = 0x7fffffff;
  }
  std::vector < std::string > rc;
  assert(text != nullptr && delimiter);
  if (delimiter == nullptr) {
    delimiter = "\n";
  }
  const char *ptr = text;
  const char *current = nullptr;
  int count = 0;
  size_t lengthDelimiter = strlen(delimiter);
  do {
    if ((current = strstr(ptr, delimiter)) == nullptr) {
      break;
    } else {
      count++;
      ptr = current + lengthDelimiter;
    }
  } while (count < maxCount);
  rc.reserve(count + 1);
  ptr = text;
  count = 0;
  do {
    if ((current = strstr(ptr, delimiter)) == nullptr) {
      rc.push_back(ptr);
      break;
    } else {
      rc.push_back(std::string(ptr, current - ptr));
      ptr = current + lengthDelimiter;
      count++;
    }
  } while (count < maxCount);
  if (count >= maxCount) {
    appendString(rc[maxCount - 1], current);
  }
  return rc;
}
std::vector<std::string> splitString(const std::string &text,
    const std::regex &delimiter) {
  std::sregex_token_iterator iter(text.begin(), text.end(), delimiter, -1);
  std::sregex_token_iterator end;
  int count = 0;
  for (; iter != end; ++iter) {
    ++count;
  }
  std::vector < std::string > rc;
  rc.reserve(count + 1);
  std::sregex_token_iterator iter2(text.begin(), text.end(), delimiter, -1);
  for (; iter2 != end; ++iter2) {
    rc.push_back(*iter2);
  }
  return rc;
}

bool startsWith(const char *source, int sourceLength, const char *head,
    int headLength, bool ignoreCase) {
  bool rc = false;
  assert(source != nullptr && head != nullptr);
  if (sourceLength < 0) {
    sourceLength = strlen(source);
  }
  if (headLength < 0) {
    headLength = strlen(head);
  }
  if (sourceLength >= headLength) {
    rc = true;
    if (!ignoreCase) {
      rc = strncmp(source, head, headLength) == 0;
    } else {
      for (int ix = 0; ix < headLength; ix++) {
        if (tolower(source[ix]) != tolower(head[ix])) {
          rc = false;
          break;
        }
      }
    }
  }
  return rc;
}
std::string stringInplace(const std::string &source, char*
(*function)(char*)) {
  std::string rc;
  size_t length = source.length();
  char buffer[8192];
  if (length + 1 < sizeof buffer) {
    memcpy(buffer, source.c_str(), length + 1);
    function(buffer);
    rc = buffer;
  } else {
    auto buffer2 = new char[length + 1];
    memcpy(buffer2, source.c_str(), length + 1);
    function(buffer2);
    rc = buffer2;
    delete[] buffer2;
  }
  return rc;
}
std::string stringToRegularExpression(const char *pattern, ssize_t length) {
  static const char metacharacters[] = R"(\.^$-+()[]{}|?*)";
  std::string rc;
  if (length < 0) {
    length = strlen(pattern);
  }
  rc.reserve(length * 2);
  for (auto ix = 0; ix < length; ix++) {
    char cc = pattern[ix];
    if (strchr(metacharacters, cc)) {
      rc.push_back('\\');
    }
    rc.push_back(cc);
  }
  return rc;
}
char* strlower(char *buffer) {
  assert(buffer != nullptr);
  char *ptr = buffer;
  while (*ptr != '\0') {
    *ptr = tolower(*ptr);
    ++ptr;
  }
  return buffer;
}
int strnicmp(const char *text1, const char *text2, size_t n) {
  char cc1, cc2;
  int rc = 0;
  assert(text1 != nullptr && text2 != nullptr);
  while ((cc1 = *text1++) != '\0' && (cc2 = *text2++) != '\0' && n > 0) {
    if (tolower(cc1) != tolower(cc2)) {
      rc = tolower(cc1) - tolower(cc2);
      break;
    }
    n--;
  }
  if (rc == 0 && n > 0) {
    if (cc1 == '\0') {
      rc = *text2 != '\0' ? 1 : 0;
    } else {
      rc = cc2 == '\0' ? -1 : cc2 - cc1;
    }
  }
  return rc;
}

int stricmp(const char *text1, const char *text2) {
  char cc1, cc2;
  int rc = 0;
  assert(text1 != nullptr && text2 != nullptr);
  while ((cc1 = *text1++) != '\0' && (cc2 = *text2++) != '\0') {
    if (tolower(cc1) != tolower(cc2)) {
      rc = tolower(cc1) - tolower(cc2);
      break;
    }
  }
  if (rc == 0) {
    if (cc1 == '\0') {
      rc = *text2 != '\0' ? 1 : 0;
    } else {
      rc = cc2 == '\0' ? -1 : cc2 - cc1;
    }
  }
  return rc;
}

char*
strupper(char *buffer) {
  assert(buffer != nullptr);
  char *ptr = buffer;
  while (*ptr != '\0') {
    *ptr = toupper(*ptr);
    ++ptr;
  }
  return buffer;
}
size_t sumOfLength(const char *string1, const char *string2,
    const char *string3, const char *string4) {
  size_t rc = (string1 == nullptr ? 0 : strlen(string1))
      + (string2 == nullptr ? 0 : strlen(string2))
      + (string3 == nullptr ? 0 : strlen(string3))
      + (string4 == nullptr ? 0 : strlen(string4));
  return rc;
}
size_t sumOfVectorLengths(const std::vector<std::string> &list) {
  size_t rc = 0;
  for (auto item : list) {
    rc += item.size();
  }
  return rc;
}
std::string& trimString(std::string &string, bool top, bool end,
    const char *chars) {
  if (string.size() > 0) {
    if (top) {
      size_t count = strspn(string.c_str(), chars);
      if (count > 0) {
        string.erase(0, count);
      }
    }
    if (end && string.size() > 0) {
      size_t last = string.size() - 1;
      const char *ptr = string.c_str() + last;
      while (strspn(ptr, chars) > 0) {
        ptr--;
      }
      ptr++;
      size_t count = strspn(ptr, chars);
      if (count > 0) {
        string.resize(string.size() - count);
      }
    }
  }
  return string;
}
std::string toLower(const std::string &source) {
  std::string rc = stringInplace(source, strlower);
  return rc;
}

std::string toUpper(const std::string &source) {
  std::string rc = stringInplace(source, strupper);
  return rc;
}
std::string truncateCString(const char *text, size_t length) {
  std::string rc;
  assert(text != nullptr);
  size_t currentLength = strlen(text);
  if (currentLength <= length) {
    rc = text;
  } else {
    rc = std::string(text, length);
  }
  return rc;
}
} /* namespace polygeo */

