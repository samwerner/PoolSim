#include <stdexcept>

#include "mining_pool.h"


std::shared_ptr<MiningPool> MiningPool::create(
    uint64_t difficulty, double uncle_prob, std::unique_ptr<RewardScheme> reward_scheme) {
  auto mining_pool = std::shared_ptr<MiningPool>(new MiningPool(difficulty, uncle_prob));
  if (reward_scheme == nullptr) {
    throw std::invalid_argument("reward_scheme cannot be null");
  }
  mining_pool->set_reward_scheme(std::move(reward_scheme));
  return mining_pool;
}

MiningPool::MiningPool(uint64_t _difficulty, double _uncle_prob)
  : difficulty(_difficulty), uncle_prob(_uncle_prob) {}

void MiningPool::set_reward_scheme(std::unique_ptr<RewardScheme> _reward_scheme) {
  reward_scheme = std::move(_reward_scheme);
  reward_scheme->set_mining_pool(shared_from_this());
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
  reward_scheme->handle_share(miner_address, share);
}
