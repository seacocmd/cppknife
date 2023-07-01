/*
 * FileTool.hpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_FILETOOL_HPP_
#define OS_FILETOOL_HPP_

namespace cppknife {
typedef int64_t FileSize_t;
typedef struct timespec FileTime_t;
typedef struct timespec FileTime_t;
const FileSize_t UNDEF_SIZE = (FileSize_t) -1;
const time_t UNDEF_TIME = (time_t) 0;
/**
 * Builds a directory with subdirectories and files.
 * @param data Description of the files to create:one file per line<br>
 * Format of one line:<br>
 * full_name;size;datetime<br>
 * size and datetime are optional.
 */
std::string
buildFileTree(const char *data);
/**
 * Copies a single file.
 * @param source The name of the file to copy.
 * @param target The name of the new file.
 * @param[out] errorMessage <em>nullptr</em> or a buffer for an error message.
 * @return <em>true</em>: success
 */
bool copyFile(const char *source, const char *target,
    std::string *errorMessage = nullptr);
/**
 * Returns the current directory.
 */
std::string
currentDirectory();
/**
 * Tests whether the directory exist. If not it will be created.
 * @param path The directory name.
 * @return true: success.
 *
 */
bool
ensureDirectory(const char *path);
/**
 * Tests whether a file given by its name exists.
 * @param path The filename with path.
 * @return true: the directory has been created.
 */
bool
fileExists(const char *path);
/**
 * Compares two file time entries.
 * @param time1 The first time to compare.
 * @param time2 The second time to compare.
 * @return 0: the times are equal<br>
 *   &lt; 0: time1 &lt; time2
 *   &gt; 0: time1 &gt; time2
 */
inline int fileTimeCompare(const FileTime_t &time1, const FileTime_t &time2) {
  time_t rc2 = time1.tv_sec - time2.tv_sec;
  if (rc2 == 0) {
    rc2 = time1.tv_nsec - time2.tv_nsec;
  }
  int rc = rc2 == 0 ? 0 : rc2 < 0 ? -1 : 1;
  return rc;
}
/**
 * Tests whether a file time has the status "undefined".
 * @param time The time to test.
 * @return true: the file time has the status "undefined".
 */
inline bool filetimeIsUndefined(FileTime_t &time) {
  return time.tv_sec == UNDEF_TIME;
}
/**
 * Converts a file time to a string.
 *
 * @param time The file time to convert.
 * @return The file time, e.g. "2014.01.07 02:59:43".
 */
std::string
filetimeToString(const FileTime_t &time);
/**
 * Converts a filetime to a unix time (seconds since the Epoche).
 *
 * @param filetime The filetime to convert.
 * @return The count of seconds since 1.1.1970.
 */
time_t
filetimeToTime(const FileTime_t &filetime);
/**
 * Tests whether a file is a binary file.
 * Criterium: a '\0' or at least 5 chars lower than ' ' and not whitespaces.
 * @param filename The name of the file to test.
 * @param checkSize The count of bytes to test.
 * @return <em>true</em>: the file is a binary file.
 */
bool isBinary(const char *filename, size_t checkSize = 4096);
/**
 * Tests whether a given path points to a directory.
 * @param path The directory name.
 * @param[out] exists @post: true a file with this name exists. May be <em>nullptr</em>.
 */
bool
isDirectory(const char *path, bool *exists = nullptr);
/**
 * Tests whether a given path points to a symbolic link.
 * @param path The filename.
 * @param[out] exists @post: true a file with this name exists. May be <em>nullptr</em>.
 */
bool isSymbolicLink(const char *path, bool *exists = nullptr);
/**
 * Returns a list of files from a directory given by a regular expression.
 * @param directory The directory to inspect.
 * @param pattern A regular expression defining the result files. If <em>nullptr</em>: return all files.
 * @param ignoreCase <em>true</em>: the case is ignored while searching.
 * @param withPath <em>false</em>: the node of the found files is returned. <em>true</em> the full name is returned.
 * @return The files from <em>directory</em> matching the <em>pattern</em>, as nodes or as full names.
 */
std::vector<std::string> listFiles(const char *directory, const char *pattern,
    bool ignoreCase = false, bool withPath = false);
/**
 * Creates a directory.
 * @param path The directory name.
 * @param recursive true: the parent directories will be created if needed.
 * @return &lt; 0: error occurred. Otherwise: the count of needed creations.
 */
int
makeDirectory(const char *path, bool recursive = false);
/**
 * Sets a file time to a value meaning "undefined"
 * @param[in out] time The time to change.
 * @param seconds The seconds since epoch.
 * @param nanoseconds The part of one second.
 */
inline void setFiletime(FileTime_t &time, time_t seconds,
    long nanoseconds = 0) {
  time.tv_sec = seconds;
  time.tv_nsec = nanoseconds;
}
inline void setFiletimeUndef(FileTime_t &time) {
  time.tv_sec = UNDEF_TIME;
  time.tv_nsec = 0;
}
/**
 * Reads a text file and return a vector of lines.
 * @param filename The name of the file to read.
 * @param logger <em>nullptr</em> or the logger.
 * @return A list (vector) of lines.
 */
std::vector<std::string> readAsList(const char *filename, Logger *logger =
    nullptr);
/**
 * Reads a text file and return one string.
 * @param filename The name of the file to read.
 * @param logger <em>nullptr</em> or the logger.
 * @param bufferSize The maximum line length.
 * @return The file contents as string.
 */
std::string readAsString(const char *filename, Logger *logger = nullptr,
    size_t bufferSize = 0x10000);
/**
 * Builds a unique filename.
 * Example: "/tmp/data.txt" becomes "/tmp/data.003.txt" if "/tmp/data.001.txt" and "/tmp/data.001.txt" already exists.
 * @param filename The filename to inspect.
 * @param width The minimal width of the part containing the current number.
 */
std::string uniqueFilename(const char *filename, int width = 2);
/**
 * Writes a text into a file.
 * @param filename The name of the file to write.
 * @param text The data to write.
 * @param textLength &lt; 0: <em>strlen</em> is used.
 *   Otherwise: the length of <em>text</em>.
 *   @param logger <em>nullptr</em> or the logger.
 * @return <em>true</em>: Success.
 */
bool writeText(const char *filename, const char *text, int textLength = -1,
    Logger *logger = nullptr);

/**
 * Writes any (binary) data into a file.
 * @param filename The name of the file to write.
 * @param data The data to write.
 * @param dataSize The length of <em>data</em>.
 * @return <em>true</em>: Success.
 */
bool writeBinary(const char *filename, const void *data, size_t dataSize);
} /* cppknife */

#endif /* OS_FILETOOL_HPP_ */
