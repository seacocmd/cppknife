/**
 * cppknife.hpp
 *
 *  Created on: 21.08.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef FILEKNIFE_HPP_
#define FILEKNIFE_HPP_
#include "ToolsCommons.hpp"
#include "../geo/geo.hpp"

namespace cppknife {
int
geoknife(int argc, char **argv, Logger *loggerExtern = nullptr);
extern bool geoknifeUnderTest;
}
#endif /* FILEKNIFE_HPP_ */
