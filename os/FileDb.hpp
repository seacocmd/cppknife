/*
 * FileDb.hpp
 *
 *  Created on: 27.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_FILEDB_HPP_
#define OS_FILEDB_HPP_

namespace cppknife {

enum TableType {
	TT_UNDEF,
	TT_LIST,
	TT_PAIR
};
class FileDb;
class FileTable {
protected:
	std::string _filename;
	FileDb& _db;
public:
	FileTable(TableType type, FileDb& db, const char* filename);
	virtual ~FileTable();
public:
	virtual void read() = 0;
	virtual void write() = 0;
};
class ListTable : public FileTable {
public:
	ListTable(FileDb& db, const char* filename);
	virtual ~ListTable();
public:
	virtual void read();
	virtual void write();
};
class FileDb {
protected:
	std::string _directory;
	std::vector<FileTable*> _tables;
	CharRandom* _cryptoEngine;
public:
  FileDb(const char* directory);
  virtual ~FileDb();
public:
	void read();  
	void write();
	FileTable* tableByName(const char* table);
};

} /* namespace cppknife */

#endif /* OS_FILEDB_HPP_ */
