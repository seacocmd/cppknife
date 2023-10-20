/*
 * Base64.hpp
 *
 *  Created on: 27.09.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_BASE64_HPP_
#define TEXT_BASE64_HPP_
/// Implements a reader for configuration in Java style. Each line contains a definition like <em>name = Jonny Walker</em>.
namespace cppknife {

typedef unsigned char ubyte_t;

/**
 * Encodes a binary block as Base64 string.
 * @param buffer The binary data block to encode.
 * @param bufferLength The length of the block.
 * @param[out] result: The base64 encoded text as string.
 * @param lineSize: 0: no additional newlines. Otherwise: Behind that count of output chars a newline is put.
 * @return The <em>result</em> (for chaining).
 */
std::string& encodeBase64(const ubyte_t *buffer, size_t bufferLength,
    std::string &result, size_t lineSize = 80);

/**
 * Decodes a BASE64 string into binary data.
 * @param source The base64 string to decode.
 * @param sourceLength The length of <em>source</em>.
 * @param[out] outputLength the length of the result.
 * @param[out] buffer <em>nullptr</em>: the result is a allocated buffer. The caller must delete it.
 * Otherwise: the output is stored in that buffer. The result is this <em>buffer</em>
 * @param bufferSize The size of <em>buffer</em>. If the buffer is too small nothing is done and the result is <em>nullptr</em>.
 * @return <em>nullptr</em>: wrong input or the buffer is too small. Otherwise: if <em>buffer</em> is not <em>nullptr</em>: the <em>buffer</em>.
 *    Otherwise: a allocated buffer that must deleted by the caller.
 */
ubyte_t* decodeBase64(const char *source, size_t sourceLength,
    size_t &outputLength, ubyte_t *buffer = nullptr, size_t bufferSize = 0);
} /* namespace cppknife */

#endif /* TEXT_BASE64_HPP_ */
