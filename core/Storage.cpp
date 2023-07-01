/*
 * Storage.cpp
 *
 *  Created on: 08.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */
#include <list>

#include "core.hpp"

namespace cppknife {

void Storage::releaseAll() {
	for (auto item : _list) {
		_releaseFunction(item);
	}
	_list.clear();
}

} /* cppknife */
