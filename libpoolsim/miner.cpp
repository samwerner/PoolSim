#include <stdexcept>

#include "miner.h"
#include "mining_pool.h"

namespace poolsim {


Miner::Miner(std::string _address, double _hashrate, std::shared_ptr<Network> _network)
  : address(_address), hashrate(_hashrate), network(_network) {}

std::shared_ptr<Miner> Miner::create(std::string address, double hashrate,
                    std::unique_ptr<ShareHandler> handler,
                    std::shared_ptr<Network> network) {
  if (handler == nullptr) {
    throw std::invalid_argument("handler cannot be null");
  }
  auto miner = std::shared_ptr<Miner>(new Miner(address, hashrate, network));
  miner->set_handler(std::move(handler));
  return miner;
}

std::string Miner::get_address() const { return address; }

double Miner::get_hashrate() const { return hashrate; }

std::shared_ptr<MiningPool> Miner::get_pool() const {
  return pool.lock();
}

std::shared_ptr<Network> Miner::get_network() const {
    return network.lock();
}

uint64_t Miner::get_blocks_found() const {
    return blocks_found;
}

uint64_t Miner::get_total_work() const {
    return total_work;
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
    total_work += get_pool()->get_difficulty();
    if (share.is_network_share()) {
        blocks_found++;
    }
    share_handler->handle_share(share);
}

void Miner::set_handler(std::unique_ptr<ShareHandler> _share_handler) {
  share_handler = std::move(_share_handler);
  share_handler->set_miner(shared_from_this());
}

void to_json(nlohmann::json& j, const Miner& miner) {
    j["address"] = miner.get_address();
    j["hashrate"] = miner.get_hashrate();
    j["blocks_found"] = miner.get_blocks_found();
    j["total_work"] = miner.get_total_work();
}

}
