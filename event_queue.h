#pragma once

#include <queue>
#include <vector>

#include "compare.h"

class EventQueue {
private:
std::priority_queue<Event*, std::vector<Event*>, Compare> queue;

// time of simulation
double time;

// number of full blocks mined by mining pool (specifies duration of simulation)
unsigned int blocks;
  
public:
EventQueue();

// schedule a new share event
void schedule(Event *_event);

// pops first element in event queue;
void pop();

// returns the current time of the event queue
double get_time() const;

// returns whether the event queue is empty or not
bool empty() const;

// sets time of event queue to t
void set_time(double t);

// resets time of event queue to 0
void reset_time();

// returns event first in queue
Event* get_top();

// begin simulation of events for a duration of _blocks
double simulate(unsigned int sim_length);

};
