/*
 * FileTool.cpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"
#include <unistd.h>
namespace cppknife {
std::string buildFileTree(const char *data) {
  auto lines = splitCString(data, "\n");
  PathInfo info;
  std::string rc;
  BaseRandom pseudoRandom;
  CharRandom charRandom(pseudoRandom);

  for (auto line : lines) {
    if (line.size() < 1) {
      continue;
    }
    auto parts = splitCString(line.c_str(), ";");
    auto filename = parts[0];
    splitPath(filename.c_str(), info);
    ensureDirectory(info._path.c_str());
    if (rc.empty()) {
      rc = info._path;
    }
    size_t size = parts.size() > 1 ? atoi(parts[1].c_str()) : 20;
    auto text = charRandom.randomText(size, size / 20, CC_ASCII95);
    writeText(filename.c_str(), text.c_str(), text.size());
  }
  return rc;
}
bool copyFile(const char *source, const char *target,
    std::string *errorMessage) {
  bool rc = false;
  std::string target2;
  std::string dummy;
  if (errorMessage == nullptr) {
    errorMessage = &dummy;
  }
  bool exists = false;
  if (isDirectory(source, &exists)) {
    *errorMessage = formatCString("cannot copy a directory: %s", source);
  } else if (!exists) {
    *errorMessage = formatCString("missing file: %s", source);
  } else {
    if (isDirectory(target)) {
      target2 = joinPath(target, basename(source).c_str(), nullptr);
      target = const_cast<const char*>(target2.c_str());
    }
    FILE *fpSource = fopen(source, "rb");
    if (fpSource == nullptr) {
      *errorMessage = formatCString("cannot open (%d): %s [%s]", errno, source,
          std::strerror(errno));
    } else {
      FILE *fpTarget = fopen(target, "wb");
      if (fpTarget == nullptr) {
        *errorMessage = formatCString("cannot open (%d): %s [%s]", errno,
            source, std::strerror(errno));
      } else {
        char buffer[64000];
        ssize_t readBytes = 0;
        ssize_t writtenBytes = 0;
        rc = true;
        while ((readBytes = fread(buffer, 1, sizeof buffer, fpSource)) > 0) {
          if ((writtenBytes = fwrite(buffer, 1, readBytes, fpTarget))
              != readBytes) {
            *errorMessage = formatCString("cannot write (%d): %s [%s]", errno,
                target, std::strerror(errno));
            rc = false;
            break;
          }
        }
        fclose(fpTarget);
        if (!rc) {
          unlink(target);
        } else {
          // Set the file time:
          std::filesystem::file_time_type modificationTime =
              std::filesystem::last_write_time(source);
          std::filesystem::last_write_time(target, modificationTime);
        }
      }
      fclose(fpSource);
    }
  }
  return rc;
}

std::string currentDirectory() {
  char path[8192];
  getcwd(path, sizeof path);
  return std::string(path);
}
bool ensureDirectory(const char *path) {
  bool rc = true;
  bool exists = false;
  if (!isDirectory(path, &exists)) {
    if (exists) {
      throw OsException(
          formatCString("cannot create directory %s: a file already exists",
              path));
    } else {
      rc = makeDirectory(path, true) >= 0;
    }
  }
  return rc;
}

bool fileExists(const char *path) {
  struct stat info;
  int rc = stat(path, &info);
  return rc == 0;
}

std::string filetimeToString(const FileTime_t &time) {
  time_t time1 = filetimeToTime(time);
  struct tm *time2 = localtime(&time1);
  std::string rc;
  char buffer[4 + 2 * 2 + 2 * 2 + 1 + 3 * 2 + 2 * 1 + 1];
  strftime(buffer, sizeof buffer, "%Y.%m.%d %H:%M:%S", time2);
  rc = buffer;
  return rc;
}

time_t filetimeToTime(const FileTime_t &filetime) {
#ifdef __linux__
  return filetime.tv_sec;
#elif defined __WIN32__
  	// 64-bit arithmetic:
  	LARGE_INTEGER date, adjust;
  	date.HighPart = filetime->dwHighDateTime;
  	date.LowPart = filetime->dwLowDateTime;
  	// 100-nanoseconds = milliseconds * 10000
  	adjust.QuadPart = 11644473600000 * 10000;
  	// removes the diff between 1970 and 1601
  	date.QuadPart -= adjust.QuadPart;
  	// converts back from 100-nanoseconds to seconds
  	time_t rc = (time_t) (date.QuadPart / 10000000);
  #if defined __WIN32__
  	static int s_diffTime = 0x7fffffff;
  	if (s_diffTime == 0x7fffffff) {
  		s_diffTime = 0;
  		ReByteArray tempFile = ReFileUtils::tempFile("$$redir$$.tmp", NULL);
  		const char* filename = tempFile.str();
  		FILE* fp = fopen(filename, "w");
  		if (fp != NULL) {
  			struct stat info;
  			int rcStat = stat(filename, &info);
  			fclose(fp);
  			if (rcStat == 0) {
  				WIN32_FIND_DATAA data;
  				HANDLE handle = FindFirstFile(filename, &data);
  				if (handle != INVALID_HANDLE_VALUE) {
  					time_t other = filetimeToTime(&data.ftLastWriteTime);
  					s_diffTime = info.st_mtime - other;
  					FindClose(handle);
  				}
  			}
  		}
  	}
  	rc += s_diffTime;
  #endif
  	return rc;
  #endif
}
bool isBinary(const char *filename, size_t checkSize) {
  bool rc = true;
  FILE *fp = fopen(filename, "rb");
  if (fp != nullptr) {
    unsigned char quickBuffer[4096];
    unsigned char *buffer =
        checkSize <= sizeof quickBuffer ?
            quickBuffer : new unsigned char[checkSize];
    auto bytes = fread(buffer, 1, checkSize, fp);
    auto ptr = buffer;
    rc = false;
    int minSpecials = 5;
    while (bytes-- > 0) {
      unsigned char cc = *ptr++;
      if (cc == 0) {
        rc = true;
        break;
      }
      if (cc < 32 && cc != '\n' && cc != '\r' && cc != '\t' && cc != '\v') {
        if (--minSpecials <= 0) {
          rc = true;
          break;
        }
      }
    }
    if (checkSize > sizeof quickBuffer) {
      delete buffer;
    }
    fclose(fp);
  }
  return rc;
}
bool isDirectory(const char *path, bool *exists) {
  bool rc = false;
  std::string path2;
  struct stat info;
  int rc2 = stat(path, &info);
  if (exists != nullptr) {
    *exists = rc2 == 0;
  }
  if (rc2 == 0) {
    rc = S_ISDIR(info.st_mode);
  }
  return rc;
}
bool isSymbolicLink(const char *path, bool *exists) {
  bool rc = false;
  std::string path2;
  struct stat info;
  int rc2 = lstat(path, &info);
  if (exists != nullptr) {
    *exists = rc2 == 0;
  }
  if (rc2 == 0) {
    rc = S_ISLNK(info.st_mode);
  }
  return rc;
}
std::vector<std::string> listFiles(const char *directory, const char *pattern,
    bool ignoreCase, bool withPath) {
  std::vector<std::string> rc;
  DIR *dir;
  struct dirent *entry;
  rc.reserve(100);
  std::string path;
  if (withPath && directory[0] != '\0' && strcmp(directory, ".") != 0) {
    appendString(path, directory, osInfo()._separatorCString);
  }
  std::regex regExpr(pattern == nullptr ? ".*" : pattern,
      ignoreCase ?
          (std::regex_constants::ECMAScript | std::regex_constants::icase) :
          std::regex_constants::ECMAScript);
  if ((dir = opendir(directory)) != nullptr) {
    while ((entry = readdir(dir)) != nullptr) {
      const char *node = entry->d_name;
      if (strcmp(node, ".") == 0 || strcmp(node, "..") == 0) {
        continue;
      }
      if (std::regex_search(node, regExpr)) {
        if (!withPath || path.empty()) {
          rc.push_back(entry->d_name);
        } else {
          rc.push_back(path + entry->d_name);
        }
      }
    }
    closedir(dir);
  }
  return rc;
}

int makeDirectory(const char *path, bool recursive) {
  auto info = osInfo();
  int rc = -1;
  bool exists = false;
  if (isDirectory(path, &exists)) {
    rc = 0;
  } else if (exists) {
    throw OsException(
        formatCString("cannot create directory %s: a file already exists.",
            path));
  } else {
    if (mkdir(path, 0777) != 0) {
      auto parts = splitCString(path, info._separatorCString);
      size_t ix = 0;
      std::string current;
      current.reserve(strlen(path));
      current = parts[0];
      rc = 0;
      if (current.empty() && parts.size() > 1) {
        // may be root (/):
        current += info._separatorString + parts[1];
        ix = 2;
      }
      do {
        if (!isDirectory(current.c_str(), &exists)) {
          if (exists) {
            throw OsException(
                formatCString(
                    "cannot create directory %s: a file already exists.",
                    current.c_str()));
          }
          if (mkdir(current.c_str(), 0777) != 0) {
            throw OsException(
                formatCString("cannot create directory %s.", current.c_str()));
          }
          rc += 1;
        }
        current += info._separatorString;
        current += parts[ix++];
      } while (ix < parts.size());
    }
  }
  return rc;
}
std::vector<std::string> readAsList(const char *filename, Logger *logger) {
  auto contents = readAsString(filename, logger);
  auto rc = splitCString(contents.c_str(), "\n");
  return rc;
}
std::string readAsString(const char *filename, Logger *logger,
    size_t bufferSize) {
  std::string rc;
  FILE *fp = fopen(filename, "r");
  if (fp == nullptr) {
    auto msg = formatCString("cannot read from %s: %d", filename, errno);
    if (logger != nullptr) {
      logger->error(msg.c_str());
    } else {
      fprintf(stderr, "+++ %s\n", msg.c_str());
    }
  } else {
    char staticBuffer[0x10000];
    char *buffer =
        bufferSize > sizeof staticBuffer ? new char[bufferSize] : staticBuffer;
    size_t bytes = 0;
    while ((bytes = fread(buffer, 1, bufferSize, fp)) > 0) {
      rc += std::string(buffer, bytes);
    }
    fclose(fp);
    if (buffer != staticBuffer) {
      delete buffer;
    }
  }
  return rc;
}
std::string uniqueFilename(const char *filename, int width) {
  std::string rc;
  if (!fileExists(filename)) {
    rc = filename;
  } else {
    PathInfo info;
    splitPath(filename, info);
    auto extension = info._extension;
    auto ptr = ::strrchr(info._name.c_str(), '.');
    std::string prefix = info._path;
    int startValue = 0;
    if (ptr == nullptr || ::strspn(ptr + 1, "0123456789") != strlen(ptr + 1)) {
      prefix += info._name;
    } else {
      ptr++;
      startValue = atol(ptr + 1);
      auto length = ptr - info._name.c_str() - 1;
      std::string suffix(info._name.c_str(), length);
      prefix += suffix;
    }
    auto format = formatCString("%%s.%%0%dd%%s", width);
    do {
      rc = formatCString(format.c_str(), prefix.c_str(), ++startValue,
          extension.c_str());
    } while (fileExists(rc.c_str()));
  }
  return rc;
}
bool writeText(const char *filename, const char *text, int textLength,
    Logger *logger) {
  FILE *file = fopen(filename, "w");
  bool rc = false;
  if (file == nullptr) {
    auto msg = formatCString("cannot write to %s: %d", filename, errno);
    if (logger != nullptr) {
      logger->error(msg.c_str());
    } else {
      fprintf(stderr, "+++ %s\n", msg.c_str());
    }
  } else {
    if (textLength < 0) {
      textLength = strlen(text);
    }
    fwrite(text, textLength, 1, file);
    fclose(file);
    rc = true;
  }
  return rc;
}
bool writeBinary(const char *filename, const void *data, size_t dataSize) {
  FILE *file = fopen(filename, "wb");
  bool rc = false;
  if (file == nullptr) {
    throw OsException(formatCString("cannot write to %s: %d", filename, errno));
  } else {
    fwrite(data, dataSize, 1, file);
    fclose(file);
    rc = true;
  }
  return rc;
}
} /* cppknife */
