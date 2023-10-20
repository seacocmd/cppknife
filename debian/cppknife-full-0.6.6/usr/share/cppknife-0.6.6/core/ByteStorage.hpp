/*
 * ByteStorage.hpp
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef CORE_BYTESTORAGE_HPP_
#define CORE_BYTESTORAGE_HPP_

namespace cppknife {
typedef long unsigned BufferSize_t;
class ByteStorage;
/**
 * @brief Stores an amount of byte sequences.
 *
 * Each sequence is stored with the length (as BufferSize_t),
 * the data, a '\0' (for c-strings) and a marker 0xff.
 * The returned pointer points onto the data.
 */
class ByteBuffer {
	friend ByteStorage;
protected:
	size_t _size;
	size_t _capacity;
	int _lengthWidth;
	int _neededBytes;
	char *_buffer;
	ByteBuffer *_predecessor;
	ByteBuffer *_successor;
public:
	/**
	 * Constructor.
	 * @param capacity The capacity of one buffer. No requirement can exceed that count of bytes.
	 * @param predecessor The predecessor of the buffer list.
	 */
	ByteBuffer(size_t capacity, ByteBuffer *predecessor);
	virtual
	~ByteBuffer();
private:
	ByteBuffer(const ByteBuffer &other);
	ByteBuffer&
	operator=(const ByteBuffer &other);
public:
	/**
	 * Allocates a byte sequence with a given usable length.
	 * @param size The size of the required byte sequence.
	 *   Will be expanded by the length, a '\0' byte and a 0xff byte.
	 * @return A pointer of the reserved byte sequence.
	 */
	char*
	allocate(size_t size);
	/**
	 * Checks the internal chaining in the <em>_buffer</em>.
	 * @return: "": OK Otherwise: the error message
	 */
	std::string
	check();
};

/**
 * @brief Manages a very efficient storage for byte sequences, e.g. c-like strings.
 *
 * The length of the sequence is stored too.
 * It is not intended to delete individual sequences:
 * So it only supports growing memory requirements, not reducing ones.
 */
class ByteStorage {
protected:
	size_t _capacity;
	ByteBuffer *_first;
	ByteBuffer *_last;
public:
	ByteStorage(size_t capacity = 0x10000);
	virtual
	~ByteStorage();
private:
	ByteStorage(const ByteStorage &other);
	ByteStorage&
	operator=(const ByteStorage &other);
public:
	/**
	 * Allocates a byte sequence with a given usable length.
	 * @param size The size of the required byte sequence.
	 *   Will be expanded by the length, a '\0' byte and a 0xff byte.
	 * @return A pointer of the reserved byte sequence.
	 */
	char*
	allocate(size_t size);
	/**
	 * Releases all memory.
	 */
	void clear();
	/**
	 * Checks the structure of the used <em>ByteBuffer</em>.
	 * @return: "": OK Otherwise: the error message
	 */
	std::string
	check();
	/**
	 * Returns the size of a given byte sequence.
	 * @param data The pointer of the byte sequence created by <em>allocate()</em>.
	 * @return 0: invalid pointer. <br>
	 * 	Otherwise: the (usable) size of the byte sequence.
	 *
	 */
	size_t
	sizeOf(const char *data) const;
	/**
	 * Returns the statistics.
	 * @param[out] buffers The count of used buffers.
	 * @param[out] used The count of used bytes.
	 * @param[out] vasted The count of reserved bytes that cannot be used.
	 */
	void statistics(size_t &buffers, size_t &used, size_t &vasted);
};

} /* cppknife */

#endif /* CORE_BYTESTORAGE_HPP_ */
