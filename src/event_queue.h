#pragma once

#include <queue>
#include <vector>

#include "event.h"

class EventQueue {
private:
  std::priority_queue<Event, std::vector<Event>, CompareEvents> queue;
  
public:
  EventQueue();

  // Schedule a new share event
  void schedule(Event _event);

  // Pops and returns first element in event queue;
  Event pop();

  // Returns whether the event queue is empty or not
  bool is_empty() const;

  // Returns event first in queue
  Event get_top();
};
