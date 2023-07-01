/*
 * StringList.hpp
 *
 *  Created on: 12.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_STRINGLIST_HPP_
#define CORE_STRINGLIST_HPP_

namespace cppknife {

class StringList {
protected:
  std::vector<const char*> _lines;
  ///
  /// The index behind the last used entry of _lines;
  size_t _nextIndex;
  size_t _startSize;
  size_t _clusterSize;
  size_t _capacity;
  ByteStorage _storage;
public:
  /**
   * Constructor.
   * @param startSize The count of reserved lines at start.
   * @param clusterSize If the list of lines must be expanded that count of lines is added.
   * @param capacity The capacity of the <em>ByteBuffer</em> items (the maximal sequence length).
   */
  StringList(size_t startSize = 100, size_t clusterSize = 500, size_t capacity =
      0x10000);
  virtual
  ~StringList();
  StringList(const StringList &other);
  StringList&
  operator=(const StringList &other);
public:
  /**
   * Append a line at the end of the list.
   * @param line The line to add.
   * @param length The length of the line. If &lt; 0: the length will be determined.
   */
  void
  append(const char *line, int length = -1);
  /**
   * Append a line at the end of the list.
   * @param line The line to add.
   */
  void
  append(const std::string &line);
  /**
   * Returns the entry given by its index.
   * @param index The index of the wanted line.
   * @return "" if index is not valid<br>
   *   Otherwise: the line with that index.
   */
  const char*
  at(size_t index) const;
  size_t clusterSize() const {
    return _clusterSize;
  }
  size_t capacity() const {
    return _capacity;
  }

  /**
   * Remove all lines.
   */
  void
  clear();
  /**
   * Tests whether the list is empty.
   * @return true: the list is empty.
   */
  bool
  empty() const;
  /**
   * Returns a join (concatenation) of all lines.
   * @param separator The separator between each line. If nullptr "" will be used.
   * @return The concatenation of all lines.
   */
  std::string
  join(const char *separator = nullptr) const;
  /**
   * Returns the count of entries.
   * @return The count of entries.
   */
  inline size_t length() const {
    return _nextIndex;
  }
  size_t startSize() const {
    return _startSize;

  }
};
} /* cppknife */

#endif /* CORE_STRINGLIST_HPP_ */
