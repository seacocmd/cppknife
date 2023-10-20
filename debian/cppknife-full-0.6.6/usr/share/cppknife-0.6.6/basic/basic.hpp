/**
 * basic.hpp
 *
 *  Created on: 01.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef BASIC_HPP_
#define BASIC_HPP_
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include <cstdio>
#include <cstdarg>
#include <ctime>
#include <string>
#include <vector>
#include <deque>
#include <cstdlib>
#include <chrono>
#include <regex>
#include "../basic/InternalError.hpp"
#include "../basic/StringTool.hpp"
#include "../basic/TimeTool.hpp"
#include "../basic/Logger.hpp"
#include "../basic/BaseRandom.hpp"
#include "../basic/CharRandom.hpp"
#include "../basic/PortableRandom.hpp"

inline int min(int a, int b) {
  return a <= b ? a : b;
}
inline int max(int a, int b) {
  return a >= b ? a : b;
}
#endif /* BASIC_HPP_ */
