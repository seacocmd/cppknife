/*
 * KissRandom.hpp
 *
 *  Created on: 08.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef BASIC_KISSRANDOM_HPP_
#define BASIC_KISSRANDOM_HPP_

namespace cppknife {

class RealRandom;
/**
 * @brief Implements the KISS algorithm for pseudo random numbers.
 *
 * @see https://en.wikipedia.org/wiki/KISS_(algorithm)
 */
class KissRandom: public BaseRandom {
protected:
  // Should never be 0!
  seed_t m_x;
  seed_t m_y;
  seed_t m_z;
  seed_t m_c;
public:
  KissRandom();
  virtual ~KissRandom();
public:
  virtual std::string asString() const;
  virtual int nextInt(seed_t maxValue = 0x7ffffffe);
  void reset();
  virtual void setSeed(seed_t seed1, seed_t seed2 = 0, seed_t seed3 = 0);
  virtual void setPhrase(const char *text1, const char *text2 = nullptr,
      const char *text3 = nullptr);
};

/**
 * @brief Offers a random generator based on the KISS algorithm with unpredictable values.
 *
 * The start state of the pseudo random generator is set to a unpredictable value.
 */
class RealKissRandom: public KissRandom {
public:
  RealKissRandom();
};

} /* namespace */
#endif /* BASIC_KISSRANDOM_HPP_ */
