/*
 * Path.cpp
 *
 *  Created on: 16.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife {
static OsInfo globalOsInfo;
static std::string globalSlash("/");

std::string joinPath(const char *path, const char *name, const char *extension,
    const char *subdirectories) {
  const OsInfo &info = osInfo();
  std::string rc;
  rc.reserve(2 + sumOfLength(path, name, extension, subdirectories));
  appendString(rc, path);
  if (rc.size() > 0 && rc.back() != info._separatorChar) {
    rc += info._separatorString;
  }
  if (subdirectories != nullptr) {
    if (subdirectories[0] == info._separatorChar || subdirectories[0] == '/') {
      subdirectories++;
    }
    std::string subdirs(subdirectories);
    subdirs.replace(subdirs.cbegin(), subdirs.cend(), '/', info._separatorChar);
    rc += subdirectories;
    if (rc.size() > 0 && rc.back() != info._separatorChar) {
      rc += osInfo()._separatorString;
    }
  }
  appendString(rc, name);
  if (extension != nullptr) {
    if (extension[0] != '.') {
      appendString(rc, ".", extension);
    } else {
      appendString(rc, extension);
    }
  }
  return rc;
}

std::string replaceExtension(const char *filename, const char *extension) {
  std::string rc = filename;
  bool extensionHasDot = extension != nullptr && extension[0] == '.';
  auto ptrDot = strrchr(filename, '.');
  auto ptrSlash = strrchr(filename, osInfo()._separatorChar);
  if (extension == nullptr) {
    // Remove the extension:
    if (ptrDot != nullptr
        && ((ptrSlash == nullptr && ptrDot > filename + 1)
            || (ptrSlash != nullptr && ptrDot > ptrSlash + 1))) {
      rc.resize(ptrDot - filename);
    }
  } else {
    // Is there an extension?
    if (ptrDot != nullptr
        && ((ptrSlash == nullptr && ptrDot > filename + 1)
            || (ptrSlash != nullptr && ptrDot > ptrSlash + 1))) {
      // remove old extension:
      rc.resize(ptrDot - filename);
    }
    if (extensionHasDot) {
      rc += extension;
    } else {
      rc += '.';
      rc += extension;
    }
  }
  return rc;
}
void splitPath(const char *full, PathInfo &info) {
  info.clear();
  if (full != nullptr) {
    const char *endOfPath = strrchr(full, osInfo()._separatorChar);
    if (endOfPath == nullptr) {
      info._name = full;
      size_t ixDot = info._name.rfind('.');
      if (ixDot != 0 && ixDot != static_cast<size_t>(-1)) {
        info._extension = info._name.substr(ixDot);
        info._name.resize(ixDot);
      }
    } else {
      info._path = full;
      size_t pathLength = endOfPath - full + 1;
      info._name = info._path.substr(pathLength);
      info._path.resize(pathLength);
      size_t ixDot = info._name.rfind('.');
      if (ixDot != 0 && ixDot != static_cast<size_t>(-1)) {
        info._extension = info._name.substr(ixDot);
        info._name.resize(ixDot);
      }

    }
  }
}

const OsInfo&
osInfo() {
  if (globalOsInfo._separatorChar == '\0') {
#if defined(_WIN32) || defined (_WINDOWS)
	  globalOsInfo._osType = WINDOWS;
	  globalOsInfo._separatorCString = "\\";
	  globalOsInfo._tempDirectory = "c:\\temp";
#endif
#ifdef __linux__
    globalOsInfo._osType = LINUX;
    globalOsInfo._separatorCString = "/";
    globalOsInfo._tempDirectory = "/tmp";
#endif
    globalOsInfo._separatorChar = globalOsInfo._separatorCString[0];
    globalOsInfo._separatorString = globalOsInfo._separatorCString;
    globalOsInfo._tempDirectorySeparator = globalOsInfo._tempDirectory
        + globalOsInfo._separatorString;
  }
  return globalOsInfo;
}

std::string basename(const char *path) {
  PathInfo info;
  splitPath(path, info);
  return info._name + info._extension;
}

std::string dirname(const char *path, bool endingSlash) {
  PathInfo info;
  splitPath(path, info);
  if (!endingSlash) {
    if (info._path.back() == osInfo()._separatorChar) {
      info._path.pop_back();
    }
  }
  return info._path;
}
std::string PathInfo::join() const {
  return _path + _name + _extension;
}

std::string temporaryFile(const char *node, const char *subDirectories,
    bool createPath) {
  std::string rc = joinPath(osInfo()._tempDirectorySeparator.c_str(), node,
      nullptr, subDirectories);
  if (createPath) {
    ensureDirectory(dirname(rc.c_str()).c_str());
  }
  return rc;
}

} /* cppknife */
