#include <cmath>
#include <cassert>


#include "miner.h"
#include "mining_pool.h"

Miner::Miner(std::string _address, double _hashrate)
  : address(_address), hashrate(_hashrate) {
}

Miner::Miner(std::string _address, double _hashrate, std::shared_ptr<MiningPool> _pool)
  : address(_address), hashrate(_hashrate), pool(_pool) {
  // initialise member variables to zero
}


std::string Miner::get_address() const { return address; }

double Miner::get_hashrate() const { return hashrate; }

std::shared_ptr<MiningPool> Miner::get_pool() const { return pool; }

void Miner::join_pool(std::shared_ptr<MiningPool> _pool) {
  leave_pool();
  pool = _pool;
  pool->join(shared_from_this());
}

void Miner::leave_pool() {
  if (pool != nullptr) {
    pool->leave(get_address());
  }
  pool = nullptr;
}

// bool Miner::schedule_share(EventQueue *queue) {
//     assert(queue != NULL);

//     if (hashrate <= 0) {
//         return false;
//     }

//     double lambda = (double) hashrate / pool->getShareDiff();
//     double t = -log(drand48()) / lambda;
//     double p = (double) pool->getShareDiff() / pool->getNetDiff();

//     // TODO: allow
//     // minerBehavior.processShare(self, pool, isValidProof)

//     if (drand48() < p) {
//         queue->schedule(new NetworkShareEvent(this, queue->getTime() + t));
//         return true;
//     } else {
//       queue->schedule(new MiningPoolShareEvent(this, queue->getTime() + t));
//       return true;
//     }
// }

void Miner::set_credits(unsigned long long _credits) {
  credits = _credits;
}

void Miner::inc_blocks_received() {
  blocks_received++;
}

void Miner::inc_blocks_mined() {
  blocks_mined++;
}

void Miner::inc_uncles_received() {
  uncles_received++;
}

void Miner::inc_uncles_mined() {
  uncles_mined++;
}
