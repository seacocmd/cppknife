/*
 * ServuseTask.hpp
 *
 *  Created on: 14.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TOOLS_SERVUSETASKS_HPP_
#define TOOLS_SERVUSETASKS_HPP_

namespace cppknife {

/**
 * @brief Observes the logins of the host. Under construction.
 */
class UserCheck : public TaskHandler {
public:
	UserCheck();
	~UserCheck();
public:
	virtual void run(TimerTask* timerTask);
};
	
} /* namespace cppknife */

#endif /* TOOLS_SERVUSETASKS_HPP_ */
