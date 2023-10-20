/**
 * cppknife.hpp
 *
 *  Created on: 18.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#ifndef NETKNIFE_HPP_
#define NETKNIFE_HPP_
#include "ToolsCommons.hpp"
#include "../net/net.hpp"

namespace cppknife {
int
netknife(int argc, char **argv, Logger *loggerExtern = nullptr);
extern bool geoknifeUnderTest;
}
#endif /* NETKNIFE_HPP_ */
