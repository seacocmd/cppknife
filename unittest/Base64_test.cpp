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
  return false;
}
#define FEW_TESTS() if (onlyFewTests()) return


TEST(Base64Test, encode) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  ubyte_t input[] = { "With a little help from my friends" };
  auto length = strlen((const char*) input);
  std::string output;
  encodeBase64(input, length, output);
  ASSERT_STREQ(output.c_str(),
      "V2l0aCBhIGxpdHRsZSBoZWxwIGZyb20gbXkgZnJpZW5kcw==");
  delete logger;
}

TEST(Base64Test, encode2) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  ubyte_t input[] = { 0, 1, 2, 3, 4, 5 };
  auto length = sizeof input;
  std::string output;
  encodeBase64(input, length, output);
  ASSERT_STREQ(output.c_str(), "AAECAwQF");
  delete logger;
}

TEST(Base64Test, decode) {
  //FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  const char *input = "V2l0aCBhIGxpdHRsZSBoZWxwIGZyb20gbXkgZnJpZW5kcw==";
  auto length = strlen(input);
  size_t outputLength = 0;
  ubyte_t buffer[128];
  memset(buffer, 0, sizeof buffer);
  auto output = decodeBase64(input, length, outputLength, buffer,
      sizeof buffer);
  ASSERT_STREQ((const char* )output, "With a little help from my friends");
  ASSERT_EQ(outputLength, strlen("With a little help from my friends"));
  delete logger;
}
TEST(Base64Test, decode2) {
  FEW_TESTS();
  auto logger = buildMemoryLogger(100, LV_FINE);

  const char *input = "AAECAwQF";
  auto length = strlen(input);
  size_t outputLength = 0;
  auto output = decodeBase64(input, length, outputLength);
  ASSERT_EQ(output[0], 0);
  ASSERT_EQ(output[1], 1);
  ASSERT_EQ(output[2], 2);
  ASSERT_EQ(output[3], 3);
  delete output;
  delete logger;
}
TEST(Base64Test, base64Random) {
  FEW_TESTS();
  ubyte_t data[4096];
  ubyte_t decoded[4096];
  std::string encoded;
  KissRandom random;
  size_t length = 0;
  auto start = time(nullptr);
  size_t total = 0;
  for (auto round = 0; round < 10000; round++) {
    auto dataLength = random.nextInt(sizeof data - 1);
    for (auto ix = 0; ix < dataLength; ix++) {
      data[ix] = random.nextInt(255);
    }
    encodeBase64(data, dataLength, encoded, 132);
    total += dataLength;
    decodeBase64(encoded.c_str(), encoded.size(), length, decoded,
        sizeof decoded);
    ASSERT_EQ(0,
        memcmp((const void* ) decoded, (const void* ) data, length));
  }
  auto diff = time(nullptr) - start;
  printf("total: %.3f Mb time: %ld\n", total / 1024.0 / 1024.0, diff);
}



