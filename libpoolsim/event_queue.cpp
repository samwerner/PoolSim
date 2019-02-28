#include <iostream>

#include "event_queue.h"
#include "event.h"

namespace poolsim {

char const* EmptyQueueException::what() const throw() {
  return "the queue is empty";
}

EventQueue::EventQueue() {}

size_t EventQueue::size() const {
  return queue.size();
}

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

Event EventQueue::get_top() const {
  if (is_empty()) {
    throw EmptyQueueException();
  }
  return queue.top();
}

}
