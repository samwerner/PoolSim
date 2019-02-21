#include <iostream>

#include "event_queue.h"
#include "event.h"

char const* EmptyQueueException::what() const throw() {
  return "the queue is empty";
}

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
  if (is_empty()) {
    throw EmptyQueueException();
  }
  return queue.top();
}
