/*
 * Storage.hpp
 *
 *  Created on: 08.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef STORAGE_HPP_
#define STORAGE_HPP_

namespace cppknife {
typedef void
(*ReleaseFunction)(void *address);

/**
 * @brief A storage for class instances living the whole program runtime.
 *
 * Purpose: The instances will be freed by a single call at the end of the program.
 * That implements the  singleton design pattern.
 */
class Storage {
private:
	size_t _clusterSize;
	ReleaseFunction _releaseFunction;
	std::vector<void*> _list;
public:
	Storage(ReleaseFunction releaseFunction, size_t startSize = 100,
			size_t clusterSize = 1000) :
			_clusterSize(clusterSize), _releaseFunction(releaseFunction), _list() {
		_list.reserve(startSize);
	}
	~Storage() {
		releaseAll();
	}
public:
	/**
	 * Stores an object and a function to delete that.
	 */
	void add(void *object) {
		if (_list.size() >= _list.capacity()) {
			_list.reserve(_list.capacity() + _clusterSize);
		}
		_list.push_back(object);
	}
	/**
	 * Releases all stored objects.
	 * That is only meaningful at the end of the program.
	 */
	void
	releaseAll();
};
} /* cppknife */

#endif /* STORAGE_HPP_ */
