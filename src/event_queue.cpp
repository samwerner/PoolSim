#include <iostream>

#include "event_queue.h"
#include "event.h"

EventQueue::EventQueue() {
  time = 0;
  blocks = 0;
}

void EventQueue::schedule(Event event) {
  queue.push(event);
}

void EventQueue::pop() {
  queue.pop();
}

double EventQueue::get_time() const {
  return time;
}

bool EventQueue::empty() const {
  return queue.empty();
}

void EventQueue::set_time(double t) {
  time = t;
}

void EventQueue::reset_time() {
  time = 0;
}

Event EventQueue::get_top() {
  return queue.top();
}

double EventQueue::simulate(unsigned int sim_length) {
  if (this->empty() || !sim_length) {
    std::cerr << "Simulation failed. There are no events scheduled." << std::endl;
    // throw exception
    
  }
  
  while (blocks < sim_length) {
    Event event = this->get_top();
    this->pop();
    this->set_time(event.time);
    // blocks += event->execute(this);
  }

  std::cout << "Simulation of " << blocks << " has finished" << std::endl;

  return 0;
}
