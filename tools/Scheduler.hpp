/*
 * Scheduler.hpp
 *
 *  Created on: 18.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#ifndef TOOLS_SCHEDULER_HPP_
#define TOOLS_SCHEDULER_HPP_

namespace cppknife {
class TimerTask;
/**
 * @brief Defines an interface (abstract class) to different task handlers.
 */
class TaskHandler {
public:
  TaskHandler();
  virtual ~TaskHandler();
public:
  virtual void run(TimerTask *timerTask) = 0;
};
class Scheduler;

/**
 * @brief Stores a combination of a point in time and the task to do at that time.
 */
class TimerTask {
  friend TaskHandler;
protected:
  Scheduler &_scheduler;
  TaskHandler &_handler;
  std::string _name;
  time_t _start;
  int _interval;
  std::string _options;
public:
  TimerTask(Scheduler &scheduler, TaskHandler &handler, const char *name,
      time_t start, int interval = 0, const char *options = nullptr);
  virtual ~TimerTask();
public:
  int interval() const {
    return _interval;
  }
  void run();
  time_t start() const {
    return _start;
  }
  void setStart(time_t start) {
    _start = start;
  }
};

/**
 * @brief Manages an amount of tasks that must be done at given point in time.
 */
class Scheduler {
protected:
  std::deque<TimerTask*> _tasks;
  bool _stop;
public:
  Scheduler();
  virtual ~Scheduler();
public:
  void addTask(TimerTask *task);
  void execute(time_t now, TimerTask *task);
  void moveTask(TimerTask *task);
  void schedule();
};
} /* namespace cppknife */
#endif /* TOOLS_SCHEDULER_HPP_ */
