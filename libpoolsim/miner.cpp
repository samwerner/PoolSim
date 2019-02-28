#include <stdexcept>

#include "miner.h"
#include "mining_pool.h"

namespace poolsim {


Miner::Miner(std::string _address, double _hashrate)
  : address(_address), hashrate(_hashrate) {}

std::shared_ptr<Miner> Miner::create(std::string address, double hashrate,
                    std::unique_ptr<ShareHandler> handler) {
  if (handler == nullptr) {
    throw std::invalid_argument("handler cannot be null");
  }
  auto miner = std::shared_ptr<Miner>(new Miner(address, hashrate));
  miner->set_handler(std::move(handler));
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
  share_handler->handle_share(share);
}

void Miner::set_handler(std::unique_ptr<ShareHandler> _share_handler) {
  share_handler = std::move(_share_handler);
  share_handler->set_miner(shared_from_this());
}

}
