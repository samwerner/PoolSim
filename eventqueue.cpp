#include "eventqueue.h"
#include "event.h"
#include <iostream>

Event_Queue::Event_Queue() {
  time = 0;
  blocks = 0;
}

void Event_Queue::schedule(Event *_event) {
  queue.push(std::move(_event));
}

void Event_Queue::pop() {
  queue.pop();
}

double Event_Queue::get_time() const {
  return time;
}

double Event_Queue::empty() {
  return queue.empty();
}

void Event_Queue::set_time(double t) {
  time = t;
}

void Event_Queue::reset_time() {
  time = 0;
}

Event* Event_Queue::get_top() {
  return queue.top();
}

double Event_Queue::simulate(unsigned int sim_length) {
  if (this->empty() || !sim_length) {
    std::cerr << "Simulation failed. There are no events scheduled." << std::endl;
    // throw exception
    
  }
  
  while (blocks < sim_length) {
    Event* event = this->get_top();
    this->pop();
    this->set_time(event->get_time());
    blocks += event->execute(this);
    delete event;
    event = nullptr;
  }

  std::cout << "Simulation of " << blocks << " has finished" << std::endl;
  
}
