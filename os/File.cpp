/*
 * File.cpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "os.hpp"

namespace cppknife {

FileSystemEntity::FileSystemEntity(FileType fileType, const char *fullName,
    const struct stat *statInfo) :
    _fileType(fileType), _statInfo(), _fullName(fullName), _node(), _path() {
  _statInfo.st_mtim.tv_sec = 0;
  auto ix = _fullName.rfind(osInfo()._separatorChar);
  if (ix > 0) {
    _node = _fullName.substr(ix + 1);
    _path = _fullName.substr(0, ix);
  }
}

FileSystemEntity::FileSystemEntity(FileType fileType, const char *node,
    const char *path, const struct stat *statInfo) :
    _fileType(fileType), _statInfo(), _fullName(joinPath(path, node)), _node(
        node), _path(path) {
  clearStatus();
}

FileSystemEntity::~FileSystemEntity() {
}

FileSystemEntity::FileSystemEntity(const FileSystemEntity &other) :
    _fileType(other._fileType), _statInfo(other._statInfo), _fullName(
        other._fullName), _node(other._node), _path(other._path) {
}

FileSystemEntity&
FileSystemEntity::operator =(const FileSystemEntity &other) {
  _fileType = other._fileType;
  _statInfo = other._statInfo;
  _fullName = other._fullName;
  _node = other._node;
  _path = other._path;
  return *this;
}

bool FileSystemEntity::exists(bool force) {
  if (force || !statIsCalled()) {
    if (stat(fullName().c_str(), &_statInfo) != 0) {
      clearStatus();
    }
  }
  return _statInfo.st_mtim.tv_sec;
}

File::File(const char *fullName, const struct stat *statInfo) :
    FileSystemEntity(FT_FILE, fullName, statInfo) {
}

File::File(const char *node, const char *path, const struct stat *statInfo) :
    FileSystemEntity(FT_FILE, node, path, statInfo) {
}

File::~File() {
  // TODO Auto-generated destructor stub
}

File::File(const File &other) :
    FileSystemEntity(other) {
}

File&
File::operator=(const File &other) {
  FileSystemEntity::operator =(other);
  return *this;
}

time_t FileSystemEntity::created() {
  time_t rc = 0;
  if (exists()) {
    rc = _statInfo.st_ctim.tv_sec;
  }
  return rc;
}

time_t FileSystemEntity::modified() {
  time_t rc = 0;
  if (exists()) {
    rc = _statInfo.st_mtim.tv_sec;
  }
  return rc;
}
FileSize_t File::size() {
  auto rc = !exists() ? UNDEF_SIZE : _statInfo.st_size;
  return rc;
}

Directory::Directory(const char *fullName, const struct stat *statInfo) :
    FileSystemEntity(FT_DIRECTORY, fullName, statInfo) {
}

Directory::~Directory() {
}

Directory::Directory(const Directory &other) :
    FileSystemEntity(other) {
}

Directory&
Directory::operator =(const Directory &other) {
  FileSystemEntity::operator =(other);
  return *this;
}

SymbolicLink::SymbolicLink(const char *fullName, const struct stat *statInfo) :
    FileSystemEntity(FT_SYMBOLIC_LINK, fullName, statInfo) {
}

SymbolicLink::SymbolicLink(const char *node, const char *path,
    const struct stat *statInfo) :
    FileSystemEntity(FT_SYMBOLIC_LINK, node, path, statInfo) {
}

SymbolicLink::~SymbolicLink() {
}

SymbolicLink::SymbolicLink(const SymbolicLink &other) :
    FileSystemEntity(other) {
}

SymbolicLink&
SymbolicLink::operator =(const SymbolicLink &other) {
  FileSystemEntity::operator =(other);
  return *this;
}

} /* cppknife */
