/*
 * StorageJobAgent.cpp
 *
 *  Created on: 21.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#include "net.hpp"

namespace cppknife {

StorageJobAgent::StorageJobAgent(KnifeTaskHandler &parent) :
    StringJobAgent("storage", parent), _map() {
}

StorageJobAgent::~StorageJobAgent() {
}

bool StorageJobAgent::isResponsible(scope_t scope, job_t job) {
  bool rc = scope == SCOPE_STORAGE;
  return rc;
}

bool StorageJobAgent::process(scope_t scope, job_t job,
    const std::string &data) {
  bool rc = true;
  switch (job) {
  case JOB_GET: {
    std::string answer;
    auto it = _map.find(data);
    if (it != _map.end()) {
      answer = it->second;
    }
    _parent.send((uint8_t*) answer.c_str(), answer.size());
    break;
  }
  case JOB_PUT: {
    auto parts = splitCString(data.c_str(), "\n", 2);
    if (parts.size() == 2) {
      _map[parts[0]] = parts[1];
    }
    _parent.send((uint8_t*) _ok.c_str(), _ok.size());
    break;
  }
  default:
    auto message = "<unknown job>";
    _parent.send((uint8_t*) message, strlen(message));
    break;
  }
  return rc;
}

} // namespace
