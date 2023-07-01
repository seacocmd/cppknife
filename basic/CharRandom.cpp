/*
 * CharRandom.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "basic.hpp"

namespace cppknife {
const char *CharRandom::_charClasses[] =
    {
        // CC_UNDEF, CC_DECIMAL, CC_ALPHA, CC_ALPHA_LOWERCASE, CC_ALPHA_UPPERCASE, CC_WORD, CC_WORD_FIRST, CC_ASCII95,
        "", "0123456789",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz",
        "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz_",
        " !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~" };
const size_t CharRandom::_lengthsCharClasses[] = {
// CC_UNDEF, CC_DECIMAL, CC_ALPHA, CC_ALPHA_LOWERCASE, CC_ALPHA_UPPERCASE, CC_WORD, CC_WORD_FIRST, CC_ASCII95,
    0, 10, 26 * 2, 26, 26, 26 * 2 + 10 + 1, 26 * 2 + 1, 95 };

CharRandom::CharRandom(BaseRandom &pseudoRandom, BaseRandom *realRandom,
    CharClass charClass, int saltSize, bool internalUse) :
    _random(pseudoRandom), _realRandom(nullptr), _internRealRandom(nullptr), _saltSize(
        saltSize < 0 ? 4 : saltSize), _charSet(nullptr), _charClass(CC_ASCII95), _countChars(
        0), _indexOfChar(), _charOfIndex(), _secret1("!=/&"), _secret2(nullptr), _secret3(
        nullptr), _seedSecret1(0), _seedSecret2(0), _seedSecret3(0), _fixSalt(
        false) {
  if (!internalUse) {
    if (realRandom == nullptr) {
      realRandom = _internRealRandom = new BaseRandom();
    }
    _realRandom = realRandom;
  }
  setCharSet(charClass);
  setSecrets("@<'", "~:", "!");
  initialize();
}

CharRandom::~CharRandom() {
  delete _internRealRandom;
  _internRealRandom = nullptr;
}

std::string CharRandom::asString() const {
  auto rc = formatCString("CharRandom: salt: %d cclass: %d seeds: %d %d %d",
      _saltSize, _charClass, _seedSecret1, _seedSecret2, _seedSecret3);
  return rc;
}
const char* CharRandom::charClassText(CharClass charClass) {
  return CharRandom::_charClasses[charClass];
}

void CharRandom::initialize() {
  memset(_indexOfChar, -1, sizeof _indexOfChar);
  memset(_charOfIndex, 0, sizeof _charOfIndex);
  seed_t length = _countChars;
  seed_t length1 = _countChars - 1;
  // Incremented in registerChar():
  _countChars = 0;
  for (seed_t ix = 0; ix < length; ix++) {
    char current = _charSet[ix];
    if (ix < length1 && _charSet[ix + 1] == '-') {
      char last = _charSet[ix + 2];
      ix += 2;
      if (last < current) {
        char x = last;
        last = current;
        current = x;
      }
      while (current <= last) {
        registerChar(current++);
      }
    } else {
      registerChar(current++);
    }
  }
}

char CharRandom::nextChar(CharClass charClass) {
  int length = _lengthsCharClasses[charClass];
  int index = _random.nextInt(length - 1);
  char rc = _charClasses[charClass][index];
  return rc;
}
char CharRandom::nextCharRealRandom(CharClass charClass) {
  int length = _lengthsCharClasses[charClass];
  int index = _realRandom->nextInt(length - 1);
  char rc = _charClasses[charClass][index];
  if (rc == 0) {
    rc = _charClasses[charClass][0];
  }
  return rc;
}
char*
CharRandom::fill(size_t count, char *buffer, size_t &bufferLength,
    size_t bufferSize, CharClass charClass, bool useRealRandom) {
  for (int ix = 0; static_cast<size_t>(ix) < count; ix++) {
    if (bufferLength + 1 >= bufferSize) {
      break;
    }
    if (useRealRandom) {
      buffer[bufferLength++] = nextChar(charClass);
    } else {
      buffer[bufferLength++] = nextChar(charClass);
    }
  }
  if (bufferLength < bufferSize) {
    buffer[bufferLength] = '\0';
  }
  return buffer;
}
std::string CharRandom::randomText(size_t length, size_t lineCount,
    CharClass clazz) {
  char buffer[256];
  size_t bufferSize = sizeof buffer;
  std::string rc;
  rc.reserve(length);
  size_t lineLength = min(bufferSize - 1, (length + lineCount - 1) / lineCount);
  while (rc.size() < length) {
    size_t bufferLength = 0;
    auto currentLength = _random.nextInt(lineLength * 2 / 3,
        lineLength * 3 / 2);
    fill(currentLength, buffer, bufferLength, bufferSize - 1, clazz);
    buffer[bufferLength++] += '\n';
    auto rest = length - rc.size();
    rc.append(buffer, min(bufferLength, rest));
  }
  return rc;
}

void CharRandom::reset() {
  _random.setSeed(_seedSecret1, _seedSecret2, _seedSecret3);
}

void CharRandom::registerChar(char cc) {
  if (_indexOfChar[(int) cc] != (seed_t) -1) {
    throw InternalError(formatCString("+++ doubled defined char: %c\n", cc));
  } else {
    _indexOfChar[(int) cc] = _countChars;
    _charOfIndex[_countChars++] = cc;
  }
}

void CharRandom::setSalt(char *buffer, size_t size) {
  if (size < static_cast<size_t>(_saltSize + 1)) {
    throw InternalError(
        formatCString("CharRandom::setSalt(): buffer too small: %d", size));
  }
  for (int ix = 0; ix < _saltSize; ix++) {
    buffer[ix] = nextCharRealRandom(_charClass);
  }
  // counterpart: see unvail():
  //BaseRandom::hashString(text, _saltSize) + _seedSecret1,
  //        _seedSecret2, _seedSecret3
  _random.setSeed(BaseRandom::hashString(buffer, _saltSize) + _seedSecret1,
      _seedSecret2, _seedSecret3);
}

void CharRandom::setCharSet(CharClass charClass) {
  _charClass = charClass;
  _charSet = CharRandom::_charClasses[charClass];
  _countChars = strlen(_charSet);
  initialize();
  setSecrets(_secret1, _secret2, _secret3);
}
void CharRandom::setSecrets(const char *text1, const char *text2,
    const char *text3) {
  _secret1 = text1;
  _secret2 = text2;
  _secret3 = text3;
  _seedSecret1 = BaseRandom::hashString(_secret1);
  _seedSecret2 =
      _secret2 == nullptr ? 0xbeef1965 : BaseRandom::hashString(_secret2);
  _seedSecret3 =
      _secret3 == nullptr ? 0x19821965 : BaseRandom::hashString(_secret3);
}

char CharRandom::veil(char cc) {
  int ix = _indexOfChar[(int) cc];
  char rc;
  if (ix == (char) -1) {
    rc = cc;
  } else {
    int offset = _random.nextInt(_countChars);
    rc = _charOfIndex[(ix + offset) % _countChars];
  }
  return rc;
}
void CharRandom::veil(const char *text, char *buffer, size_t size) {
  if (size >= (size_t) _saltSize) {
    setSalt(buffer, size);
    buffer += _saltSize;
    size -= _saltSize;
  }
  const char *source = text;
  //int ix = 0;
  while (*source && size-- > 0) {
    *buffer++ = veil(*source++);
    //printf("veil[%d]: %c -> %c\n", ++ix, source[-1], buffer[-1]);
  }
  *buffer = '\0';
}
std::string CharRandom::veil(const char *text) {
  char buffer2[8192];
  size_t length = strlen(text) + _saltSize;
  char *buffer = length < sizeof buffer2 ? buffer2 : new char[length + 1];
  veil(text, buffer, length + 1);
  std::string rc(buffer);
  if (length >= sizeof buffer2) {
    delete buffer;
  }
  return rc;
}
char CharRandom::unveil(char cc) {
  seed_t ix = _indexOfChar[(int) cc];
  char rc;
  if (ix == (seed_t) -1) {
    rc = cc;
  } else {
    int offset = _random.nextInt(_countChars);
    rc = _charOfIndex[(ix - offset + _countChars) % _countChars];
  }
  return rc;
}
void CharRandom::unveil(const char *text, char *buffer, size_t size) {
  if (strlen(text) >= (size_t) _saltSize) {
    // counterpart see setSalt()
    _random.setSeed(BaseRandom::hashString(text, _saltSize) + _seedSecret1,
        _seedSecret2, _seedSecret3);
    text += _saltSize;
  }
  const char *source = text;
  //int ix = 0;
  while (*source && size-- > 0) {
    *buffer++ = unveil(*source++);
    //printf("unvail[%d]: %c -> %c\n", ++ix, source[-1], buffer[-1]);
  }
  *buffer = '\0';
}
std::string CharRandom::unveil(const char *text) {
  char buffer2[8192];
  size_t length = strlen(text) + _saltSize;
  char *buffer = length < sizeof buffer2 ? buffer2 : new char[length + 1];
  unveil(text, buffer, length + 1);
  std::string rc(buffer);
  if (length >= sizeof buffer2) {
    delete buffer;
  }
  return rc;
}
bool CharRandom::testCharClass(CharClass charClass, const char *buffer,
    int bufferLength) {
  const char *chars = _charClasses[charClass];
  if (bufferLength < 0) {
    bufferLength = strlen(buffer);
  }
  bool rc = true;
  for (int ix = 0; ix < bufferLength; ix++) {
    auto ptr = strchr(chars, buffer[ix]);
    if (ptr == nullptr) {
      rc = false;
      break;
    }
  }
  return rc;
}

} /* namespace cppknife */
