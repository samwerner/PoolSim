#ifndef SHAREEVENTS_H
#define SHAREEVENTS_H

class Event, Event_Queue, Miner;

class Share_Event : public Event {
protected:
Miner *miner;

public:
Share_Event(Miner *_miner, double _time);

~Share_Event();
};

class Pool_Event : public Share_Event {
 public:
Pool_Event(Miner *_miner, double _time);

// executes a pool event: a valid pool share but an invalid network share has been submitted to the pool
int execute(Event_Queue *queue);

};

class Network_Event : public Share_Event {
 public:
Network_Event(Miner *_miner, double _time);

// executes a network event: a valid network share has been submitted to the pool
int execute(Event_Queue *queue);
};

#endif
