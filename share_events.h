#pragma once

#include "event_queue.h"
#include "event.h"
#include "miner.h"

class ShareEvent : public Event {
protected:
  Miner *miner;

public:
  ShareEvent(Miner *_miner, double _time);

};

class PoolEvent : public ShareEvent {
public:
  PoolEvent(Miner *_miner, double _time);

  // executes a pool event: a valid pool share but an invalid network share has been submitted to the pool
  int execute(EventQueue *queue);

};

class NetworkEvent : public ShareEvent {
public:
  NetworkEvent(Miner *_miner, double _time);

  // executes a network event: a valid network share has been submitted to the pool
  bool execute(EventQueue *queue);
};
