/*
 * StorageJobAgent.hpp
 *
 *  Created on: 21.07.2023
 *      Author: seapluspro
 *     License: CC0 1.0 Universal
 */

#ifndef NET_StorageJobAgent_HPP_
#define NET_StorageJobAgent_HPP_
namespace cppknife {

class StorageJobAgent: public StringJobAgent {
public:
  static const uint64_t SCOPE_STORAGE = 0x20656761726f7473;
  static const uint64_t JOB_PUT = 0x2020202020747570;
  static const uint64_t JOB_GET = 0x2020202020746567;
protected:
  std::map<std::string, std::string> _map;
public:
  StorageJobAgent(KnifeTaskHandler &parent);
  virtual ~StorageJobAgent();
public:
  virtual bool isResponsible(scope_t scope, job_t job);
  virtual bool process(scope_t scope, job_t job, const std::string &data);
};

} // namespace
#endif /* NET_StorageJobAgent_HPP_ */
