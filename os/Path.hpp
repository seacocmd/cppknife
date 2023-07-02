/*
 * Path.hpp
 *
 *  Created on: 16.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_PATH_HPP_
#define OS_PATH_HPP_

/// Offers functions for a portable way to handle files system paths.
namespace cppknife {
enum OsType {
  OS_UNDEF, LINUX, WINDOWS, ANDROID
};
/**
 * @brief Stores the specific data of an operating system like path separator.
 */
struct OsInfo {
  OsType _osType;
  ///
  /// The file path separator as string.
  const char *_separatorCString;
  ///
  /// The file path separator as string.
  std::string _separatorString;
  ///
  /// The file path separator as char.
  char _separatorChar;
  ///
  /// The temporary directory (without trailing separator).
  std::string _tempDirectory;
  ///
  /// The temporary directory with trailing separator.
  std::string _tempDirectorySeparator;
};
/**
 * @brief Stores the parts of a full file path.
 */
struct PathInfo {
  std::string _path;
  std::string _name;
  std::string _extension;
  /**
   * Clears the path parts.
   */
  inline void clear() {
    _path.clear();
    _name.clear();
    _extension.clear();
  }
  std::string
  join() const;
};

/**
 * Returns the path of the parent of a given path.
 * @param path The full name of a file, e.g. "/abc/def.txt"
 * @return The <em>filename</em> without the path, e.g. "def.txt"
 */
std::string basename(const char *path);
/**
 * Returns the path of the parent of a given path.
 * @param path The full name of a file, e.g. "/abc/def.txt"
 * @param endingSlash <em>true</em>: the result contains a path separator if given.
 * @return The <em>path</em> without the node, e.g. "/abc/"
 */
std::string dirname(const char *path, bool endingSlash = false);
/**
 * Join some path parts to a full path.
 * @param path The path with or without trailing separator
 * @param name The filename without extension
 * @param extension The filename extension with or without preceding '.'
 * @param subdir1 <em>nullptr</em> or a extension of the path
 */
std::string
joinPath(const char *path, const char *name, const char *extension = nullptr,
    const char *subdir1 = nullptr);
/**
 * Returns the info about the operating system.
 * @return The OS info.
 */
const OsInfo&
osInfo();

/**
 * Replaces the file extension by another. If no extension is given the new extension will be appended.
 * @param filename The filename to inspect.
 * @param extension The new file extension. With or without preceding '.'.
 * @return The filename with exchanged file extension.
 */
std::string replaceExtension(const char *filename, const char *extension);

/**
 * Returns the file path separator as char.
 */
inline char separatorChar() {
  return osInfo()._separatorChar;
}
/**
 * Returns the file path separator as char.
 */
inline const std::string&
separatorString() {
  return osInfo()._separatorString;
}
/**
 * Splits a full path into its parts.
 * @param full The path to split.
 * @param[out] info The result is stored here.
 */
void
splitPath(const char *full, PathInfo &info);

/**
 * Returns the name of a temporary file with a given node and sub directories.
 */
std::string temporaryFile(const char *node,
    const char *subDirectories = nullptr, bool createPath = true);

} /* cppknife */

#endif /* OS_PATH_HPP_ */
