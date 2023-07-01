/*
 * CharRandom_test.cpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"

namespace cppknife {

static bool onlyFewTests() {
  return false;
}
#define FEW_TESTS if (onlyFewTests()) return

TEST(CharRandomTest, randomText) {
  FEW_TESTS;
  BaseRandom random2;
  CharRandom random1(random2);
  random2.setSeed(0x12345678);
  auto value = random1.randomText(80, 5);
  ASSERT_STREQ(value.c_str(),
      "#96[r)Kc3\"\nh[I2N.96=lb8=LL-$u7+uNAR\nGz%[|{AXES$Tm;qWz*tDS\"E\n7P=bqo$7j&re\"1;Ge={P");
}

TEST(CharRandomTest, nextChar) {
  FEW_TESTS;
  BaseRandom random2;
  CharRandom random1(random2);
  random2.setSeed(0x12345678);
  char buffer[32];
  size_t bufferLength = 0;
  buffer[bufferLength++] = random1.nextChar(CC_DECIMAL);
  buffer[bufferLength++] = random1.nextChar(CC_ALPHA);
  buffer[bufferLength++] = random1.nextChar(CC_ALPHA_LOWERCASE);
  buffer[bufferLength++] = random1.nextChar(CC_ALPHA_UPPERCASE);
  buffer[bufferLength++] = random1.nextChar(CC_WORD_FIRST);
  buffer[bufferLength++] = random1.nextChar(CC_WORD);
  buffer[bufferLength++] = random1.nextChar(CC_ASCII95);
  buffer[bufferLength++] = random1.nextChar(CC_ASCII95);
  buffer[bufferLength++] = '\0';

  ASSERT_STREQ(buffer, "5htYTX)K");
}
TEST(CharRandomTest, fill) {
  FEW_TESTS;
  BaseRandom random2;
  CharRandom random1(random2);
  random2.setSeed(0x12345678);
  char buffer[30];
  size_t bufferLength = 0;
  random1.fill(sizeof buffer - 1, buffer, bufferLength, sizeof buffer, CC_WORD);
  ASSERT_STREQ(buffer, "jBosKXRxdI52yGEEGFj4U4tlLD0az");
}
TEST(CharRandomTest, testCharClass) {
  FEW_TESTS;
  BaseRandom random2;
  random2.setSeed(0x12345678);
  CharRandom random1(random2);
  char buffer[80];
  size_t bufferLength = 0;
  for (CharClass clazz = static_cast<CharClass>(CC_UNDEF + 1);
      clazz <= static_cast<CharClass>(CC_ASCII95); clazz =
      static_cast<CharClass>(clazz + 1)) {
    bufferLength = 0;
    random1.fill(sizeof buffer - 1, buffer, bufferLength, sizeof buffer, clazz);
    ASSERT_TRUE(CharRandom::testCharClass(clazz, buffer, -1));
  }
}
TEST(CharRandomTest, veil) {
  FEW_TESTS;
  PortableRandom random2;
  CharRandom random1(random2, nullptr, CC_WORD);
  char buffer[80];
  ASSERT_EQ('a', random1.veil('A'));
  const char* text = "Jimmy goes to Hollywood";
  random1.veil("Jimmy goes to Hollywood", buffer, sizeof buffer);
  ASSERT_EQ(strlen(text) + 4, strlen(buffer));
}

TEST(CharRandomTest, unveil) {
  //FEW_TESTS;
  char buffer[128];
  char buffer2[128];
  PortableRandom random2;
  CharRandom random1(random2, nullptr, CC_ALPHA);
  random1.reset();
  char cc = random1.veil('A');
  random1.reset();
  ASSERT_EQ('A', random1.unveil(cc));
  const char *text = "2 Jimmies go to Hollywood!";
  random1.reset();
  printf("%s\n", random1.asString().c_str());
  random1.veil(text, buffer, sizeof buffer);
  random1.reset();
  printf("%s\n", random1.asString().c_str());
  random1.unveil(buffer, buffer2, sizeof buffer2);
  ASSERT_STREQ(buffer2, text);
  for (CharClass clazz = static_cast<CharClass>(CC_UNDEF + 1);
      clazz <= static_cast<CharClass>(CC_ASCII95); clazz =
          static_cast<CharClass>(clazz + 1)) {
    random1.setCharSet(clazz);
    random1.reset();
    random1.veil(text, buffer, sizeof buffer);
    random1.reset();
    random1.unveil(buffer, buffer2, sizeof buffer2);
    ASSERT_STREQ(buffer2, text);
  }
}
TEST(CharRandomTest, unvail2) {
  FEW_TESTS;
  char buffer[128];
  char buffer2[128];
  char buffer3[128];
  BaseRandom randomBase;
  KissRandom randomKiss;
  PortableRandom randomPortable;
  for (int type = 0; type < 3; type++) {
    BaseRandom *random0;
    switch (type) {
      case 0:
      random0 = &randomBase;
      break;
      case 1:
      random0 = &randomKiss;
      break;
      default:
      random0 = &randomPortable;
      break;
    }
    CharRandom random1(*random0, nullptr, CC_ASCII95);
    for (int ix = 0; ix < 5000; ix++) {
      // salt size 4:
      size_t len = sizeof buffer - 4 - 1;
      size_t bufferLength = 0;
      random1.fill(len, buffer, bufferLength, sizeof buffer, CC_ASCII95, true);
      random1.reset();
      //printf("  veil: %s %s\n", random1.asString().c_str(),
      //    random0.asString().c_str());
      random1.veil(buffer, buffer2, sizeof buffer2);
      random1.reset();
      //printf("unvail: %s %s\n", random1.asString().c_str(),
      //    random0.asString().c_str());
      random1.unveil(buffer2, buffer3, sizeof buffer3);
      if (strcmp(buffer, buffer3) != 0) {
        ASSERT_STREQ(buffer, buffer3);
      }
    }
  }
}

}
/* namespace cppknife */
