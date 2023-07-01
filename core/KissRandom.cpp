/*
 * KissRandom.cpp
 *
 *  Created on: 08.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "core.hpp"
namespace cppknife {

static const int DEFAULT_X = 0x20111958;
static const int DEFAULT_Y = 0x11081965;
static const int DEFAULT_Z = 0x01091988;
static const int DEFAULT_C = 0x12111957;

KissRandom::KissRandom() :
    BaseRandom(), m_x(DEFAULT_X), m_y(DEFAULT_Y), m_z(DEFAULT_Z), m_c(DEFAULT_C) {
}
KissRandom::~KissRandom() {
}
std::string KissRandom::asString() const {
  auto rc = formatCString("CharRandom: m: %d y: %d z: %d c: %d", m_x, m_y, m_z,
      m_c);
  return rc;
}

seed_t KissRandom::nextInt(seed_t maxValue) {
  // congruential generator:
  m_x = 0x11121989 * m_x + 0x20041991;
  // xor shift:
  m_y ^= m_y << 13;
  m_y ^= m_y >> 17;
  m_y ^= m_y << 5;
  // multiply with carry:
  uint64_t t = 698769069 * m_z + m_c;
  m_c = t >> 32;
  m_z = (seed_t) t;
  seed_t rc = m_x + m_y + m_z;
  if (maxValue != 0)
    rc %= maxValue;
  return rc < 0 ? -rc : rc;
}

void KissRandom::reset() {
  m_x = DEFAULT_X;
  m_y = DEFAULT_Y;
  m_z = DEFAULT_Z;
  m_c = DEFAULT_C;
}
void KissRandom::setSeed(seed_t seed1, seed_t seed2, seed_t seed3) {
  reset();
  m_x = seed1 != 0 ? seed1 : DEFAULT_X;
  m_y = seed2 != 0 ? seed2 : DEFAULT_Y;
  m_z = seed3 != 0 ? seed3 : DEFAULT_Y;
}
void KissRandom::setPhrase(const char *text1, const char *text2,
    const char *text3) {
  seed_t seed1 = BaseRandom::hashString(text1);
  seed_t seed2 = text2 == nullptr ? DEFAULT_Y : BaseRandom::hashString(text2);
  seed_t seed3 = text3 == nullptr ? DEFAULT_Z : BaseRandom::hashString(text3);
  setSeed(seed1, seed2, seed3);
}
RealKissRandom::RealKissRandom() :
    KissRandom() {
  int *x = new int[2];
  m_x = time(nullptr) ^ DEFAULT_X;
  m_y = (int) (int64_t) x;
  m_z = clock() ^ DEFAULT_Z;
  m_c = (int) (int64_t) this;
}
}
