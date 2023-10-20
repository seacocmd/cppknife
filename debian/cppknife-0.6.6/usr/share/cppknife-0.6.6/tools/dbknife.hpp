/**
 * cppknife.hpp
 *
 *  Created on: 08.04.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef FILEKNIFE_HPP_
#define FILEKNIFE_HPP_
#include "ToolsCommons.hpp"
#include "Scheduler.hpp"
#include "ServuseTasks.hpp"

namespace cppknife {
class Daemon: public Scheduler {
protected:
  Configuration &_configuration;
public:
  Daemon(Logger *logger);
  virtual ~Daemon();
public:
  void initialize();
  void checkLogins();
};
int
dbknife(int argc, char **argv, Logger *loggerExtern = nullptr);
extern bool dbknifeUnderTest;
}
#endif /* FILEKNIFE_HPP_ */
