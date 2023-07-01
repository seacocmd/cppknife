/*
 * Scheduler.hpp
 *
 *  Created on: 18.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TOOLS_SCHEDULER_HPP_
#define TOOLS_SCHEDULER_HPP_

class TimerTask;
class TaskHandler {
	public:
	TaskHandler();
	virtual ~TaskHandler();
	public:
	virtual void run(TimerTask* timerTask) = 0;
};
class Scheduler;
class TimerTask {
	friend TaskHandler;
protected:
	Scheduler& _scheduler;
	TaskHandler& _handler;
	std::string _name;
	time_t _start;
	int _interval;
	std::string _options;
public:
	TimerTask(Scheduler& scheduler, TaskHandler& handler, const char* name, time_t start, int interval = 0, const char* options = nullptr);
	virtual ~TimerTask();
public:
	int interval() const {
		return _interval;
	}
	void run();
	time_t start() const {
		return _start;
	}
	void setStart(time_t start){
		_start = start;
	}
};
class Scheduler {
protected:
	std::deque<TimerTask*> _tasks;
	bool _stop;
public:
  Scheduler();
  virtual ~Scheduler();
public:
  void addTask(TimerTask* task);
  void execute(time_t now, TimerTask *task);
  void moveTask(TimerTask* task);
  void schedule();
};

#endif /* TOOLS_SCHEDULER_HPP_ */
