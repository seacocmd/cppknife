/*
 * ToolsCommons.hpp
 *
 *  Created on: 20.02.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TOOLS_TOOLSCOMMONS_HPP_
#define TOOLS_TOOLSCOMMONS_HPP_

#include "../os/os.hpp"
#include "ArgumentParser.hpp"

namespace cppknife {

/**
 * @brief A base class for handlers implementing a sub command.
 */
class CommandHandler {
public:
  static CommandHandler *_lastInstance;
protected:
  ArgumentParser &_argumentParser;
  Logger *_logger;
  DirEntryFilter _filter;
  int _level;
  FsEntry *_status;
  Traverser _traverser;
  int _processedFiles;
public:
  CommandHandler(ArgumentParser &argumentParser, Logger *logger);
  virtual ~CommandHandler();
public:
  /**
   * Checks the preconditions before starting the traversation.
   * @return <em>true</em>: success
   */
  virtual bool check();
  /**
   * Does things after processing all files.
   */
  virtual void finish();
  /**
   * Does things before processing all files.
   */
  virtual void initialize();
  /**
   * Tests whether the meta data of the file allows proceessing.
   * @return <em>true</em>: the file should be processed.
   */
  virtual bool isValid();

  /**
   * Returns the last instance of the <em>CommandHandler</em>.
   */
  static CommandHandler* lastInstance() {
    return _lastInstance;
  }
  /**
   * Does the things for one file.
   */
  virtual bool oneFile() = 0;
  /**
   * Returns the number of processed files.
   */
  inline int processedFiles() const {
    return _processedFiles;
  }
  /**
   * Runs the command: Initialization, traversion and finishing.
   * @param nameSources The variable name defining the source files/directories.
   * @return The exit code: 0: success
   */
  int run(const char *nameSources);
  /**
   * Traverses the file tree and call the user defined method <em>oneFile</em> for all selected files.
   * @param nameSources The name (in argument parser) for the argument defining the files to handle.
   */
  virtual void traverse(const char *nameSources);
}
;

void addTraverserOptions(ArgumentParser &parser);
void populateFilter(const ArgumentParser &parser, DirEntryFilter &filter);
}

#endif /* TOOLS_TOOLSCOMMONS_HPP_ */
