/*
 * BaseRandom.hpp
 *
 *  Created on: 02.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_BASERANDOM_HPP_
#define CORE_BASERANDOM_HPP_

namespace cppknife {

typedef int32_t seed_t;

class BaseRandom {
protected:
  static BaseRandom *_globalInstance;
  static const int _maxInt = 0x7ffffffe;
  seed_t _lastSeed;
public:
  BaseRandom();
  virtual
  ~BaseRandom();
  BaseRandom&
  operator=(const BaseRandom &other);
  BaseRandom(const BaseRandom &other);
public:
  virtual std::string asString() const;
  /**
   * Returns a random bool value.
   * @return A random character.
   */
  virtual bool
  nextBool();
  /**
   * Returns a random float number from [0, 1.0[.
   * @return A random character.
   */
  virtual double
  nextFloat();
  /**
   * Returns a random integer from [0, maxValue].
   * @return A random integer.
   */
  virtual int
  nextInt(int maxValue = _maxInt);
  /**
   * Returns a random integer from [minValue, maxValue].
   * @return A random integer.
   */
  virtual int
  nextInt(int minValue, int maxValue);
  /**
   * Returns a random 50 bit integer.
   * @return A random integer in [0..2**50-1].
   */
  virtual int64_t nextInt50();
  /**
   * Returns a random 63 bit integer.
   * @return A random integer in [0..2**63-1].
   */
  virtual int64_t nextInt63();
  /**
   * Sets the instance into a well known state.
   * This is needed if the random sequences should be reproduced
   * or should depend on one until three secrets.
   * @param text1 The first secret that is converted into a seed.
   * @param text2 The second secret that is converted into a seed.
   * @param text3 The third secret that is converted into a seed.
   *
   */
  virtual void
  setPhrase(const char *text1, const char *text2 = nullptr, const char *text3 =
      nullptr);
  /**
   * Sets the instance into a well known state.
   * This is needed if the random sequences should be reproducible.
   * @param seed1 This value defines the state (partitially).
   * @param seed2 This value defines the state (partitially).
   * @param seed3 This value defines the state (partitially).
   */
  virtual void
  setSeed(seed_t seed1, seed_t seed2 = 0x7654321, seed_t seed3 = 0x3adf001);
public:
  /**
   * Builds a hash value for a given text.
   * Note: That method is portable for all programming languages knowing IEEE doubles.
   * @param text The text to hash.
   * @param length The length of <em>text</em>. If <em>-1</em> the <em>strlen(text)</em> is taken.
   * @return An integer value in [0, 2**32-1].
   */
  static int hashString(const char *text, int length = -1);
  static BaseRandom*
  instance();
  static void
  testDistribution50(BaseRandom &random, size_t loops = 5000);
  static void
  testDistributionInt(BaseRandom &random, int maxInt, size_t loops = 5000);
};

} /* cppknife */

#endif /* CORE_BASERANDOM_HPP_ */
