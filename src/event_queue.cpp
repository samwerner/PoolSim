#include <iostream>

#include "event_queue.h"
#include "event.h"

EventQueue::EventQueue() {}

void EventQueue::schedule(Event event) {
  queue.push(event);
}

Event EventQueue::pop() {
  Event event = get_top();
  queue.pop();
  return event;
}

bool EventQueue::is_empty() const {
  return queue.empty();
}

Event EventQueue::get_top() {
  return queue.top();
}
