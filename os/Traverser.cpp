/*
 * Traverser.cpp
 *
 *  Created on: 22.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#if defined __WIN32__
#include "accctrl.h"
#include "aclapi.h"
#pragma comment(lib, "advapi32.lib")
#endif
#include "os.hpp"

namespace cppknife {
FsEntry::~FsEntry() {
}
/**
 * Returns the filetype as a single character.
 *
 * @return	the filetype, e.g. 'd' for a directory
 */
char FsEntry::typeAsChar() {
  char rc = ' ';
  switch (type()) {
  case TF_REGULAR:
    rc = ' ';
    break;
  case TF_LINK:
    rc = 'l';
    break;
  case TF_SUBDIR:
    rc = 'd';
    break;
  case TF_CHAR:
    rc = 'c';
    break;
  case TF_BLOCK:
    rc = 'b';
    break;
  case TF_PIPE:
    rc = 'p';
    break;
  case TF_SOCKET:
    rc = 's';
    break;
  default:
    rc = 'o';
    break;
  }
  return rc;
}
FsEntryLinux::FsEntryLinux() :
    _singleFileFts(nullptr), _intrinsicStatInfo(false), _state(nullptr), _linkReference(), _type(
        FsEntry::TF_UNDEF), _statInfo(nullptr), _statBuffer() {
}
FsEntryLinux::~FsEntryLinux() {
  if (_singleFileFts != nullptr) {
    fts_close(_singleFileFts);
    _singleFileFts = nullptr;
  }
}
const FileTime_t*
FsEntryLinux::accessed() {
  const FileTime_t *rc = &getStatus()->st_atim;
  return rc;
}

const char*
FsEntryLinux::accessFullName() {
  return _state->fts_accpath;
}

FileSize_t FsEntryLinux::fileSize() {
  return getStatus()->st_size;

}

std::string FsEntryLinux::filetimeAsString() {
  return filetimeToString(getStatus()->st_mtim);
}
void FsEntryLinux::finishCertainFile() {
  if (_singleFileFts != nullptr) {
    fts_close(_singleFileFts);
    _singleFileFts = nullptr;
  }
}
const char*
FsEntryLinux::fullName() {
  return _state->fts_path;
}

size_t FsEntryLinux::fullNameLength() {
  return _state->fts_pathlen;
}

struct stat* FsEntryLinux::getStatus() {
  if (_statInfo == nullptr) {
    lstat(_state->fts_accpath, &_statBuffer);
    _statInfo = &_statBuffer;
  }
  return _statInfo;
}
bool FsEntryLinux::isDirectory() {
  bool rc = S_ISDIR(getStatus()->st_mode);
  return rc;
}

bool FsEntryLinux::isDotDir() {
  bool rc = _state->fts_info == FTS_DOT;
  return rc;
}

bool FsEntryLinux::isLink() {
  auto info = _state->fts_info;
  bool rc = info == FTS_SL || info == FTS_SLNONE;
  return rc;
}

bool FsEntryLinux::isRegular() {
  bool rc = S_ISREG(getStatus()->st_mode);
  return rc;
}

const FileTime_t*
FsEntryLinux::modified() {
  const FileTime_t *rc = &getStatus()->st_mtim;
  return rc;
}

const std::string&
FsEntryWithStack::linkReference() {
#ifdef __linux__
  if (_linkReference.empty()) {
    char buffer[8192];
    buffer[0] = '\0';
    readlink(fullName(), buffer, sizeof buffer);
    _linkReference = buffer;
  }
#endif
  return _linkReference;
}
bool FsEntryWithStack::setCertainFile(const char *fullName) {
  return false;
}
const char*
FsEntryLinux::node() {
  const char *rc = _state->fts_name;
  return rc;
}

const std::string&
FsEntryLinux::linkReference() {
  if (_linkReference.empty()) {
    char buffer[8192];
    auto len = readlink(_state->fts_accpath, buffer, sizeof buffer - 1);
    if (len >= 0) {
      buffer[len] = '\0';
      _linkReference = buffer;
    }
  }
  return _linkReference;
}

const char*
FsEntryLinux::rightsAsString(std::string &buffer, bool numerical,
    int ownerWidth) {
  const char *rc = "";
  return rc;
}

void FsEntryLinux::set(FTSENT *entry) {
  _state = entry;
  _statInfo = _intrinsicStatInfo ? entry->fts_statp : nullptr;
  _linkReference.clear();
}
bool FsEntryLinux::setCertainFile(const char *fullName) {
  bool rc = false;
  auto base = dirname(fullName);
  const char *node = fullName;
  if (base.empty()) {
    base = ".";
  } else {
    node += base.size() + 1;
  }
  bool intrinsic = true;
  setIntrinsicStatInfo(intrinsic);
  char *argv[] = { const_cast<char*>(base.c_str()), nullptr };

  ;
  auto flags = FTS_COMFOLLOW | FTS_PHYSICAL;
  if (!intrinsic) {
    flags |= FTS_NOSTAT;
  }
  _singleFileFts = fts_open(argv, flags, nullptr);
  if (_singleFileFts != nullptr) {
    FTSENT *current = nullptr;
    while ((current = fts_read(_singleFileFts)) != nullptr) {
      if (strcmp(current->fts_name, node) == 0) {
        set(current);
        rc = true;
        break;
      }
    }
    // fts_close() is done in finishCertainFile()
  }
  return rc;
}
FsEntry::Type_t FsEntryLinux::type() {
  Type_t rc = FsEntry::TF_UNDEF;
  switch (_state->fts_info) {
// Preorder directory: ignore it, it comes again as post order.
  case FTS_D:
  case FTS_DEFAULT:
    // unreadable directory
  case FTS_DNR:
    // dot or dot-dot
  case FTS_DOT:
    // error; errno is set
  case FTS_ERR:
    // initialized only
  case FTS_INIT:
    // stat(2) failed
  case FTS_NS:
    // stat(2) failed
  case FTS_W:
    // whiteout object
  default:
    break;
  case FTS_NSOK:
    // no stat(2) requested
    rc = TF_UNKNOWN;
    break;
  case FTS_DP:
    // Post order:
    rc = TF_SUBDIR;
    break;
  case FTS_SL:
  case FTS_SLNONE:
    rc = TF_LINK;
    break;
  case FTS_F: {
    auto mode = _state->fts_statp->st_mode;
    if (mode == 0 || S_ISREG(mode))
      rc = TF_REGULAR;
    else if (S_ISCHR(mode))
      rc = TF_CHAR;
    else if (S_ISBLK(mode))
      rc = TF_BLOCK;
    else if (S_ISFIFO(mode))
      rc = TF_PIPE;
    else if (S_ISSOCK(mode))
      rc = TF_SOCKET;
    else
      rc = TF_OTHER;
    break;
  }
  }
  return rc;
}

FsEntryWithStack::FsEntryWithStack(Logger *logger) :
    _path(), _fullName(), _passNo(0), _logger(logger), _handle(nullptr), _data(
        nullptr), _status(), _linkReference() {
  memset(&_status, 0, sizeof _status);
}
FsEntryWithStack::~FsEntryWithStack() {
}
const FileTime_t*
FsEntryWithStack::accessed() {
#ifdef __linux__
  return &(getStatus()->st_atim);
#elif defined __WIN32__
  	return &_data.ftLastAccessTime;
  #endif
}

/**
 * Returns the full filename (with path).
 *
 * @return  the filename with path
 */
const char*
FsEntryWithStack::accessFullName() {
  return _fullName.c_str();
}

/**
 * Returns the filesize.
 *
 * @return	the filesize
 */
FileSize_t FsEntryWithStack::fileSize() {
#ifdef __linux__
  return getStatus()->st_size;
#elif defined __WIN32__
  	return ((int64_t) _data.nFileSizeHigh << 32) + _data.nFileSizeLow;
  #endif
}

/**
 * Returns the file time as a string.
 *
 * @param buffer    OUT: the file time
 * @return          <em>buffer.str()</em> (for chaining)
 */
std::string FsEntryWithStack::filetimeAsString() {
  return filetimeToString(*modified());
}

/**
 * Loads the info about the first file into the instance.
 *
 * @return	<em>true</em>: success
 */
bool FsEntryWithStack::findFirst() {
  bool rc = false;
#if defined __linux__
  if (_handle != nullptr)
    closedir(_handle);
  _handle = opendir(_path.c_str());
  rc = _handle != nullptr && (_data = readdir(_handle)) != nullptr;
  _status.st_ino = 0;
#elif defined __WIN32__
  	if (_handle != INVALID_HANDLE_VALUE)
  	FindClose(_handle);
  	std::string thePath(_path);
  	thePath.append(_path.lastChar() == '\\' ? "*" : "\\*");
  	_handle = FindFirstFileA(thePath.str(), &_data);
  	rc = _handle != INVALID_HANDLE_VALUE;
  #endif
  _fullName.clear();
  return rc;
}

/**
 * Loads the info about the next file into the instance.
 *
 * @return	<em>true</em>: success
 */
bool FsEntryWithStack::findNext() {
#if defined __linux__
  bool rc = _handle != nullptr && (_data = readdir(_handle)) != nullptr;
  _status.st_ino = 0;
#elif defined __WIN32__
  	bool rc = _handle != INVALID_HANDLE_VALUE && FindNextFileA(_handle, &_data);
  #endif
  _fullName.clear();
  return rc;
}

void FsEntryWithStack::finishCertainFile() {
}

/**
 * Frees the resources of an instance.
 */
void FsEntryWithStack::freeEntry() {
#if defined __linux__
  if (_handle != nullptr) {
    closedir(_handle);
    _handle = nullptr;
  }
#elif defined __WIN32__
  	if (_handle != INVALID_HANDLE_VALUE) {
  		FindClose(_handle);
  		_handle = INVALID_HANDLE_VALUE;
  	}
  #endif
  _path.clear();
  _fullName.clear();
}

/**
 * Returns the full filename (with path).
 *
 * @return	the filename with path
 */
const char*
FsEntryWithStack::fullName() {
  if (_fullName.empty()) {
    _fullName = _path + node();
  }
  return _fullName.c_str();
}

size_t FsEntryWithStack::fullNameLength() {
  if (_fullName.empty()) {
    _fullName = _path + node();
  }
  return _fullName.length();
}

#if defined __WIN32__
  /** Gets the name of the file owner.
   *
   * @param handle		file handle (see <em>CreateFile()</em>)
   * @param name		OUT: the owner: [domain\\]name
   * @return			<em>true</em>: success
   */
  bool FsEntryWithStack::getFileOwner(HANDLE handle, const char* file,
  	std::string& name, Logger* logger) {
  	bool rc = false;
  	PSID pSidOwner = nullptr;
  	PSECURITY_DESCRIPTOR pSD = nullptr;
  	if (GetSecurityInfo(handle, SE_FILE_OBJECT,
  			OWNER_SECURITY_INFORMATION, &pSidOwner, nullptr, nullptr, nullptr, &pSD) != ERROR_SUCCESS) {
  		if (logger != nullptr)
  		logger->sayF(LOG_ERROR | CAT_FILE, LC_GET_FILE_OWNER_1,
  			"GetSecurityInfo($1): $2").arg(file).arg((int) GetLastError()).end();
  	} else {
  		char accountName[128];
  		char domainName[128];
  		DWORD dwAcctName = sizeof accountName;
  		DWORD dwDomainName = sizeof domainName;
  		SID_NAME_USE eUse = SidTypeUnknown;
  		if (! LookupAccountSid(nullptr, pSidOwner, accountName, &dwAcctName, domainName,
  				&dwDomainName, &eUse)) {
  			if (logger != nullptr)
  			logger->sayF(LOG_ERROR | CAT_SECURITY, LC_GET_FILE_OWNER_2,
  				"LookupAccountSid(): $1").arg((int) GetLastError()).end();
  		} else {
  			if (dwDomainName > 0)
  			name.append(domainName).appendChar('\\');
  			name.append(accountName);
  			rc = true;
  		}
  	}
  	return rc;
  }
  #endif /* __WIN32__ */

#if defined __WIN32__
  /** Tries to get a privilege.
   *
   * @param privilege	the name of the privilege, e.g. "SeBackup"
   * @param logger		logger for error logging
   */
  bool FsEntryWithStack::getPrivilege(const char* privilege, Logger* logger) {
  	bool rc = false;
  	LUID luidPrivilege;
  	HANDLE hAccessToken;
  	if (! OpenProcessToken (GetCurrentProcess(),
  			TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hAccessToken)) {
  		if (logger != nullptr)
  		logger->sayF(LOG_ERROR | CAT_FILE, LC_GET_PRIVILEGE_1,
  			"OpenProcessToken(): $1").arg((int) GetLastError()).end();
  	} else if (! LookupPrivilegeValue (nullptr, SE_BACKUP_NAME, &luidPrivilege)) {
  		if (logger != nullptr)
  		logger->sayF(LOG_ERROR | CAT_FILE, LC_GET_PRIVILEGE_2,
  			"LookupPrivilegeValue(): $1").arg((int) GetLastError()).end();
  	} else {
  		TOKEN_PRIVILEGES tpPrivileges;
  		tpPrivileges.PrivilegeCount = 1;
  		tpPrivileges.Privileges[0].Luid = luidPrivilege;
  		tpPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
  		if (AdjustTokenPrivileges (hAccessToken, FALSE, &tpPrivileges,
  				0, nullptr, nullptr) == 0)
  		rc = true;
  		else {
  			int error = GetLastError();
  			if (error != 1300 && logger != nullptr)
  			logger->sayF(LOG_ERROR | CAT_FILE, LC_GET_PRIVILEGE_3,
  				"AdjustTokenPrivileges(): $1").arg((int) GetLastError()).end();
  		}
  	}
  	return rc;
  }
  #endif /* __WIN32__ */

/**
 * Returns the status of the current file (lazy loading).
 *
 * @return	the status of the current file
 */
struct stat*
FsEntryWithStack::getStatus() {
  if (_status.st_ino == 0) {
    if (stat(fullName(), &_status) != 0)
      _status.st_ino = 0;
  }
  return &_status;
}

/**
 * Tests whether the instance is a directory.
 *
 * @return	<em>true</em>: instance contains the data of a directory
 */
bool FsEntryWithStack::isDirectory() {
#ifdef __linux__
  return _data->d_type == DT_DIR
      || (_data->d_type == DT_UNKNOWN && S_ISDIR(getStatus()->st_mode));
#elif defined __WIN32__
  	return 0 != (_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
  #endif
}

/**
 * Tests whether the instance contains data about "." or "..".
 *
 * @return	<em>true</em>: an ignorable entry has been found
 */
bool FsEntryWithStack::isDotDir() {
#ifdef __linux__
  bool rc = _data == nullptr
      || (_data->d_name[0] == '.'
          && (_data->d_name[1] == '\0'
              || (_data->d_name[1] == '.' && _data->d_name[2] == '\0')));
#elif defined __WIN32__
  	bool rc = _data.cFileName[0] == '.' && (_data.cFileName[1] == '\0'
  		|| (_data.cFileName[1] == '.' && _data.cFileName[2] == '\0'));
  #endif
  return rc;
}

/**
 * Tests whether the instance is a symbolic link.
 *
 * Unter windows it tests whether the the instance is a reparse point.
 *
 * @return	<em>true</em>: instance contains the data of a link
 */
bool FsEntryWithStack::isLink() {
  bool rc;
#ifdef __linux__
  rc = _data->d_type == DT_LNK
      || (_data->d_type == DT_UNKNOWN && S_ISLNK(getStatus()->st_mode));
#elif defined __WIN32__
  	rc = 0 != (_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT);
  #endif
  return rc;
}
/**
 * Tests whether the instance is a "normal" file.
 *
 * @return	<em>true</em>: instance contains the data of a not special file
 */
bool FsEntryWithStack::isRegular() {
#ifdef __linux__
  return _data->d_type == DT_REG
      || (_data->d_type == DT_UNKNOWN && S_ISREG(getStatus()->st_mode));
#elif defined __WIN32__
  	return 0 == (_data.dwFileAttributes & (FILE_ATTRIBUTE_REPARSE_POINT | FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_DEVICE));
  #endif
}
/**
 * Returns the modification time.
 *
 * @return	the modification time
 */
const FileTime_t*
FsEntryWithStack::modified() {
#ifdef __linux__
  return &(getStatus()->st_mtim);
#elif defined __WIN32__
  	return &_data.ftLastWriteTime;
  #endif
}

/**
 * Returns the name of the current file (without path).
 *
 * @return	the name of the current file.
 */
const char*
FsEntryWithStack::node() {
#ifdef __linux__
  return _data->d_name;
#elif defined __WIN32__
  	return _data.cFileName;
  #endif
}

inline void addRight(int mode, std::string &string) {
  const char *right;
  switch (mode & 7) {
  case 1:
    right = "x";
    break;
  case 2:
    right = "w";
    break;
  case 3:
    right = "X";
    break;
  case 4:
    right = "r";
    break;
  case 5:
    right = "R";
    break;
  case 6:
    right = "W";
    break;
  case 7:
    right = "A";
    break;
  default:
    right = "-";
    break;
  }
  appendString(string, right);
}
inline void addId(const char *id, int maxLength, std::string &string) {
  int length = strlen(id);
  if (length == maxLength)
    string.append(id, length);
  else if (length < maxLength) {
    appendString(string, id);
    string.resize(maxLength, ' ');
  } else {
    string.append(id, 2);
    string.append(id + length - maxLength - 2, maxLength - 2);
  }
}
const char*
FsEntryWithStack::rightsAsString(std::string &string, bool numerical,
    int ownerWidth) {
  string.reserve(32);
  string.clear();
#if defined __linux__
  if (numerical) {
    appendInt(string, getStatus()->st_mode & ALLPERMS, "%04o");
    appendInt(string, getStatus()->st_uid, " %4d");
    appendInt(string, getStatus()->st_gid, " %4d");
  } else {
    int mode = getStatus()->st_mode & ALLPERMS;
    addRight(mode >> 6, string);
    addRight(mode >> 3, string);
    addRight(mode, string);
    appendString(string, " ");
    struct passwd *passwd = getpwuid(getStatus()->st_uid);
    if (passwd == nullptr)
      appendInt(string, getStatus()->st_uid, "%4d");
    else
      addId(passwd->pw_name, 5, string);
    appendString(string, " ");
    struct group *group = getgrgid(getStatus()->st_gid);
    if (group == nullptr)
      appendInt(string, getStatus()->st_gid, "%4d");
    else
      addId(group->gr_name, 5, string);
    appendString(string, " ");
  }
#elif defined __WIN32__
  	const char* name = fullName();
  	HANDLE handle = INVALID_HANDLE_VALUE;
  	if (! isDirectory()) {
  		if ( (handle = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, nullptr,
  					OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)) == INVALID_HANDLE_VALUE)
  		_logger->sayF(LOG_ERROR | CAT_FILE, LC_RIGHTS_AS_STRING_1,
  			"CreateFile($1): $2").arg(name).arg((int) GetLastError()).end();
  	} else if (m_getPrivilege) {
  		// we try only one time:
  		m_getPrivilege = false;
  		if (getPrivilege(SE_BACKUP_NAME, _logger)) {
  			if ( (handle = CreateFile(name, 0, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
  						nullptr)) != INVALID_HANDLE_VALUE)
  			_logger->sayF(LOG_ERROR | CAT_FILE, LC_RIGHTS_AS_STRING_2,
  				"CreateFile($1): $2").arg(name).arg((int) GetLastError()).end();
  		}

  	}
  	std::string owner;
  	if (handle != INVALID_HANDLE_VALUE)
  	getFileOwner(handle, name, owner, _logger);
  	CloseHandle(handle);
  	buffer.appendFix(owner.str(), owner.length(), ownerWidth, ownerWidth);
  #endif
  return string.c_str();
}

/**
 * Returns the type of the entry.
 * return       the file type, e.g. TF_REGULAR
 */
FsEntry::Type_t FsEntryWithStack::type() {
  Type_t rc = TF_UNDEF;
#if defined __linux__
  int flags = getStatus()->st_mode;
  if (S_ISDIR(flags))
    rc = TF_SUBDIR;
  else if (flags == 0 || S_ISREG(flags))
    rc = TF_REGULAR;
  else if (S_ISLNK(flags))
    rc = TF_LINK;
  else if (S_ISCHR(flags))
    rc = TF_CHAR;
  else if (S_ISBLK(flags))
    rc = TF_BLOCK;
  else if (S_ISFIFO(flags))
    rc = TF_PIPE;
  else if (S_ISSOCK(flags))
    rc = TF_SOCKET;
  else
    rc = TF_OTHER;
#elif defined __WIN32__
  	int flags = (_data.dwFileAttributes & ~(FILE_ATTRIBUTE_READONLY
  			| FILE_ATTRIBUTE_HIDDEN
  			| FILE_ATTRIBUTE_SYSTEM
  			| FILE_ATTRIBUTE_ARCHIVE
  			| FILE_ATTRIBUTE_NORMAL
  			| FILE_ATTRIBUTE_TEMPORARY
  			| FILE_ATTRIBUTE_SPARSE_FILE
  			| FILE_ATTRIBUTE_COMPRESSED
  			| FILE_ATTRIBUTE_NOT_CONTENT_INDEXED
  			| FILE_ATTRIBUTE_ENCRYPTED
  			| FILE_ATTRIBUTE_HIDDEN));

  	if (0 == flags)
  	rc = TF_REGULAR;
  	else if (0 != (_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
  		rc = (0 != (_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
  		? TF_LINK_DIR : TF_SUBDIR;
  	} else if (0 != (_data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT))
  	rc = TF_LINK;
  	else
  	rc = TF_OTHER;
  #endif
  return rc;
}

DirEntryFilter::DirEntryFilter() :
    _types(FsEntry::TC_ALL), _nodePatterns(nullptr), _pathPatterns(nullptr), _minSize(
        0), _maxSize(0x7fffffffffffffffLL), _minAge(), _maxAge(), _minDepth(0), _maxDepth(
        512) {
  memset(_first, 'x', sizeof _first - 1);
  _first[sizeof _first - 1] = '\0';
  memset(_last, 'x', sizeof _last - 1);
  _last[sizeof _last - 1] = '\0';
  setFiletime(_minAge, 0, 0);
  setFiletime(_maxAge, 0x7fffffffffffffffL, 0);
}

DirEntryFilter::~DirEntryFilter() {
}

bool DirEntryFilter::match(FsEntry &entry, DirTreeStatistic *statistics) const {
  bool testNamesFirst = entry.testNamesFirst();
  const char *node = nullptr;
  bool rc;
  do {
    rc = false;
    if (testNamesFirst) {
      node = entry.node();
      if (_nodePatterns != nullptr && !_nodePatterns->match(node)) {
        break;
      }
    }
    if (0 == (entry.type() & _types)) {
      break;
    }
    int64_t size = entry.fileSize();
    if (size < _minSize || size > _maxSize) {
      break;
    }
    if (fileTimeCompare(_minAge, *entry.modified()) > 0
        || fileTimeCompare(_maxAge, *entry.modified()) < 0) {
      break;
    }
    if (!testNamesFirst) {
      node = entry.node();
      if (_nodePatterns != nullptr && !_nodePatterns->match(node)) {
        break;
      }
    }
    rc = true;
  } while (false);
  if (statistics != nullptr) {
    if (rc) {
      if (entry.isRegular()) {
        statistics->_sizes += entry.fileSize();
        statistics->_files++;
      } else if (!entry.isDirectory()) {
        statistics->_files++;
      }
    }
  }
  return rc;
}

/**
 * Constructor.
 */
DirTreeStatistic::DirTreeStatistic() :
    _directories(0), _files(0), _sizes(0ll), _ignoredFiles(0), _ignoredDirectories(
        0) {
}
const char*
DirTreeStatistic::statisticAsString(std::string &string, bool append,
    const char *formatFiles, const char *formatSizes, const char *formatDirs) {
  if (!append)
    string.clear();
  appendInt(string, _files, formatFiles);
  appendString(string, "file(s)", " ");
  appendDouble(string, static_cast<double>(_sizes) / 1000.0 / 1000,
      formatSizes);
  appendString(string, " ", "MByte ");
  appendInt(string, _directories, formatDirs);
  appendString(string, "dirs(s)");
  appendString(string, " Ignored: ");
  appendInt(string, _ignoredDirectories, formatDirs);
  appendString(string, " dir(s)");
  appendInt(string, _ignoredFiles, formatFiles);
  appendString(string, " file(s) ");
  return string.c_str();
}

TraceUnit::TraceUnit(int triggerCount, int interval) :
    _count(0), _triggerCount(triggerCount), _lastTrace(0), _interval(
        interval * CLOCKS_PER_SEC), _startTime(clock()) {
  _lastTrace = _startTime;
}
/**
 * Destructor.
 */
TraceUnit::~TraceUnit() {
}

/**
 * Prints a message.
 *
 * Often overwritten by a subclass.
 *
 * @param message	message for the trace
 * @return			<em>true</em> (for chaining)
 */
bool TraceUnit::trace(const char *message) {
  printf("%s\n", message);
  return true;
}

FileAgentWithStack::FileAgentWithStack(const char *base,
    const DirEntryFilter *filter, TraceUnit *tracer, Logger *logger,
    DirTreeStatistic &statistics) :
    FileAgent(), _depth(-1), _base(base),
    // m_dirs
    _passNoForDirSearch(2), _filter(filter), _tracer(tracer), _logger(logger), _statistics(
        statistics), _separator(0) {
  auto theOsInfo = osInfo();
  _separator = theOsInfo._separatorChar;
  memset(m_dirs, 0, sizeof m_dirs);
  m_dirs[0] = new FsEntryWithStack(_logger);
// remove a preceding "./". This simplifies the pattern expressions:
  if (_base.size() > 0 && _base[0] == '.'
      && _base[1] == theOsInfo._separatorChar) {
    _base.erase(0, 2);
  }
}

/**
 * Destructor.
 */
FileAgentWithStack::~FileAgentWithStack() {
  destroy();
}

/**
 * Initializes the instance to process a new base.
 *
 * @param base	the base directory to search
 */
void FileAgentWithStack::changeBase(const char *base) {
  destroy();
  _base.clear();
  appendString(_base, base);
  memset(m_dirs, 0, sizeof m_dirs);
  m_dirs[0] = new FsEntryWithStack(_logger);
// remove a preceding "./". This simplifies the pattern expressions:
  if (_base.size() > 0 && _base[0] == '.'
      && _base[1] == osInfo()._separatorChar) {
    _base.erase(0, 2);
  }
}
/**
 * Releases the resources.
 */
void FileAgentWithStack::destroy() {
  for (size_t ix = 0; ix < sizeof m_dirs / sizeof m_dirs[0]; ix++) {
    if (m_dirs[ix] != nullptr) {
      m_dirs[ix]->freeEntry();
      delete m_dirs[ix];
      m_dirs[ix] = nullptr;
    }
  }
}
/**
 * Returns the info about the next file in the directory tree traversal.
 *
 * @param level	OUT: the level relative to the base.<br>
 * 					0 means the file is inside the base.<br>
 * 					Not defined if the result is nullptr
 * @return nullptr	no more files<br>
 * 					otherwise: the stack entry with the next file in the
 * 					directory tree. May be a directory too
 */
FsEntry*
FileAgentWithStack::rawNextFile(int &level) {
  FsEntry *rc = nullptr;
  bool alreadyRead = false;
  bool again;
  do {
    again = false;
    if (_depth < 0) {
      // Not yet initialized?
      if (m_dirs[0]->_passNo == 2)
        rc = nullptr;
      else {
        // first call:
        if (initEntry(_base.c_str(), nullptr, 0)) {
          _statistics._directories++;
          if (1 != _passNoForDirSearch)
            rc = m_dirs[0];
          else
            again = alreadyRead = true;
        }
      }
    } else {
      FsEntryWithStack *current = m_dirs[_depth];
      if (alreadyRead || current->findNext()) {
        alreadyRead = false;
        // a file or directory found:
        if (_tracer != nullptr && _tracer->isCountTriggered()
            && _tracer->isTimeTriggered())
          _tracer->trace(current->fullName());
        if (current->_passNo != _passNoForDirSearch) {
          // we search for any file:
          rc = m_dirs[_depth];
        } else {
          // we are interested only in true subdirectories:
          again = true;
          if (current->isDirectory()) {
            if (!current->isDotDir()) {
              bool doEnter = true;
              if (_filter != nullptr) {
                doEnter = _depth < _filter->_maxDepth && !current->isLink()
                    && (_filter->_pathPatterns == nullptr
                        || _filter->_pathPatterns->match(current->node()));
              }
              if (doEnter) {
                // open a new level
                alreadyRead = initEntry(current->_path, current->node(),
                    _depth + 1);
                _statistics._directories++;
                if (_logger->currentLevel() >= LV_DETAIL) {
                  _logger->say(LV_DETAIL,
                      formatCString("- ignored: %s%c", current->_path,
                          _separator));
                }

              } else {
                _statistics._ignoredDirectories++;
                if (_logger->currentLevel() >= LV_DETAIL) {
                  _logger->say(LV_DETAIL,
                      formatCString("+ ignored: %s%c", current->_path,
                          _separator));
                }
              }
            }
          }
        }
      } else {
        // the current subdir does not have more files:
        if (current->_passNo == 1) {
          // we start the second pass:
          alreadyRead = initEntry(current->_path, nullptr, -1);
          current->_passNo = 2;
          again = true;
        } else {
          // this subdirectory is complete. We continue in the parent directory:
          current->freeEntry();
          if (--_depth >= 0) {
            again = true;
          }
        }
      }
    }
    if (rc != nullptr && rc->isDotDir())
      again = true;
  } while (again);
  if (rc != nullptr && !rc->isDirectory()) {
    _statistics._files++;
    if (_statistics._sizes >= 0)
      _statistics._sizes += rc->fileSize();
  }
  level = _depth;
  return rc;
}
void FileAgentWithStack::freeEntry(int level) {
}

/**
 * Initializes an entry in the directory entry stack.
 *
 * @param parent The parent directory of the entry
 * @param node <em>nullptr</em> or the name of the directory belonging to the entry (without path)
 * @param level The index of the entry in the stack.<br>
 *   If < 0: m_levels and m_path will not be changed
 * @return <em>true</em>: a new file is available<br>
 *   <em>false</em>: findFirstEntry() signals: no entry.
 */
bool FileAgentWithStack::initEntry(const std::string &parent, const char *node,
    int level) {
  bool rc = false;
  if (level < MAX_ENTRY_STACK_DEPTH) {
    auto theOsInfo = osInfo();
    //printf ("node: %s\n", node);
    if (level >= 0)
      _depth = level;
    if (m_dirs[_depth] == nullptr)
      m_dirs[_depth] = new FsEntryWithStack(_logger);
    FsEntryWithStack *current = m_dirs[_depth];
    current->_passNo = 1;
    if (level >= 0) {
      current->_path = parent;
      if (!endsWith(parent.c_str(), parent.size(), theOsInfo._separatorCString))
        appendString(current->_path, theOsInfo._separatorCString);
      if (node != nullptr)
        appendString(current->_path, node, theOsInfo._separatorCString);
    }
    rc = current->findFirst();
  }
  return rc;
}

/**
 * Returns the info of an entry the directory stack.
 *
 * @param offsetFromTop 0: return the top of stack.<br>
 *   1: returns the entry one below the top.<br>
 *   2: ...
 * @return <em>nullptr</em>: not available<br>
 *   Otherwise: the wanted entry.
 */
FsEntryWithStack*
FileAgentWithStack::topOfStack(int offsetFromTop) {
  FsEntryWithStack *rc = nullptr;
  if (offsetFromTop >= 0 && _depth - 1 - offsetFromTop >= 0)
    rc = m_dirs[_depth - 1 - offsetFromTop];
  return rc;
}

FileAgentLinux::FileAgentLinux(const char *base, const DirEntryFilter *filter,
    TraceUnit *tracer, Logger *logger, DirTreeStatistic &statistics) :
    FileAgent(), _base(base == nullptr ? "." : base), _filter(filter), _tracer(
        tracer), _logger(logger), _fts(nullptr), _statistics(statistics), _currentRawEntry(
        nullptr), _currentEntry(), _currentNo(0), _lastLevel(0) {
  bool intrinsic = !filter->testNameFirst();
  _currentEntry.setIntrinsicStatInfo(intrinsic);
  if (base != nullptr) {
    char *argv[] = { (char*) base, nullptr };

    ;
    auto flags = FTS_COMFOLLOW | FTS_PHYSICAL;
    if (!intrinsic) {
      flags |= FTS_NOSTAT;
    }
    _fts = fts_open(argv, flags, nullptr);
  }
}

FileAgentLinux::~FileAgentLinux() {
  if (_fts != nullptr) {
    fts_close(_fts);
    _fts = nullptr;
  }
}

void FileAgentLinux::changeBase(const char *base) {
  if (_fts != nullptr) {
    fts_close(_fts);
    _fts = nullptr;
  }
  char *argv[] = { (char*) base, nullptr };
  _fts = fts_open(argv, FTS_COMFOLLOW | FTS_PHYSICAL | FTS_DONTCHDIR, nullptr);
}

FsEntry*
FileAgentLinux::rawNextFile(int &level) {
  char message[8192];
  bool again = false;
  FsEntry *rc = nullptr;
  do {
    again = false;
    ++_currentNo;
    _currentRawEntry = fts_read(_fts);
    if (_currentRawEntry == nullptr) {
      rc = nullptr;
      break;
    }
    if (_currentRawEntry->fts_level != _lastLevel) {
      if (_currentRawEntry->fts_level > _lastLevel) {
        _statistics._directories++;
      }
      _lastLevel = _currentRawEntry->fts_level;
    }
    switch (_currentRawEntry->fts_info) {
    case FTS_D:
      // Pre order directory: ignore it, it comes again as post order.
      again = true;
      break;
    case FTS_DP:
      // Post order directory:
      if (_currentRawEntry->fts_pathlen == _base.size()
          && strcmp(_currentRawEntry->fts_path, _base.c_str()) == 0) {
        // We ignore the base directory:
        again = true;
      }
      break;
    case FTS_DNR:
      // unreadable directory:
      _statistics._ignoredDirectories++;
      if (_logger->currentLevel() >= LV_FINE) {
        _logger->say(LV_FINE,
            formatOnBuffer(message, sizeof message, "_ ignored: %s/",
                _currentRawEntry->fts_path));
      }
      again = true;
      break;
    case FTS_SL:
      break;
    case FTS_DOT:
      // dot or dot-dot
    case FTS_ERR:
      // error; errno is set
    case FTS_INIT:
      // initialized only
    case FTS_NS:
      // stat(2) failed
    case FTS_W:
      // whiteout object
    default:
      _statistics._ignoredFiles++;
      if (_logger->currentLevel() >= LV_DEBUG) {
        _logger->say(LV_DEBUG,
            formatOnBuffer(message, sizeof message, "_ ignored: %s",
                _currentRawEntry->fts_path));
      }
      again = true;
      break;
    case FTS_NSOK:
      // no stat(2) requested
    case FTS_F:
      // regular file
      break;
    }
    rc = &_currentEntry;
    _currentEntry.set(_currentRawEntry);
    if (_filter != nullptr && _filter->_pathPatterns != nullptr) {
      if (!_filter->_pathPatterns->match(_currentRawEntry->fts_name)) {
        fts_set(_fts, _currentRawEntry, FTS_SKIP);
        if (_currentNo > 1) {
          _statistics._ignoredDirectories++;
          if (_logger->currentLevel() >= LV_FINE) {
            _logger->say(LV_FINE,
                formatOnBuffer(message, sizeof message, "_ ignored: %s/",
                    _currentRawEntry->fts_path));
          }
        }
      }
    }
  } while (again);
  return rc;
}

Traverser::Traverser(const char *base, const DirEntryFilter *filter,
    TraceUnit *tracer, Logger *logger) :
    _base(base == nullptr ? "." : base), _filter(filter), _tracer(tracer), _logger(
        logger), _fileAgent(nullptr), _osInfo(osInfo()) {
  _fileAgent = new FileAgentLinux(base, filter, tracer, logger, *this);
}

Traverser::~Traverser() {
  delete _fileAgent;
  _fileAgent = nullptr;
}
void Traverser::changeBaseByPatterns(const char *patternList,
    DirEntryFilter &filter) {
  auto items = splitCString(patternList, ",");
  PathInfo info;
  splitPath(items[0].c_str(), info);
  changeBase(info._path.empty() ? "." : info._path.c_str());
  items[0] = info._name + info._extension;
  auto baseWithoutPath = joinVector(items, ",");
  delete filter._nodePatterns;
  filter._nodePatterns = nullptr;
  filter._nodePatterns = new PatternList();
  filter._nodePatterns->set(baseWithoutPath.c_str(), baseWithoutPath.length(),
      true, ",");
}

FsEntry*
Traverser::nextFile(int &level) {
  FsEntry *rc = _fileAgent->rawNextFile(level);
  auto baseLength = _base.length();
//const char *node;
  while (rc != nullptr) {
    //node = rc->fullName();
    size_t len = 0;
    if ((_filter == nullptr || _filter->match(*rc, this))
        && (level > 0 || (len = rc->fullNameLength()) > baseLength)) {
      break;
    }
    rc = _fileAgent->rawNextFile(level);
  }
  return rc;
}

} /* cppknife */
