#pragma once

#include "event.h"


// Overwrites 'Compare' class used for priority queue implementation in event_queue.h
class Compare {
public:
    inline bool operator()(const Event* l, const Event* r) {
        return l->get_time() > r->get_time();
    }
};
