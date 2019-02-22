#include "miner.h"
#include "mining_pool.h"

Miner::Miner(std::string _address, double _hashrate)
  : address(_address), hashrate(_hashrate) {}

std::shared_ptr<Miner> Miner::create(std::string address, double hashrate,
                    std::unique_ptr<ShareHandler> handler) {
  auto miner = std::shared_ptr<Miner>(new Miner(address, hashrate));
  if (handler != nullptr) {
    miner->set_handler(std::move(handler));
  }
  return miner;
}

std::string Miner::get_address() const { return address; }

double Miner::get_hashrate() const { return hashrate; }

std::shared_ptr<MiningPool> Miner::get_pool() const {
  return pool.lock();
}

void Miner::join_pool(std::shared_ptr<MiningPool> _pool) {
  if (get_pool() != nullptr) {
    get_pool()->leave(get_address());
    pool.reset();
  }
  pool = _pool;
  get_pool()->join(get_address());
}

void Miner::process_share(const Share& share) {
  // TODO: update stats
  share_handler->handle_share(share);
}

void Miner::set_handler(std::unique_ptr<ShareHandler> _share_handler) {
  share_handler = std::move(_share_handler);
  share_handler->set_miner(shared_from_this());
}

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
