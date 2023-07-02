/*
 * Scheduler.cpp
 *
 *  Created on: 18.07.2022
 *      Author: seacocmd
 *     License: CC0 1.0 Universal
 */

#include "ToolsCommons.hpp"
#include "Scheduler.hpp"
using namespace cppknife;

TaskHandler::TaskHandler() {
}
TaskHandler::~TaskHandler() {
  // do nothing
}
TimerTask::TimerTask(Scheduler &scheduler, TaskHandler &handler,
    const char *name, time_t start, int interval, const char *options) :
    _scheduler(scheduler), _handler(handler), _name(name), _start(start), _interval(
        interval), _options(options) {
}
TimerTask::~TimerTask() {
}
void TimerTask::run() {

}
Scheduler::Scheduler() :
    _tasks(100), _stop(false) {

}
int compareTimerTasks(TimerTask *task1, TimerTask *task2) {
  return task1->start() - task2->start();
}
Scheduler::~Scheduler() {
}
void Scheduler::addTask(TimerTask *task) {
  _tasks.push_back(task);
  std::sort(_tasks.begin(), _tasks.end(), compareTimerTasks);
}
void Scheduler::execute(time_t now, TimerTask *task) {
  task->run();
  auto interval = task->interval();
  if (interval > 0) {
    task->setStart(now + interval);
    addTask(task);
  }
}
void Scheduler::moveTask(TimerTask *task) {

}
void Scheduler::schedule() {
  while (!_stop) {
    time_t now = time(nullptr);
    while (_tasks.size() > 0 && _tasks[0]->start() <= now) {
      TimerTask *current = _tasks[0];
      _tasks.pop_front();
      execute(now, current);
    }
    sleep(1);
  }
}
