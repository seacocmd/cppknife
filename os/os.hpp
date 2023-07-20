/**
 * os.hpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef OS_HPP_
#define OS_HPP_
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#if defined __linux__
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <fts.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#endif
#include <filesystem>
#ifndef BASIC_HPP_
#include "../basic/basic.hpp"
#endif
#ifndef TEXT_HPP_
#include "../text/text.hpp"
#endif
#include "OsException.hpp"
#include "Path.hpp"
#include "FileTool.hpp"
#include "LineAgent.hpp"
#include "File.hpp"
#include "Traverser.hpp"
#include "Process.hpp"
//#include "Traverser.hpp"
#endif /* OS_HPP_ */
