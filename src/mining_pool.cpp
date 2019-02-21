#include <iostream>

#include "mining_pool.h"


MiningPool::MiningPool() {}

MiningPool::MiningPool(const std::string& miner_file) {
  // populate pool with miners from file
}

void MiningPool::join(std::shared_ptr<Miner> miner) {
  miners[miner->get_address()] = miner;
}

void MiningPool::leave(const std::string& miner_address) {
  miners.erase(miner_address);
}

std::vector<std::shared_ptr<Miner>> MiningPool::get_miners() {
  std::vector<std::shared_ptr<Miner>> result;
  for (auto it = miners.begin(); it != miners.end(); it++) {
    result.push_back(it->second);
  }
  return result;
}

void MiningPool::submit_share(const std::string& miner_address, const Share& share) {
}
