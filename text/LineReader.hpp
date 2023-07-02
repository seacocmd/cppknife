/*
 * LineReader.h
 *
 *  Created on: 10.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TEXT_LINEREADER_HPP_
#define TEXT_LINEREADER_HPP_
// Implements a line reader with unlimited line size.
namespace cppknife {

/// Manages a line reader with unlimited line size.
/**
 * Manages a line reader with unlimited line size.
 * Note: fgets() operates with a limited line size.
 */
class LineReader {
protected:
  std::string _filename;
  Logger &_logger;
  int _fileHandle;
  bool _removeNewline;
  std::vector<std::string> _lookAheadLines;
  /// <em>true</em>: we are in a "read ahead lines" loop.
  bool _ignoreLookahead;
  /// The last result of nextLine(): valid until the next call of nextLine()
  std::string _currentLine;
  size_t _blockSize;
  /// A data block containing the start of the next data.
  std::string _nextBlock;
  /// Points on the start of the next line inside <em>_nextBlock</em>.
  const char *_cursorNextBlock;
  /// <em>true</em>: the block contains binary data: '\0' ...
  bool _hasBinaryData;
public:
  int _countBlockReads;
public:
  LineReader(const char *filename, Logger &logger, bool removeNewline = true,
      size_t bufferSize = 0x10000);
  virtual ~LineReader();
public:
  /**
   * Returns whether the end of file is reached.
   * @return <em>true</em>: no more data available.
   */
  bool endOfFile();
  /**
   * Estimates the count of lines in the file.
   * Counts the lines in the first block and interpolates that with the file size.
   * @param factor A factor to increase the line number.
   * @return the estimation of the line count
   */
  int estimateLineCount(double factor = 1.05);
  /**
   * Tests whether the <em>_nextBlock</em> contains binary data: '\0' ...
   * @param localOnly: <em>false</em>: the previous test should be respected.
   * @return <em>true</em>: there are binary data.
   */
  bool hasBinaryData(bool localOnly = true);
  /**
   * Reads up to <em>countLines</em> lines into the instance (from the current point in the file).
   * That lines will be returned line by line by the next calls of <em>nextLine()</em>.
   */
  const std::vector<std::string> lookahead(int countLines);
  /**
   * Reads the next line into <em>_currentLine</em>.
   * @return <em>true</em>: Data was available.
   */
  const std::string& nextLine();
  /**
   * Initializes the instance with a given file.
   * @param filename The name of the file to process.
   * @return <em>true</em>: data was available
   */
  bool openFile(const char *filename);
  /**
   * Puts the next line into <em>_currentLine</em>.
   */
  bool readLine();
  /**
   * Reads the next block into <em>_nextBlock</em>.
   * @return <em>true</em>: data available.
   */
  bool readBlock();
  /**
   * Sets the flag whether the newline should be removed from returned lines.
   */
  inline void setBufferSize(size_t value) {
    _blockSize = value == 0 ? 0x10000 : value;
    _nextBlock.reserve(_blockSize);
  }
  /**
   * Sets the flag whether the newline should be removed from returned lines.
   */
  inline void setRemoveNewline(bool value) {
    _removeNewline = value;
  }
};

} /* namespace cppknife */

#endif /* TEXT_LINEREADER_HPP_ */
