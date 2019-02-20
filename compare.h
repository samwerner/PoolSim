#ifndef COMPARE_H
#define COMPARE_H

class Event;


// Overwrites 'Compare' class used for priority queue implementation in event_queue.h
class Compare {
public:
    bool operator()(const Event* l, const Event* r);
};


#endif 
