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
 * A base class for handlers implementing a sub command.
 */
class CommandHandler {
protected:
  ArgumentParser &_argumentParser;
  Logger *_logger;
  DirEntryFilter _filter;
  int _level;
  FsEntry *_status;
  Traverser _traverser;
public:
  CommandHandler(ArgumentParser &argumentParser, Logger *logger);
  virtual ~CommandHandler();
public:
  /**
   * Checks the preconditions.
   * @return <em>true</em>: success
   */
  virtual bool check();
  /**
   * Does things after handling all files.
   */
  virtual void finish();
  /**
   * Does things before handling all files.
   */
  virtual void initialize();
  /**
   * Does the things for one file.
   */
  virtual bool oneFile() = 0;
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
