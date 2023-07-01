/*
 * Traverser.hpp
 *
 *  Created on: 22.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_TRAVERSER_HPP_
#define OS_TRAVERSER_HPP_
#ifndef _DIRENT_H
#include <dirent.h>
#endif
namespace cppknife {

/**
 * A portable interface to a file system entry like file, directory, symbol link...
 * Offers only the properties of the entry, no actions.
 * Abstract base class.
 */
class FsEntry {
public:
  enum Type_t {
    TF_UNDEF = 0,
    // single property flags:
    TF_SUBDIR = 1 << 0,
    TF_REGULAR = 1 << 1,
    TF_LINK = 1 << 2,
    TF_LINK_DIR = 1 << 3,
    TF_BLOCK = 1 << 4,
    TF_PIPE = 1 << 5,
    TF_CHAR = 1 << 6,
    TF_SOCKET = 1 << 7,
    TF_OTHER = 1 << 8,
    TF_UNKNOWN = 1 << 9,
    // collections:
    TC_SPECIAL = (TF_BLOCK | TF_CHAR | TF_SOCKET | TF_PIPE | TF_OTHER),
    TC_NON_DIR = (TC_SPECIAL | TF_LINK | TF_REGULAR),
    TC_TEXT = (TF_LINK | TF_REGULAR),
    TC_ALL = (TF_SUBDIR | TC_NON_DIR | TF_LINK_DIR | TF_UNKNOWN),
  };
public:
  virtual
  ~FsEntry();
public:
  /**
   * Returns the last access time.
   */
  virtual const FileTime_t*
  accessed() = 0;
  /**
   * Frees the resources used in setCertainFile().
   */
  virtual void finishCertainFile() = 0;
  /**
   * Returns the full name independent from the current directory.
   */
  virtual const char*
  accessFullName() = 0;
  /**
   * Returns the file size.
   */
  virtual FileSize_t
  fileSize() = 0;
  /**
   * Returns the modified time as string.
   */
  virtual std::string
  filetimeAsString() = 0;
  /**
   * Returns the name with path.
   */
  virtual const char*
  fullName() = 0;
  /**
   * Returns the length of the full name.
   */
  virtual size_t fullNameLength() = 0;
  /**
   * Returns whether the entry is a directory.
   */
  virtual bool
  isDirectory() = 0;
  /**
   * Returns whether the entry is a "." or the ".." directory.
   */
  virtual bool
  isDotDir() = 0;
  /**
   * Returns whether the entry is a symbolic link.
   */
  virtual bool
  isLink() = 0;
  /**
   * Returns whether the regular file: not a directory, special file.
   */
  virtual bool
  isRegular() = 0;
  /**
   * Returns the last modified date time.
   */
  virtual const FileTime_t*
  modified() = 0;
  /**
   * Returns the filename without path.
   */
  virtual const char*
  node() = 0;
  /**
   * Returns the name of the referenced file of a symbolic link.
   */
  virtual const std::string&
  linkReference() = 0;
  /**
   * Returns the file rights as a string.
   * @param[out] buffer The result buffer.
   * @param numerical <em>true</em>: the rights are returned as numeric values.
   * @param ownerWidth The width of the partitional field owner.
   */
  virtual const char*
  rightsAsString(std::string &buffer, bool numerical, int ownerWidth) = 0;
  /**
   * Sets the internal data for a given file.
   * @param fullName The full filename.
   * @return <em>true</em>: the file exists.
   */
  virtual bool setCertainFile(const char *fullName) = 0;

  /**
   * Tests whether it is more powerful to do the name matching before other matching.
   */
  virtual bool testNamesFirst() const = 0;
  /**
   * Returns the file type.
   */
  virtual Type_t
  type() = 0;
  /**
   * Returns the file type as a character.
   */
  virtual char
  typeAsChar();

};
class TraverserLinux;
class DirEntryFilter;
class FsEntryLinux: public FsEntry {
  friend TraverserLinux;
protected:
  FTS *_singleFileFts;
  bool _intrinsicStatInfo;
  FTSENT *_state;
  std::string _linkReference;
  Type_t _type;
  struct stat *_statInfo;
  struct stat _statBuffer;
public:
  FsEntryLinux();
  virtual
  ~FsEntryLinux();
public:
  virtual const FileTime_t*
  accessed();
  virtual const char*
  accessFullName();
  virtual FileSize_t
  fileSize();
  virtual std::string
  filetimeAsString();
  virtual void finishCertainFile();
  virtual const char*
  fullName();
  virtual size_t fullNameLength();
  struct stat* getStatus();
  bool intrinsicStatInfo() const {
    return _intrinsicStatInfo;
  }
  virtual bool
  isDirectory();
  virtual bool
  isDotDir();
  virtual bool
  isLink();
  virtual bool
  isRegular();
  virtual const FileTime_t*
  modified();
  virtual const char*
  node();
  virtual const std::string&
  linkReference();
  virtual const char*
  rightsAsString(std::string &buffer, bool numerical, int ownerWidth);
  void
  set(FTSENT *entry);
  virtual bool setCertainFile(const char *fullName);
  void setIntrinsicStatInfo(bool value) {
    _intrinsicStatInfo = value;
  }
  inline virtual bool testNamesFirst() const {
    //return !_intrinsicStatInfo;
    return true;
  }
  virtual Type_t
  type();
};
class FsEntryWithStack: public FsEntry {
public:
  std::string _path;
  std::string _fullName;
  int _passNo;
  Logger *_logger;
#ifdef __linux__
  DIR *_handle;
  struct dirent *_data;
  struct stat _status;
  std::string _linkReference;
public:
  struct stat*
  getStatus();
#elif defined WIN32
    HANDLE _handle;
    WIN32_FIND_DATAA _data;
    bool m_getPrivilege;
#endif
public:
  /**
   * Constructor.
   */
  FsEntryWithStack(Logger *logger);
  virtual
  ~FsEntryWithStack();
public:
  bool
  findFirst();
  bool
  findNext();
  void
  freeEntry();
public:
  virtual const FileTime_t*
  accessed();
  virtual void finishCertainFile();
  virtual const char*
  accessFullName();
  virtual FileSize_t
  fileSize();
  virtual std::string
  filetimeAsString();
  virtual const char*
  fullName();
  virtual size_t fullNameLength();
  virtual bool
  isDirectory();
  virtual bool
  isDotDir();
  virtual bool
  isLink();
  virtual bool
  isRegular();
  virtual const FileTime_t*
  modified();
  virtual const char*
  node();
  virtual const std::string&
  linkReference();
  /**
   * Returns the file rights as a string.
   *
   * @param buffer		OUT: the file rights
   * @param numerical		<em>true</em>: the owner/group should be numerical (UID/GID)
   * @param ownerWidth	the width for group/owner
   * @return				<em>buffer.str()</em> (for chaining)
   */
  virtual const char*
  rightsAsString(std::string &buffer, bool numerical, int ownerWidth);
  virtual bool setCertainFile(const char *fullName);
  inline virtual bool testNamesFirst() const {
    return false;
  }
  virtual Type_t
  type();
public:
#if defined __WIN32__
  	static bool getFileOwner(HANDLE handle, const char* file, std::string& name,
  		Logger* logger = NULL);
  	static bool getPrivilege(const char* privilege, Logger* logger);
  #endif
};
class DirTreeStatistic;
class DirEntryFilter {
public:
  DirEntryFilter();
  ~DirEntryFilter();
public:
  bool
  /**
   * Tests whether an entry matches the conditions of the filter.
   *
   * @param entry Entry to test.
   * @param[out] statistics <em>nullptr</em> or the hit statistic.
   * @return <em>true</em>: the entry matches the conditions of the filter<br>
   *   <em>false</em>: otherwise
   */
  match(FsEntry &entry, DirTreeStatistic *statistics) const;
  /**
   * Returns whether the name matching test should be before the other matching tests.
   */
  inline bool testNameFirst() const {
    return _nodePatterns != nullptr;
  }
public:
  char _first[16];
  FsEntry::Type_t _types;
  PatternList *_nodePatterns;
  PatternList *_pathPatterns;
  FileSize_t _minSize;
  FileSize_t _maxSize;
  FileTime_t _minAge;
  FileTime_t _maxAge;
  int _minDepth;
  int _maxDepth;
  char _last[16];
};

/**
 * Manages trigger points.
 * A trigger point is reached if a given amount of time or a given count of files has been reached.
 * Than an action is done, e.g. showing a status message.
 */
class TraceUnit {
protected:
  int _count;
  int _triggerCount;
  clock_t _lastTrace;
  /// time interval in clocks (sec * CLOCK_PER_SEC)
  clock_t _interval;
  clock_t _startTime;
public:
  /**
   * Constructor.
   *
   * @param triggerCount Efficiency: only every N calls a time check takes place.
   * @param interval The minimum number of seconds between two traces.
   */
  TraceUnit(int triggerCount = 10, int interval = 60);
  virtual
  ~TraceUnit();
public:
  /** Returns whether the the instance is triggered.
   * To avoid too the not cheap call of time() the trace unit uses a counter.
   * If the counter reaches a given level the time check should be done.
   * @return <em>true</em>: the counter has reached <em>m_triggerCount</em>
   */
  inline bool isCountTriggered() {
    bool rc = ++_count % _triggerCount == 0;
    return rc;
  }
  /** Tests whether a given waiting time has been gone since the last trace.
   * @return	<em>true</em>: the last trace has been done after
   *			at least <em>m_interval</em> seconds
   */
  inline bool isTimeTriggered() {
    clock_t now = clock();
    bool rc = (now - _lastTrace) >= _interval;
    if (rc) {
      _lastTrace = now;
    }
    return rc;
  }
  virtual bool
  trace(const char *message);
};

class DirTreeStatistic {
public:
  int _directories;
  int _files;
  int64_t _sizes;
  int _ignoredFiles;
  int _ignoredDirectories;
public:
  DirTreeStatistic();
public:
  /**
   * Builds a string describing the data.
   *
   * @param[in out] buffer A buffer for the result.
   * @param append <em>true</em>: the string will be appended to the buffer<br>
   * 						<em>false</em>: the buffer will be cleared at the beginning.
   * @param formatFiles	The <em>sprintf</em> format for the file count, e.g. "%8d"
   * @param formatSizes	The <em>sprintf</em> format for the MByte format, e.g. "%12.6f"
   * @param formatDirs	The <em>sprintf</em> format for the directory count, e.g. "%6d"
   * @return A human readable string
   */
  const char*
  statisticAsString(std::string &buffer, bool append = false,
      const char *formatFiles = "%8d ", const char *formatSizes = "%12.6f",
      const char *formatDirs = "%7d ");
  /**
   * Resets the counters.
   */
  inline void clear() {
    _files = _directories = _ignoredFiles = _ignoredDirectories = 0;
    _sizes = 0ll;
  }
  /**
   * Return the number of entered directories .
   * @return	the number of directories entered until now
   */
  inline int directories() const {
    return _directories;
  }
  /**
   * Return the number of found files.
   * @return	the number of files found until now
   */
  inline int files() const {
    return _files;
  }
  /** Returns whether the current directory has changed since the last call.
   * @param[in out] state Stored info about the current directory.
   * 					The value has no interest for the caller.
   * @return			<em>true</em>: the path has been changed
   */
  inline bool hasChangedPath(int &state) {
    bool rc = _directories > state;
    state = _directories;
    return rc;
  }
  /**
   * Return the sum of file lengths of the found files.
   * @return	the sum of file lengths of the files found until now
   */
  inline int64_t sizes() const {
    return _sizes;
  }
};
static const int MAX_ENTRY_STACK_DEPTH = 256;

/**
 * An abstract class for fetching files from a file tree.
 */
class FileAgent {
public:
  virtual ~FileAgent() {
  }
public:
  virtual void
  changeBase(const char *base) = 0;
  virtual FsEntry*
  rawNextFile(int &level) = 0;
};
class FileAgentWithStack: FileAgent {
protected:
  ///
  /// The current depth of the directory tree: 0 means in the base directory.
  int _depth;
  std::string _base;
  FsEntryWithStack *m_dirs[MAX_ENTRY_STACK_DEPTH];
  ///
  /// Each directory will be passed twice: for all files + for directories only
  /// 1: depth first 2: breadth first
  int _passNoForDirSearch;
  ///
  /// The filter criteria.
  const DirEntryFilter *_filter;
  TraceUnit *_tracer;
  Logger *_logger;
  DirTreeStatistic &_statistics;
  char _separator;
public:
  /**
   * Constructor.
   *
   * @param base The base directory. The traversal starts at this point.
   * @param filter Specifies the search criteria. May be <em>nullptr</em>.
   * @param tracer <em>nullptr</em> or a handler of trigger points.
   * @param logger The logging unit.
   * @param statistics The statistics about files and directories will be updated here.
   */
  FileAgentWithStack(const char *base, const DirEntryFilter *filter,
      TraceUnit *tracer, Logger *logger, DirTreeStatistic &statistics);
  virtual
  ~FileAgentWithStack();
public:
  virtual void
  changeBase(const char *base);
  virtual FsEntry*
  rawNextFile(int &level);
  /** Sets the tree traversal algorithm.
   * @param depthFirst <em>true</em>: files of the subdirectories will
   *                      be returned earlier
   */
  void setDepthFirst(bool depthFirst) {
    _passNoForDirSearch = depthFirst ? 1 : 2;
  }
  FsEntryWithStack*
  topOfStack(int offset = 0);
protected:
  void
  destroy();
  void
  freeEntry(int level);
  bool
  initEntry(const std::string &parent, const char *node, int level);
};
class FileAgentLinux: public FileAgent {
protected:
  std::string _base;
  const DirEntryFilter *_filter;
  TraceUnit *_tracer;
  Logger *_logger;
  FTS *_fts;
  DirTreeStatistic &_statistics;
  FTSENT *_currentRawEntry;
  FsEntryLinux _currentEntry;
  int _currentNo;
  int _lastLevel;
public:
  /**
   * Constructor.
   *
   * @param base The base directory. The traversal starts at this point.
   * @param filter Specifies the search criteria. May be <em>nullptr</em>.
   * @param tracer <em>nullptr</em> or a handler of trigger points.
   * @param logger The logging unit.
   * @param[out] statistics The statistics about files and directories is updated here.
   */
  FileAgentLinux(const char *base, const DirEntryFilter *filter,
      TraceUnit *tracer, Logger *logger, DirTreeStatistic &statistics);
  virtual
  ~FileAgentLinux();
public:
  virtual void
  changeBase(const char *base);
  virtual FsEntry*
  rawNextFile(int &level);
  FsEntry*
  nextFile(int &level);
};
class Traverser: public DirTreeStatistic {
protected:
  std::string _base;
  const DirEntryFilter *_filter;
  TraceUnit *_tracer;
  Logger *_logger;
  FileAgent *_fileAgent;
  const OsInfo &_osInfo;
public:
  /**
   * Constructor.
   *
   * @param base The base directory. The traversal starts at this point.
   * @param filter Specifies the search criteria. May be <em>nullptr</em>.
   * @param tracer <em>nullptr</em> or a handler of trigger points.
   * @param logger The logging unit.
   */
  Traverser(const char *base, const DirEntryFilter *filter, TraceUnit *tracer =
      NULL, Logger *logger = nullptr);
  virtual
  ~Traverser();
public:
  /**
   * Changes the base directory where the traversion starts.
   * @param base The directory name.
   */
  void changeBase(const char *base) {
    _base = base;
    _fileAgent->changeBase(base);
  }
  /**
   * Changes the base directory and sets the filter for given file patterns.
   * @param patternList A comma separated list of file patterns.
   *  The first pattern can be preceded by a path, e.g. "tmp/ *.c,*.h,~*tmp*".
   *  Note: the (optional) path applies for all patterns.
   *  If the pattern starts with '~' than that is an exclusion pattern
   * @param[in out] filter The filter will be modified by the patterns.
   */
  void changeBaseByPatterns(const char *patternList, DirEntryFilter &filter);
  /**
   * Returns the next file.
   * @param[out] level The directory tree depth (0 for the base directory) of the returned file.
   * @return The next file in the directory tree.
   */
  FsEntry*
  nextFile(int &level);
};
} /* cppknife */

#endif /* OS_TRAVERSER_HPP_ */
