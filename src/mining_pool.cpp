#include <iostream>

#include "mining_pool.h"


MiningPool::MiningPool(uint64_t _difficulty): difficulty(_difficulty) {}

MiningPool::MiningPool(const std::string& miner_file) {
  // populate pool with miners from file
}

void MiningPool::join(const std::string& miner_address) {
  miners.insert(miner_address);
}

void MiningPool::leave(const std::string& miner_address) {
  miners.erase(miner_address);
}

std::set<std::string> MiningPool::get_miners() {
  return miners;
}

uint64_t MiningPool::get_difficulty() const {
  return difficulty;
}

size_t MiningPool::get_miners_count() const {
  return miners.size();
}

void MiningPool::submit_share(const std::string& miner_address, const Share& share) {
}
