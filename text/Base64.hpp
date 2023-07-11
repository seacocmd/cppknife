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
 * @param[out] out_len: the length of the result (including the terminating '\0').
 * @param lineSize: 0: no additional newlines. Otherwise: Behind that count of output chars a newline is put.
 * @return <em>nullptr</em>: error Otherwise: The encoded string. The caller must delete that!
 */
char* encodeBase64(const ubyte_t *buffer, size_t bufferLength, size_t *out_len,
    size_t lineSize = 80);

/**
 * Decodes a BASE64 string into binary data.
 * @param source The base64 string to decode.
 * @param sourceLength The length of <em>source</em>.
 * @param outputLength the length of the result.
 * @result The decoded data. The caller must delete it.
 */
ubyte_t* decodeBase64(const char *source, size_t sourceLength,
    size_t *outputLength);
} /* namespace cppknife */

#endif /* TEXT_BASE64_HPP_ */
