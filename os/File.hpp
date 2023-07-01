/*
 * File.hpp
 *
 *  Created on: 24.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_FILE_HPP_
#define OS_FILE_HPP_

namespace cppknife {
enum FileType {
	FT_UNDEF, FT_FILE, FT_DIRECTORY, FT_SYMBOLIC_LINK
};
/**
 * Manages a file system entry, the base class of files, directories and symbolic links.
 *
 */
class FileSystemEntity {
protected:
	FileType _fileType;
	struct stat _statInfo;
	///
	/// The filename with path.
	std::string _fullName;
	/// The filename without path
	std::string _node;
	///
	/// The path of the file without trailing separator.
	std::string _path;
public:
	FileSystemEntity(FileType fileType, const char *fullName,
			const struct stat *statInfo = nullptr);
	FileSystemEntity(FileType fileType, const char *node, const char *path,
			const struct stat *statInfo = nullptr);
	virtual
	~FileSystemEntity();
	FileSystemEntity(const FileSystemEntity &other);
	FileSystemEntity&
	operator=(const FileSystemEntity &other);
public:
	/**
	 * Tests whether the file exists.
	 */
	bool
	exists(bool force = false);
	/**
	 * Returns the filename with path.
	 */
	inline const std::string&
	fullName() const {
		return _fullName;
	}
	/**
	 * Returns the creation date time of the file.
	 * @return UNDEF_TIME: file time not accessable.<br>
	 *   Otherwise: the creation time in seconds since epoche.
	 */
	time_t
	created();
	/**
	 * Returns the modification date time of the file.
	 * @return UNDEF_TIME: file time not accessable.<br>
	 *   Otherwise: the modification time in seconds since epoche.
	 */
	time_t
	modified();
	/**
	 * Returns the filename without path.
	 */
	const std::string&
	node() const {
		return _node;
	}
	/**
	 * Returns the path name of the file.
	 */
	const std::string&
	path() const {
		return _path;
	}
protected:
	inline void clearStatus() {
		_statInfo.st_mtim.tv_sec = 0;
	}
	inline bool statIsCalled() {
		return _statInfo.st_mtim.tv_sec != 0;
	}
};
/**
 * Manages a
 */
class File: public FileSystemEntity {
protected:
public:
	File(const char *fullName, const struct stat *statInfo = nullptr);
	File(const char *node, const char *path, const struct stat *statInfo =
			nullptr);
	virtual
	~File();
	File(const File &other);
	File&
	operator=(const File &other);
public:
	/**
	 * Returns the file size.
	 * @return UNDEF_SIZE: the file does not exists.
	 *   Otherwise: the file size in byte.
	 */
	FileSize_t
	size();
};
/**
 * Manages a directory.
 */
class Directory: public FileSystemEntity {
protected:
public:
	Directory(const char *fullName, const struct stat *statInfo = nullptr);
	virtual
	~Directory();
	Directory(const Directory &other);
	Directory&
	operator=(const Directory &other);
};

class SymbolicLink: public FileSystemEntity {
protected:
public:
	SymbolicLink(const char *fullName, const struct stat *statInfo = nullptr);
	SymbolicLink(const char *node, const char *path,
			const struct stat *statInfo = nullptr);
	virtual
	~SymbolicLink();
	SymbolicLink(const SymbolicLink &other);
	SymbolicLink&
	operator=(const SymbolicLink &other);
};

} /* cppknife */

#endif /* OS_FILE_HPP_ */
