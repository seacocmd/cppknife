/*
 * StringList.hpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_STRINGTOOL_HPP_
#define CORE_STRINGTOOL_HPP_

namespace cppknife {
/// Defines some functions for string handling: C-strings and <em>std::string<em> items.
extern const std::regex stringToolRegexWhitespaces;
extern const std::regex stringToolRegexWord;

/// A helper class for maps with <em>const char*</em> keys.
/**
 * A comparator for <em>const char*</em> pointers.
 * This type is needed for maps with <em>const char*</em> keys.
 * Example:
 * <em>std::map<const char*, int, StringComparism> _map;</em>
 */
struct StringComparism {
  bool operator()(char const *a, char const *b) const;
};

/**
 * Appends a string at the end of a char buffer.
 * @param[in out] buffer The buffer to append.
 * @param bufferLength The occupied space in the buffer.
 * @param bufferSize The capacity of the buffer.
 * @param tail The string to append.
 * @param tailLength &lt; 0: <em>strlen(tail)</em> is used. Otherwise: the length of the tail.
 * @return <em>buffer</em> (for chaining).
 */
char*
appendCString(char *buffer, size_t &bufferLength, size_t bufferSize,
    const char *tail, int tailLength = -1);

/**
 * Appends an integer to a string.
 * @param[in out] string The string to extend.
 * @param data The double to append.
 * @param format The format of the integer, @see sprintf().
 * @return The <em>string</em>: for chaining.
 */
std::string& appendDouble(std::string &string, double data, const char *format =
    "%f");

/**
 * Appends an integer to a string.
 * @param[in out] string The string to extend.
 * @param data The integer to append.
 * @param format The format of the integer, @see sprintf().
 * @return The <em>string</em>: for chaining.
 */
std::string& appendInt(std::string &string, int data,
    const char *format = "%d");

/**
 * Appends one or two C strings to a std:string.
 * @param[in out] string That string will be expanded.
 * @param source1 The first C string to append. May be <em>nullptr</em>.
 * @param source2 The second C string to append. May be <em>nullptr</em>.
 * @return The <em>string</em>: for chaining.
 */
std::string&
appendString(std::string &string, const char *source1, const char *source2 =
    nullptr);
/**
 * Returns the value of a hex digit.
 * @param hexDigit A hex digit.
 * @return -1: <em>hexDigit</em> is not a hexadecimal digit. Otherwise: the value: 0..15
 */
int charToNibble(char hexDigit);
/**
 * Duplicates a string.
 *
 * Note: the result must be freed with delete[] after life time.
 * @param source The string to copy. If <em>nullptr</em> an empty string will be copied.
 * @param sourceLength The length of the source in characters. If -1 the length will be determined by <em>strlen()</em>.
 * @return A copy of the <em>source</em>.
 */
const char*
copyCString(const char *source, int sourceLength = -1);
/**
 * Copies a string limited by length n into a target.
 * @param[out] target The target buffer.
 * @param targetSize The size of <em>target</em>.
 * @param source The string to copy.
 * @param sourceLength &lt; 0: <em>strlen(source)</em> is used. Otherwise: the length of <em>source</em>.
 */
const char* copyNCString(char *target, size_t targetSize, const char *source,
    int sourceLength = -1);
/**
 * Counts the occurrences of the character <em>toSearch</em> in <em>source</em>.
 * @param source In that string will be searched.
 * @param toSearch The character to search.
 * @return the count of <em>toSearch</em> in <em>source</em>.
 */
size_t countCharInCString(const char *source, char toSearch);
/**
 * Counts the occurrences of <em>subString</em> in <em>source</em>.
 * @param source In that string will be searched.
 * @param sourceLength -1: <em>strlen(source)</em> will be used. Otherwise: The size of <em>source</em>.
 * @param subString That string will searched in <em>source</em>.
 * @param subStringLength -1: <em>strlen(subString)</em> will be used. Otherwise: The size of <em>subString</em>.
 */
size_t countCString(const char *source, int sourceLength, const char *subString,
    int subStringLength = -1);
/**
 * Calculates the CRC-32 check sum.
 * @param buffer The buffer to inspect.
 * @param bufferLength The length of <em>buffer</em>.
 * @param lastCall <em>true</em>The last part of the data is processed.
 *  In this case the result is inverted. Set it to <em>false</em> to process more data.
 * @return The CRC-32 checksum.
 */
uint32_t crc32(uint8_t *buffer, size_t bufferLength, bool lastCall = true);
/**
 * Updates a CRC-32 checksum with data from a buffer.
 * @param buffer The buffer to inspect.
 * @param bufferLength The length of <em>buffer</em>.
 * @param[in out] checkSum This checksum will be updated.
 *  That is usable to build a checksum over multiple data buffers. Use <em>crc32()</em> to initialize it.
 * @param lastCall <em>true</em>The last part of the data is processed.
 *  In this case the result is inverted. Set it to <em>false</em> to process more data.
 * @return <em>checkSum</em> (for chaining)
 */
uint32_t crc32Update(uint8_t *buffer, size_t bufferLength, uint32_t &checkSum,
    bool lastCall);
/**
 * Tests whether a given string is the end of a given text.
 * @param source The string to inspect
 * @param sourceLength -1 or the length of source to inspect.
 * @param tail That string should be the end of <em>source</em>.
 * @param tailLength Only that amount of <em>tail</em> will be inspected.<br>
 *   If &lt; 0 the string length of tail is taken.
 * @param ignoreCase true: the case of <em>source</em> and <em>tail</em> will be ignored.
 * @return true: <em>source</em> ends with <em>end</em> .
 */
bool
endsWith(const char *source, int sourceLength, const char *tail,
    int tailLength = -1, bool ignoreCase = false);
/**
 * Formats a string into a <em>std::string</em>.
 * @param format The format with placeholders like in <em>sprintf()</em>.
 * @param ... The variable amount of arguments (like in <em>sprintf()</em>).
 */
std::string
formatCString(const char *format, ...);
/**
 * Formats a string into a <em>std::string</em>.
 * @param buffer The formatted string will be composed in that buffer.
 * @param bufferSize The size of the <em>buffer</em>.
 * @param format The format with placeholders like in <em>sprintf()</em>.
 * @param ... The variable amount of arguments (like in <em>sprintf()</em>).
 */
const char*
formatOnBuffer(char *buffer, size_t bufferSize, const char *format, ...);
/**
 * Converts a glob pattern into a regular expression.
 * @param globPattern The pattern to convert.
 * @param length The length of <em>globPattern</em> or <em>-1</em> (c-string).
 * @return The regular expression that is equivalent to <em>globPattern</em>.
 */
std::string globToRegularExpression(const char *globPattern, int length = -1);
/**
 * Finds the position of a <em>part</em> in a <em>source</em> starting at a <em>position</em>.
 * @param source The string to inspect.
 * @param sourceLength -1 or that length of <em>source</em> should be inspected.
 * @param part The string to search.
 * @param lengthPart -1 or the length of <em>part</em>.
 * @param start The first index of <em>source</em> to inspect.
 * @param ignoreCase true: the case of <em>source</em> and <em>part</em> will be ignored.
 * @return -1: not found. Otherwise: the index in <em>source</em> where <em>part</em> is found.
 */
int
indexOf(const char *source, int sourceLength, const char *part, int lengthPart,
    int start = 0, bool ignoreCase = false);
/**
 * Returns the first index of a given element in a given list.
 * @param list The list to inspect.
 * @param element The element to search.
 * @param elementLength The lenght of <em>element</em> or <em>-1</em> (<em>strlen(element)</em>).
 * @param isSorted <em>true</em>: the list is sorted.
 * @return <em>-1</em>: not found. Otherwise: the index of the <em>element</em> in the <em>list</em>.
 */
int indexOf(const std::vector<std::string> &list, const char *element,
    int elementLength = -1, bool isSorted = false);
bool isBool(const char *source, int sourceLength = -1, bool *value = nullptr,
    std::string *error = nullptr);
bool isFloat(const char *source, int sourceLength = -1, double *value = nullptr,
    std::string *error = nullptr);
bool isInt(const char *source, int sourceLength = -1, int *value = nullptr,
    std::string *error = nullptr);
bool isNat(const char *source, int sourceLength = -1, size_t *value = nullptr,
    std::string *error = nullptr);
bool isSize(const char *source, int sourceLength = -1, int64_t *value = nullptr,
    std::string *error = nullptr, bool mayBeNegative = false);
/**
 * Concatenate until 4 C strings in a buffer.
 * @param[out] buffer The concatenation will be copied here.
 * @param bufferSize The size of <em>buffer</em>.
 * @param string1 The first string to append. May be <em>nullptr</em>.
 * @param string2 The second string to append. May be <em>nullptr</em>.
 * @param string3 The third string to append. May be <em>nullptr</em>.
 * @param string4 The forth string to append. May be <em>nullptr</em>.
 */
char*
joinCStrings(char *buffer, size_t bufferSize, const char *string1,
    const char *string2, const char *string3 = nullptr, const char *string4 =
        nullptr);
/**
 * Concatenates the entries of a string list into a string.
 * @param array The string list.
 * @param separator <em>nullptr</em> or the string between the entries.
 * @return the concatenation of the string entries separated by a given separator.
 */
std::string
joinVector(const std::vector<std::string> &array, const char *separator =
    nullptr);
/**
 * Returns the keys of a map.
 * @param map: the map to inspect
 * @return: the keys of the map
 */
template<typename K, typename V> std::vector<K> keysOfMap(std::map<K, V> map) {
  std::vector<K> rc;
  rc.reserve(map.size());
  for (auto it = map.cbegin(); it != map.cend(); ++it) {
    auto value = it->first;
    rc.push_back(value);
  }
  return rc;
}
/**
 * Replaces a substring in a given string.
 * @param[in out] string The string to change.
 * @param what The substring that will be replaced.
 * @param to The replacement
 * @param count The number of replacements. If &lt; 0: replace all.
 */
std::string& replaceString(std::string &string, const std::string &what,
    const std::string &to, int count = -1);
/**
 * Sets the content of a string.
 * @param[out] string The string to set.
 * @param source The new content.
 * @param sourceLength: -1: <em>strlen(source)</em> will be used. <br>
 *   Otherwise: the length of <em>source</em>.
 */
const std::string& setString(std::string &string, const char *source,
    int sourceLength = -1);
/**
 * Splits a string into a vector of strings by a given separator.
 */
std::vector<std::string>
splitCString(const char *text, const char *separator, int maxCount = -1);
/**
 * Splits a string into parts with a given separator (regular expression).
 * @param text The text to split.
 * @param separator The regular expression describing the separator.
 * @return A vector with the text split by the <em>separator</em>.
 */
std::vector<std::string> splitString(const std::string &text,
    const std::regex &separator);
/**
 * Tests whether a given string is the start of a given text.
 * @param source The string to inspect
 * @param sourceLength -1 or the length of source to inspect.
 * @param head That string should be the head of <em>source</em>.
 * @param maxLength Only that amount of <em>head</em> will be inspected.<br>
 *   If &lt; 0 the string length of tail is taken.
 * @param ignoreCase true: the case of <em>source</em> and <em>head</em> will be ignored.
 * @return true: <em>source</em> starts with <em>head</em> .
 */
bool
startsWith(const char *source, int sourceLength, const char *head,
    int maxLength = -1, bool ignoreCase = false);
std::string
stringInplace(const std::string &source, char*
(*function)(char*));
/**
 * Transform a string into a regular expression: All meta characters will be escaped.
 * Example: "delimiter: '*'" is transformed to "delimiter: '\\*'"
 * @param pattern The string to process.
 * @param length <em>-1</em> or the length of <em>pattern</em>.
 * @return A copy of <em>pattern</em> with escaped meta characters.
 */
std::string stringToRegularExpression(const char *pattern, ssize_t length = -1);
/**
 * Converts a C string to lower case (ASCII only) in place.
 * @param[in out] string The string to convert.
 */
inline void strlwr(char *string) {
  while (*string != '\0') {
    *string = tolower(*string);
  }
}
/**
 * Compares two strings ignoring case.
 */
int stricmp(const char *text1, const char *text2);
/**
 * Compares two strings with a given max length and ignoring case.
 */
int
strnicmp(const char *text1, const char *text2, size_t n);
/**
 * Converts a C string to upper case (ASCII only) in place.
 * @param[in, out] string The string to convert.
 */
inline void strupr(char *string) {
  while (*string != '\0') {
    *string = toupper(*string);
  }
}
size_t
sumOfLength(const char *string1, const char *string2, const char *string3 =
    nullptr, const char *string4 = nullptr);
/**
 * Returns the sum of the string lengths of a string list.
 * @param list The list of strings.
 * @return the sum of the string lengths of the items.
 */
size_t
sumOfVectorLengths(const std::vector<std::string> &list);
/**
 * Removes specified characters from the top and/or the end of a string.
 * @param[in out] string The string to modify.
 * @param top <em>true</em>The <em>chars</em> will removed from the top.
 * @param end <em>true</em>The <em>chars</em> will removed from the end.
 * @param chars The characters that will be removed.
 * @return <em>string</em> (for chaining).
 */
std::string& trimString(std::string &string, bool top = true, bool end = true,
    const char *chars = " \t\n\r\v");
/**
 * Converts a string to upper case.
 * @param source This string will be converted.
 * @return The source with upper case content.
 */
std::string
toUpper(const std::string &source);
/**
 * Converts a string to lower case.
 * @param source This string will be converted.
 * @return The source with upper case content.
 */
std::string
toLower(const std::string &source);
/**
 * Truncates a string to a given length.
 * @param text: The string to inspect.
 * @param length: The maximal length of the result.
 * @return The truncated text.
 */
std::string
truncateCString(const char *text, size_t length);
} /* namespace polygeo */

#endif /* CORE_STRINGTOOL_HPP_ */
