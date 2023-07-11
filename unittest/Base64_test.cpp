/*
 * Base64_test.cpp
 *
 *  Created on: 27.09.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "google_test.hpp"
#include "../text/text.hpp"

using namespace cppknife;

static bool onlyFewTests() {
  return true;
}
#define FEW_TESTS() if (onlyFewTests()) return

TEST(Base64Test, encode) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  ubyte_t input[] = { "With a little help from my friends" };
  auto length = strlen((const char*) input);
  size_t outputLength = 0;

  auto output = encodeBase64(input, length, &outputLength);
  ASSERT_STREQ((const char* )output,
      "V2l0aCBhIGxpdHRsZSBoZWxwIGZyb20gbXkgZnJpZW5kcw==");
  ASSERT_EQ(outputLength, 48);
  delete output;
  delete logger;
}

TEST(Base64Test, encode2) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  ubyte_t input[] = { 0, 1, 2, 3, 4, 5 };
  auto length = sizeof input;
  size_t outputLength = 0;
  auto output = encodeBase64(input, length, &outputLength);
  ASSERT_STREQ((const char* )output, "AAECAwQF");
  ASSERT_EQ(outputLength, 48);
  delete output;
  delete logger;
}

TEST(Base64Test, decode) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  const char *input = "V2l0aCBhIGxpdHRsZSBoZWxwIGZyb20gbXkgZnJpZW5kcw==";
  auto length = strlen(input);
  size_t outputLength = 0;

  auto output = decodeBase64(input, length, &outputLength);
  ASSERT_STREQ((const char* )output, "With a little help from my friends");
  delete output;
  delete logger;
}
TEST(Base64Test, decode2) {
  //FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  const char *input = "AAECAwQF";
  auto length = strlen(input);
  size_t outputLength = 0;

  auto output = decodeBase64(input, length, &outputLength);
  ASSERT_EQ(output[0], 0);
  ASSERT_EQ(output[1], 1);
  ASSERT_EQ(output[2], 2);
  ASSERT_EQ(output[3], 3);
  delete output;
  delete logger;
}

