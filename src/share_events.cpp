#include "share_events.h"
#include "event_queue.h"
#include "miner.h"
#include "mining_pool.h"
#include "event.h"

ShareEvent::ShareEvent(Miner *_miner, double _time) : Event(time) {
  miner = _miner;
}

PoolEvent::PoolEvent(Miner *_miner, double _time) : ShareEvent(miner, time) {}

int PoolEvent::execute(EventQueue *queue) {
  miner->pool_share();
  // miner->schedule_share(queue);
  return 0;
}

NetworkEvent::NetworkEvent(Miner *_miner, double _time) : ShareEvent(miner, time) {}

bool NetworkEvent::execute(EventQueue *queue) {
  miner->network_share();
  // miner->schedule_share(queue);
  return false;
}
