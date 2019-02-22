#pragma once

#include <queue>
#include <vector>

#include "event.h"

class EmptyQueueException : public std::exception {
public:
  virtual char const* what() const throw();
};

class EventQueue {
private:
  std::priority_queue<Event, std::vector<Event>, CompareEvents> queue;
  
public:
  EventQueue();

  size_t size() const;

  // Schedules a new share event
  void schedule(Event _event);

  // Pops and returns first element in event queue;
  Event pop();

  // Returns whether the event queue is empty or not
  bool is_empty() const;

  // Returns event first in queue
  Event get_top() const;
};
