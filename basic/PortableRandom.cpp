/*
 * PortableRandom.cpp
 *
 *  Created on: 03.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "basic.hpp"

namespace cppknife {
// Offsets lower than 2**53:
seed50_t PortableRandom::_offsets[] = { 477417747081244, 11891569955439,
    1112953394633955, 914449391256858, 574570619325411, 140389009826762,
    608000936401952, 32991187017681, 158039946481952, 700357467346732,
    968941481455215, 25223739256570, 471061646629072, 551047729047099,
    907255805781784, 628387343904130, 893283886891000, 981075091853751,
    776506900100360, 434511969151373, 177432087675717, 491951583746060,
    299997671178038, 1110099901520936, 255708987865834, 578403292103942,
    1093220808197141, 643105169062152, 189554682132125, 947667916711915,
    599096422373408, 662711516075667, 112172498828425, 588478080163912,
    720064562609749, 520347612306989, 330303796102313, 191695345476469,
    419517186899079, 752629552906654, 18312389588488, 88428897947984,
    675611758740110, 1064495336311691 };
int PortableRandom::_countNumbers = sizeof _offsets / sizeof _offsets[0];
// prime numbers lower 2**25
seed50_t PortableRandom::_factors[] = { 33554393, 23554387, 33554383, 32559983 };
int PortableRandom::_countFactors = sizeof _factors / sizeof _factors[0];
// 2**50:
seed50_t PortableRandom::_maxNumber = 1125899906842624;
// 2**27: product with 2**25 need to be lower than 2**52 (53 bits precision of double), factor < 2**
seed50_t PortableRandom::_maxFactor = 134217728;

PortableRandom *PortableRandom::_globalInstance = nullptr;

PortableRandom::PortableRandom() :
    BaseRandom(), _indexOffset(0), _indexFactor(0), _seed(0), _seed2(0) {
  setSeed(0x7eadbeef, 0x1989211, 0x19910420);
}

PortableRandom::~PortableRandom() {
}

std::string PortableRandom::asString() const {
  auto rc = formatCString("PortableRandom: offsets: %d %d seeds: %lld %lld",
      _indexOffset, _indexFactor, _seed, _seed2);
  return rc;
}

double PortableRandom::nextFloat() {
  nextInt();
  auto rc = (double) _seed / _maxNumber;
  return rc;
}

seed_t PortableRandom::nextInt(seed_t maxValue) {
  _seed = ((_seed % _maxFactor) * _factors[_indexFactor++]
      + _offsets[_indexOffset++] + _seed2) % _maxNumber;
  if (_indexOffset >= _countNumbers) {
    _indexOffset = 0;
  }
  if (_indexFactor >= _countFactors) {
    _indexFactor = 0;
  }
  int rc = _seed % (maxValue + 1);
  //printf("PortableRandom::nextInt(%d): %d\n", maxValue, rc);
  return rc;
}

seed_t PortableRandom::nextInt(seed_t minValue, seed_t maxValue) {
  int range = maxValue - minValue;
  if (range < 0) {
    throw InternalError(
        formatCString("PortableRandom::nextInt(): illegal range: %d %d %d",
            minValue, maxValue, _maxInt));
  }
  int rc = minValue + nextInt(range);
  return rc;
}

int64_t PortableRandom::nextInt50() {
  nextInt();
  int64_t rc = static_cast<int64_t>(floor(_seed));
  return rc;
}

void PortableRandom::setSeed(seed_t seed1, seed_t seed2, seed_t seed3) {
  _seed = abs(seed1 + seed2 + seed3);
  _seed2 = seed2;
  _indexOffset = abs(seed1 + seed3) / 47 % (_countNumbers - 1);
  _indexFactor = abs(seed1 + seed2) / 61 % (_countFactors - 1);
  //printf("PortableRandom::setSeed: %ld %ld %d %d\n", _seed, _seed2,
  //    _indexOffset, _indexFactor);
}

PortableRandom*
PortableRandom::instance() {
  if (_globalInstance == nullptr) {
    _globalInstance = new PortableRandom();
  }
  return _globalInstance;
}

} /* cppknife */
