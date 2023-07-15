/*
 * Base64.cpp
 *
 *  Created on: 27.09.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "text.hpp"

namespace cppknife {

static const ubyte_t *s_tableBase64 =
    (const ubyte_t*) ("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
static const ubyte_t s_inversBase64[256] = { 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 62, 255, 255, 255, 63, 52, 53, 54, 55, 56, 57,
    58, 59, 60, 61, 255, 255, 255, 0, 255, 255, 255, 0, 1, 2, 3, 4, 5, 6, 7, 8,
    9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 255, 255,
    255, 255, 255, 255, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
    40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255 };

std::string& encodeBase64(const ubyte_t *buffer, size_t bufferLength,
    std::string &result, size_t lineSize) {
  size_t outputLength = 0;
  int lineLength = 0;

  // 3 bytes input needs 4 char output:
  outputLength = bufferLength * 4 / 3 + 4;
  if (lineSize > 0) {
    // round down to a 4 char bound:
    lineSize = lineSize / 4 * 4;
    outputLength += outputLength / lineSize;
  }
  // End of string:
  outputLength++;
  // Arithmetic overflow?:
  if (outputLength >= bufferLength) {
    result.reserve(outputLength + 1);
    char *ptrOutput = const_cast<char*>(result.c_str());
    const ubyte_t *endOfInput = buffer + bufferLength;
    const ubyte_t *ptrInput = buffer;
    lineLength = 0;
    while (endOfInput - ptrInput >= 3) {
      // Take the 6 higher bits from x[0]
      *ptrOutput++ = s_tableBase64[ptrInput[0] >> 2];
      // Take the two lower bits from x[0] and the 4 higher bits from x[1]
      *ptrOutput++ = s_tableBase64[((ptrInput[0] & 0x03) << 4)
          | (ptrInput[1] >> 4)];
      // Take the 4 lower bits from x[1] and the the two higher bits from x[2]
      *ptrOutput++ = s_tableBase64[((ptrInput[1] & 0x0f) << 2)
          | (ptrInput[2] >> 6)];
      // Take the 6 lower bits from x[2]
      *ptrOutput++ = s_tableBase64[ptrInput[2] & 0x3f];
      ptrInput += 3;
      lineLength += 4;
      if (static_cast<size_t>(lineLength) >= lineSize) {
        *ptrOutput++ = '\n';
        lineLength = 0;
      }
    }
    if (ptrInput != endOfInput) {
      *ptrOutput++ = s_tableBase64[ptrInput[0] >> 2];
      if (ptrInput + 1 == endOfInput) {
        *ptrOutput++ = s_tableBase64[(ptrInput[0] & 0x03) << 4];
        *ptrOutput++ = '=';
      } else {
        *ptrOutput++ = s_tableBase64[((ptrInput[0] & 0x03) << 4)
            | (ptrInput[1] >> 4)];
        *ptrOutput++ = s_tableBase64[(ptrInput[1] & 0x0f) << 2];
      }
      *ptrOutput++ = '=';
      lineLength += 4;
    }
    *ptrOutput = '\0';
  }
  return result;
}

ubyte_t* decodeBase64(const char *source, size_t sourceLength,
    size_t &outputLength, ubyte_t *buffer, size_t bufferSize) {
  static const ubyte_t UNDEF = 0xFF;
  ubyte_t *rc = nullptr;
  size_t ix = 0;
#ifdef CREATE_INVERSE_TABLE
  memset((void*) s_inversBase64, UNDEF, 256);
  for (ix = 0; ix < 64; ix++) {
    ((ubyte_t*) s_inversBase64)[s_tableBase64[ix]] = (ubyte_t) ix;
  }
  ((ubyte_t*) s_inversBase64)['='] = 0;
  for (ix = 0; ix < 256; ix++) {
    printf("%d, ", s_inversBase64[ix]);
  }
  printf("\n");
#endif
  size_t count = 0;
  for (ix = 0; ix < sourceLength; ix++) {
    if (s_inversBase64[static_cast<int>(source[ix])] != UNDEF) {
      count++;
    }
  }
  if (count > 0 && count % 4 == 0) {
    size_t outputLength2 = count / 4 * 3;
    rc = outputLength2 > bufferSize ? new ubyte_t[outputLength2] : buffer;
    ubyte_t *ptrOutput = rc;
    count = 0;
    int countPadding = 0;
    char block[4];
    for (ix = 0; ix < sourceLength; ix++) {
      ubyte_t current = s_inversBase64[static_cast<int>(source[ix])];
      if (current == UNDEF) {
        continue;
      }

      if (source[ix] == '=')
        countPadding++;
      block[count] = current;
      count++;
      if (count == 4) {
        *ptrOutput++ = (block[0] << 2) | (block[1] >> 4);
        *ptrOutput++ = (block[1] << 4) | (block[2] >> 2);
        *ptrOutput++ = (block[2] << 6) | block[3];
        count = 0;
        if (countPadding) {
          if (countPadding == 1)
            ptrOutput--;
          else if (countPadding == 2)
            ptrOutput -= 2;
          else {
            /* Invalid padding */
            free(rc);
            return NULL;
          }
          break;
        }
      }
    }
    outputLength = ptrOutput - rc;
  }
  return rc;
}

} /* namespace */
