/*
 * ByteStorage.cpp
 *
 *  Created on: 06.01.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "core.hpp"

namespace cppknife {
ByteBuffer::ByteBuffer(size_t capacity, ByteBuffer *predecessor) :
    _size(0), _capacity(capacity), _lengthWidth(
        capacity >= 0x1000000 ? 3 : capacity >= 0x10000 ? 2 : 1), _neededBytes(
        0), _buffer(new char[capacity]), _predecessor(predecessor), _successor(
        nullptr) {
  _neededBytes = _lengthWidth + 2;
  if (predecessor != nullptr) {
    predecessor->_successor = this;
  }
}

ByteBuffer::~ByteBuffer() {
  delete[] _buffer;
  _buffer = nullptr;
}

ByteBuffer::ByteBuffer(const ByteBuffer &other) :
    _size(0), _capacity(other._capacity), _lengthWidth(other._lengthWidth), _neededBytes(
        other._neededBytes), _buffer(new char[other._capacity]), _predecessor(
        nullptr), _successor(nullptr) {
}

ByteBuffer&
ByteBuffer::operator=(const ByteBuffer &other) {
  _size = 0;
  _capacity = other._capacity;
  _buffer = new char[_capacity];
  _predecessor = nullptr;
  _predecessor = nullptr;
  return *this;
}

char*
ByteBuffer::allocate(size_t size) {
  char *rc = nullptr;
  size_t needed = size + _neededBytes;
  if (_size + needed <= _capacity) {
    memcpy(_buffer + _size, &size, _lengthWidth);
    rc = _buffer + _size + _lengthWidth;
    rc[size] = '\0';
    rc[size + 1] = static_cast<char>(-1);
    _size += needed;
  }
  return rc;
}

ByteStorage::ByteStorage(size_t capacity) :
    _capacity(capacity), _first(new ByteBuffer(capacity, nullptr)), _last(
        nullptr) {
  _last = _first;
}

ByteStorage::~ByteStorage() {
  clear();
}

ByteStorage::ByteStorage(const ByteStorage &other) :
    _capacity(other._capacity), _first(
        new ByteBuffer(other._capacity, nullptr)), _last(nullptr) {
  _last = _first;
}

ByteStorage&
ByteStorage::operator =(const ByteStorage &other) {
  _capacity = other._capacity;
  _first = new ByteBuffer(other._capacity, nullptr);
  _last = _first;
  return *this;
}

char*
ByteStorage::allocate(size_t size) {
  if (_first == nullptr) {
    _first = new ByteBuffer(_capacity, nullptr);
    _last = _first;
  }
  char *rc = _last->allocate(size);
  if (rc == nullptr) {
    if (size < _capacity - _last->_neededBytes) {
      _last = new ByteBuffer(_capacity, _last);
      rc = _last->allocate(size);
    }
  }
  return rc;
}

void ByteStorage::clear() {
  ByteBuffer *next = _first;
  while (next != nullptr) {
    auto current = next;
    next = next->_successor;
    delete current;
  }
  _first = _last = nullptr;
}

size_t ByteStorage::sizeOf(const char *data) const {
  size_t rc = 0;
  int width = _last->_lengthWidth;
  memcpy(&rc, data - width, width);
  if (rc > _capacity - _last->_neededBytes) {
    rc = 0;
  }
  return rc;
}

std::string ByteStorage::check() {
  std::string rc;
  auto current = _first;
  while (current != nullptr) {
    auto rc2 = current->check();
    if (!rc2.empty()) {
      rc = rc2;
      break;
    }
    current = current->_successor;
  }
  return rc;
}

void ByteStorage::statistics(size_t &buffers, size_t &used, size_t &vasted) {
  auto current = _first;
  buffers = used = vasted = 0;
  while (current != nullptr) {
    ++buffers;
    used += current->_size;
    if (current != _last) {
      vasted += current->_capacity - current->_size;
    }
    current = current->_successor;
  }
}

std::string ByteBuffer::check() {
  char buffer[1024];
  size_t offset = 0;
  std::string rc;
  while (offset < _size) {
    int size = 0;
    memcpy(&size, _buffer + offset, _lengthWidth);
    if (size <= 0 || size > int(_capacity - offset - _neededBytes)) {
      sprintf(buffer, "wrong size %d at offset %lu: %.20s", size, offset,
          _buffer + _lengthWidth);
      appendString(rc, buffer);
      break;
    }
    char cc;
    if ((cc = _buffer[offset + size + _lengthWidth]) != '\0') {
      sprintf(buffer, "missing EOS (%d) at offset %lu: %.20s", cc, offset,
          _buffer + _lengthWidth);
      appendString(rc, buffer);
      break;
    }
    if ((cc = _buffer[offset + size + _lengthWidth + 1]) != 255 && cc != -1) {
      sprintf(buffer, "missing 0xff (%d) at offset %lu: %.20s", cc, offset,
          _buffer + _lengthWidth);
      appendString(rc, buffer);
      break;
    }
    offset += size + _neededBytes;
  }
  return rc;
}

} /* cppknife */

