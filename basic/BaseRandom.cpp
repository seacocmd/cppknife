/*
 * BaseRandom.cpp
 *
 *  Created on: 02.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "../core/core.hpp"

namespace cppknife {

BaseRandom *BaseRandom::_globalInstance = nullptr;

BaseRandom::BaseRandom() :
    _lastSeed(0) {
  setSeed(19911211, 19890420, 0xdeedbeef);
}

BaseRandom::~BaseRandom() {
}

BaseRandom&
BaseRandom::operator=(const BaseRandom &other) {
  return *this;
}

BaseRandom::BaseRandom(const BaseRandom &other) {
}

std::string BaseRandom::asString() const {
  return formatCString("BaseRandom(): seed: %x", _lastSeed);
}
bool BaseRandom::nextBool() {
  auto rc = nextInt(_maxInt) >= _maxInt / 2;
  return rc;
}
double BaseRandom::nextFloat() {
  auto rc = nextInt(_maxInt) / (double) _maxInt;
  return rc;
}
int BaseRandom::nextInt(int maxValue) {
  int rc = 0;
  if (maxValue > _maxInt) {
    rc = rand() % (_maxInt + 1);
  } else {
    rc = rand() % (maxValue + 1);
  }
  //printf("nextInt(%d): %d\n", maxValue, rc);
  return rc;
}

int BaseRandom::nextInt(int minValue, int maxValue) {
  int range = maxValue - minValue;
  if (range < 0) {
    throw InternalError(
        formatCString("BaseRandom::nextInt(): illegal range: %d %d %d",
            minValue, maxValue, _maxInt));
  }
  int rc = minValue + nextInt(range);
  return rc;
}

int64_t BaseRandom::nextInt50() {
  // 2**50-1 = 0x3ffffffffffff
  int64_t rc = ((static_cast<int64_t>(nextInt()) << 28) + nextInt())
      & 0x3ffffffffffffLL;
  return rc;
}
int64_t BaseRandom::nextInt63() {
  int64_t rc = ((static_cast<int64_t>((nextInt() & 0x7fffffff)) << 32)
      ^ nextInt());
  return rc;
}

int BaseRandom::hashString(const char *text, int length) {
  const size_t p = 31;
  // maxInt for 32 bit integers:
  const size_t m = 0x7fffffff;
  const size_t maxFactor = 0x10000;
  size_t hash = 0x7eadbeef;
  size_t powerValue = 1;
  char cc = 0;
  if (length < 0) {
    while ((cc = *text++) != '\0') {
      hash += cc;
      hash = (hash % maxFactor * powerValue + (hash / maxFactor) * maxFactor)
          % m;
      powerValue = powerValue * p % maxFactor;
    }
  } else {
    for (int ix = 0; ix < length; ix++) {
      cc = text[ix];
      hash += cc;
      hash = (hash % maxFactor * powerValue + (hash / maxFactor) * maxFactor)
          % m;
      powerValue = powerValue * p % maxFactor;
    }
  }
  return static_cast<int>(hash);
}

void BaseRandom::setPhrase(const char *text1, const char *text2,
    const char *text3) {
  int seed2 = text2 == nullptr ? 19891211 : hashString(text2);
  int seed3 = text3 == nullptr ? 19910420 : hashString(text3);
  setSeed(hashString(text1), seed2, seed3);
}

void BaseRandom::setSeed(seed_t seed1, seed_t seed2, seed_t seed3) {
  _lastSeed = seed1 + seed2 + seed3;
  //printf("seed: %x\n", _lastSeed);
  srand(_lastSeed);
}

void BaseRandom::testDistribution50(BaseRandom &random, size_t loops) {
  static const int PARTS = 20;
  auto divider = static_cast<int64_t>(PortableRandom::_maxNumber) / PARTS;
  int frequencies[PARTS];
  const size_t count = sizeof frequencies / sizeof frequencies[0];
  for (size_t ix = 0; ix < count; ix++) {
    frequencies[ix] = 0;
  }
  double start = nowAsDouble();
  for (size_t ix = 0; ix < loops; ix++) {
    size_t value = static_cast<size_t>(random.nextInt50() / divider);
    if (value >= 0 && value < count) {
      frequencies[value]++;
    }
  }
  double end = nowAsDouble();
  size_t min = 0x7ffff;
  size_t max = 0;
  int sum = 0;
  for (size_t ix = 0; ix < count; ix++) {
    size_t current = frequencies[ix];
    if (min > current) {
      min = current;
    }
    if (max < current) {
      max = current;
    }
    sum += current;
  }
  // ..................123456789_123456789_123456789_123456789_1234567890
  const char *stars = "**************************************************";
  for (size_t ix = 0; ix < count; ix++) {
    int len = 50 * frequencies[ix] / max;
    printf("%02ld: %5d %s\n", ix, frequencies[ix], stars + 50 - len);
  }
  printf("min: %ld max: %ld avg: %f calls/microsec: %.2f\n", min, max,
      (double) sum / count, loops / (end - start) * 1E-6);

}
void BaseRandom::testDistributionInt(BaseRandom &random, int maxInt,
    size_t loops) {
  static const int PARTS = 20;
  size_t divider = maxInt / PARTS;
  int frequencies[PARTS];
  const size_t count = sizeof frequencies / sizeof frequencies[0];
  for (size_t ix = 0; ix < count; ix++) {
    frequencies[ix] = 0;
  }
  double start = nowAsDouble();
  for (size_t ix = 0; ix < loops; ix++) {
    size_t value = static_cast<size_t>(random.nextInt(maxInt) / divider);
    if (value >= 0 && value < count) {
      frequencies[value]++;
    }
  }
  double end = nowAsDouble();
  size_t min = 0x7ffff;
  size_t max = 0;
  int sum = 0;
  for (size_t ix = 0; ix < count; ix++) {
    size_t current = frequencies[ix];
    if (min > current) {
      min = current;
    }
    if (max < current) {
      max = current;
    }
    sum += current;
  }
  // ..................123456789_123456789_123456789_123456789_1234567890
  const char *stars = "**************************************************";
  for (size_t ix = 0; ix < count; ix++) {
    int len = 50 * frequencies[ix] / max;
    printf("%02ld: %5d %s\n", ix, frequencies[ix], stars + 50 - len);
  }
  printf("maxInt: %d min: %ld max: %ld avg: %f calls/microsec: %.2f\n", maxInt,
      min, max, (double) sum / count, loops / (end - start) * 1E-6);

}

BaseRandom* BaseRandom::instance() {
  if (_globalInstance == nullptr) {
    _globalInstance = new BaseRandom();
  }
  return _globalInstance;
}
} /* cppknife */
