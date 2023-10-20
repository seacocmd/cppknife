/*
 * LineAgent.hpp
 *
 *  Created on: 02.03.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_LINEAGENT_HPP_
#define TEXT_LINEAGENT_HPP_
namespace cppknife {

/**
 * @brief Manages a line oriented block of a file.
 */
class FileBuffer {
public:
  char _staticBuffer[0x10001];
  /// Points to <em>_staticBuffer</em> or to a allocated buffer.
  char *_buffer;
  /// The usable length of the buffer. The buffer is at least one byte larger
  /// to hold a terminating '\0' (for C strings).
  size_t _bufferSize;
  /// Points to the byte behind the valid buffer contents. Points to the terminating '\0'.
  /// This can be shorter than the buffer size.
  char *_endOfBuffer;
  /// Pointer within the buffer where the next line will start.
  char *_nextLine;
  /// The amount of unprocessed data read from the file.
  size_t _restLength;
  /// The distance of the buffer start to the file start.
  size_t _filePosition;
  /// The distance of the last return value of nextLine() to the buffer start.
  size_t _offsetCurrentLine;
  /// The line number of the current line: the first line has 1.
  size_t _lineNo;
public:
  FileBuffer(size_t bufferSize);
  ~FileBuffer();
public:
  /**
   * Adapts the file position of the block.
   */
  void addOffset() {
    _filePosition += _nextLine - _buffer;
  }
  /**
   * Makes a copy of the meta data of another instance.
   *
   * The buffer contents remains unchanged.
   *
   * @param other The instance to copy.
   */
  void clone(FileBuffer &other);
  /**
   * Increases the buffer.
   * @param clusterSize 0: the buffer size should be doubled. Otherwise: the size is increased by that amount of bytes.
   */
  void increaseBuffer(size_t clusterSize);
  /**
   * Sets the instance at the begin of file.
   */
  void reset();
  /**
   * Sets the next line to the buffer start.
   * @return The buffer start.
   */
  inline const char* resetNextLine() {
    _nextLine = _buffer;
    return _buffer;
  }
  /**
   * Sets another buffer size.
   */
  void setBufferSize(size_t bufferSize);
};
/**
 * @brief Allows splitting a file into lines like <em>FILE</em>: Binary data detection, unlimited line size. previous line(s), file position of the line.
 *
 * Some features of the class:
 * <ul><li>Adapts automatically the buffer size to the largest line size in the file.</li>
 * <li>Returns always the whole line: <em>fgets()</em> works with a fix block size.</li>
 * <li>The file position of the current line can be requested: <em>offsetCurrentLine()</em></li>
 * <li>Avoids memory allocation for one line.</li>
 * <li>At least one previous line can be requested: <em>previousLine()</em>.
 * </ul>
 */
class LineAgent {
protected:
  Logger *_logger;
  int _handle;
  std::string _filename;
  FileBuffer *_currentBuffer;
  FileBuffer *_previousBuffer;
  ///
  /// true: the last fread() has returned 0
  bool _eofReached;
  bool _hasBinaryData;
  ///
  /// 0: If the buffer is too small the size will be doubled. Otherwise: the buffer is increased by that amount of bytes.
  size_t _clusterSize;
public:
  /**
   * Constructor.
   * @param logger The logger.
   * @param startBufferSize The buffer size starts with that value.
   * @param clusterSize If the buffer is too small for a full line, that amount of bytes will be increased.
   *  If 0 the current buffer size will be doubled.
   */
  LineAgent(Logger *logger, size_t startBufferSize = 0x10000,
      size_t clusterSize = 0x10000);
  virtual ~LineAgent();
public:
  /**
   * Returns the line number of the current line: 1 is the first line of the file.
   */
  inline size_t currentLineNo() const {
    return _currentBuffer->_lineNo;
  }
  /**
   * Tests whether the end of file has been reached: No more data available.
   */
  bool eof() const {
    return _eofReached && _currentBuffer->_restLength == 0;
  }
  /**
   * Estimates the count of lines of the file.
   *
   * Count the newlines in the buffer and make a linear approximation with the file size.
   * @return <em>-1</em>Error. Otherwise: The estimation of the count of lines in the file.
   */
  int estimateLineCount();
  /**
   * Returns whether binary data has been read.
   */
  inline bool hasBinaryData() const {
    return _hasBinaryData;
  }
  /**
   * Returns the no-th predesessor line of the current line.
   * @param no The number of the predesessor: 1: the direct predesessor ...
   * @param[out] size The length of the returned line.
   * @return <em>nullptr</em>: the line cannot returned: no more in the buffer. Otherwise: the requested line.
   */
  char* previousLine(int no, size_t &size);
  /**
   * Returns the next line.
   * Reads some data into the <em>_buffer</em> if needed.
   * @param[out] length The line length.
   * @return <em>nullptr</em>: no more data.
   *   Otherwise: the line without ending '\n'.
   */
  const char* nextLine(size_t &length);
  /**
   * Returns the file position of the current line;
   */
  size_t offsetCurrentLine() const;
  /**
   * Opens a file.
   * @param filename The name of the file
   * @param ignoreError <em>true</em>: an error will be not logged.
   * @return true: success
   */
  bool openFile(const char *filename, bool testBinary = true, bool ignoreError =
      false);
  void reset();
  /**
   * Sets the buffer size. This is the maximal possible line length.
   * @param bufferSize the new buffersize.
   */
  void setBufferSize(size_t bufferSize);
protected:
  void fillBuffer(size_t startOffset);
}
;
} /* namespace */
#endif /* TEXT_LINEAGENT_HPP_ */
