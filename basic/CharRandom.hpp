/*
 * CharRandom.hpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef BASIC_CHARRANDOM_HPP_
#define BASIC_CHARRANDOM_HPP_

namespace cppknife {
enum CharClass {
  CC_UNDEF,
  CC_DECIMAL,
  CC_ALPHA,
  CC_ALPHA_LOWERCASE,
  CC_ALPHA_UPPERCASE,
  CC_WORD,
  CC_WORD_FIRST,
  CC_ASCII95
};

class CharRandom {
public:
  static const char *_charClasses[];
  static const size_t _lengthsCharClasses[];
protected:
  BaseRandom &_random;
  BaseRandom *_realRandom;
  BaseRandom *_internRealRandom;
  int _saltSize;
  const char *_charSet;
  CharClass _charClass;
  seed_t _countChars;
  seed_t _indexOfChar[256];
  char _charOfIndex[256];
  const char *_secret1;
  const char *_secret2;
  const char *_secret3;
  seed_t _seedSecret1;
  seed_t _seedSecret2;
  seed_t _seedSecret3;
  bool _fixSalt;
public:
  CharRandom(BaseRandom &pseudoRandom, BaseRandom *realRandom = nullptr,
      CharClass charClass = CC_DECIMAL, int saltSize = 4, bool internalUse =
          false);
  virtual ~CharRandom();
public:
  virtual std::string asString() const;
  static const char* charClassText(CharClass charClass);
  static bool
  testCharClass(CharClass charClass, const char *buffer, int bufferLength = -1);
protected:
  void initialize();
public:
  /**
   * Returns a random character.
   * @param charClass Only characters from that character set will be returned.
   * @return A random character created by the pseudo random generator.
   */
  virtual char
  nextChar(CharClass charClass = CC_ASCII95);
  /**
   * Returns a random character.
   * @param charClass Only characters from that character set will be returned.
   * @return A random character created by the real random generator.
   */
  virtual char
  nextCharRealRandom(CharClass charClass = CC_ASCII95);
  /**
   * Appends random characters to a buffer.
   * @param count The count of characters to append.
   * @param buffer IN/OUT: the buffer to append.
   * @param bufferLength IN/OUT: the current buffer length.
   * @param bufferSize The size of the buffer.
   * @param charClass The fill characters come from that character class.
   * @param useRealRandom true: the real random generator is used, else the pseudo random generator
   * @return The buffer (for chaining).
   */
  virtual char*
  fill(size_t count, char *buffer, size_t &bufferLength, size_t bufferSize,
      CharClass charClass = CC_WORD, bool useRealRandom = false);
  /**
   Returns the random generator of the instance.
   */
  BaseRandom& randomOf() {
    return _random;
  }
  /**
   * Returns a random string with a given length.
   * @param length The length of the result.
   * @param lineCount: if &gt; 0 the result is separated by '\n'.
   * @param clazz Only character from that class will be returned.
   */
  std::string
  randomText(size_t length, size_t lineCount, CharClass clazz = CC_ASCII95);
  /** Returns the salt size of encrypted data.
   * @return The count of bytes used for the salt.
   */
  int saltSize() {
    return _saltSize;
  }
  /**
   * Sets a status to begin a new veil() or unveil() sequence.
   */
  void reset();
  /**
   * Sets the character set used for veil() and unveil().
   * @param charClass The character class: only characters of that class will be translated.
   */
  void setCharSet(CharClass charClass = CC_ASCII95);
  /**
   * Sets the randomly generated salt.
   * @param buffer: OUT: the salt is put here.
   * @param size: the size of the buffer.
   */
  void setSalt(char *buffer, size_t size);
  /**
   * Sets one til three secret phrases to customize encrypting.
   * @param text1 The first secret.
   * @param text2 The second secret. May be nullptr.
   * @param text3 The third secret. May be nullptr.
   */
  void setSecrets(const char *text1, const char *text2 = nullptr,
      const char *text3 = nullptr);
  /**
   * Encrypts one character.
   * @param cc The character to encrypt.
   * @return The encrypted character.
   */
  char veil(char cc);
  /**
   * Encrypts a text.
   * @param text The text to encrypt.
   * @param buffer: OUT: the result is put here.
   * @param size: the size of the buffer.
   */
  void veil(const char *text, char *buffer, size_t size);
  /**
   * Encrypts a text.
   * @param text The text to encrypt.
   * @return The encrypted text.
   */
  std::string veil(const char *text);
  /**
   * Decrypts one character.
   * @param cc The character to decrypt.
   * @return The decrypted character.
   */
  char unveil(char cc);
  /**
   * Decrypts a text.
   * @param text The text to decrypt.
   * @param buffer: OUT: the result is put here.
   * @param size: the size of the buffer.
   */
  void unveil(const char *text, char *buffer, size_t size);
  /**
   * Decrypts a text.
   * @param text The text to decrypt.
   */
  std::string unveil(const char *text);
private:
  void registerChar(char cc);
};

} /* namespace cppknife */

#endif /* BASIC_CHARRANDOM_HPP_ */
