/*
 * PortableRandom.hpp
 *
 *  Created on: 03.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_PORTABLERANDOM_HPP_
#define CORE_PORTABLERANDOM_HPP_

namespace cppknife {
typedef int64_t seed50_t;
/// A pseudo random generator which algorithm can be simply ported to other programming languages.
class PortableRandom: public BaseRandom {
public:
  // 2**50-1:
  static seed50_t _maxNumber;
  // 2**25
  static seed50_t _maxFactor;
protected:
  static PortableRandom *_globalInstance;
  static seed50_t _offsets[];
  static int _countNumbers;
  static int _countFactors;
  static seed50_t _factors[];
  static const int _maxInt = 0x7ffffffe;
protected:
  int _indexOffset;
  int _indexFactor;
  seed50_t _seed;
  seed50_t _seed2;
public:
  PortableRandom();
  virtual
  ~PortableRandom();
public:
  virtual std::string asString() const;
  virtual double
  nextFloat();
  virtual seed_t
  nextInt(seed_t maxValue = _maxInt);
  virtual seed_t
  nextInt(seed_t minValue, int maxValue);
  /**
   * Returns a random 50 bit integer.
   * @return A random integer in [0..2**50].
   */
  virtual seed50_t nextInt50();
  virtual void
  setSeed(int seed1, int seed2 = 0x7654321, int seed3 = 0x3adf001);
public:
  static double hashString(const char *text, int length = -1);
  static PortableRandom*
  instance();
};

} /* cppknife */

#endif /* CORE_PORTABLERANDOM_HPP_ */
