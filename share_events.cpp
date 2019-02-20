#include "share_events.h"
#include "eventqueue.h"
#include "miner.h"
#include "miningpool.h"
#include "event.h"

Share_Event::Share_Event(Miner *_miner, double _time) : Event(time) {
  miner = _miner;
}

Share_Event::~Share_Event() {}

Pool_Event::Pool_Event(Miner *_miner, double _time) : Share_Event(miner, time) {}

Pool_Event::execute(Event_Queue *queue) {
  miner->pool_share();
  miner->schedule_share(queue);
}

Network_Event::Network_Event(Miner *_miner, double _time) : Share_Event(miner, time) {}

Network_Event::execute(Event_Queue *queue) {
  miner->network_share();
  miner->schedule_share(queue);
}
