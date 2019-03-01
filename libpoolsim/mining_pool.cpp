#include <stdexcept>

#include "mining_pool.h"

namespace poolsim {

std::shared_ptr<MiningPool> MiningPool::create(
    const std::string& name,
    uint64_t difficulty, double uncle_prob, std::unique_ptr<RewardScheme> reward_scheme) {
  return create(name, difficulty, uncle_prob, std::move(reward_scheme), SystemRandom::get_instance());
}

std::shared_ptr<MiningPool> MiningPool::create(
    const std::string& name,
    uint64_t difficulty, double uncle_prob,
    std::unique_ptr<RewardScheme> reward_scheme,
    std::shared_ptr<Random> random) {
  auto mining_pool = std::shared_ptr<MiningPool>(new MiningPool(name, difficulty, uncle_prob, random));
  if (reward_scheme == nullptr) {
    throw std::invalid_argument("reward_scheme cannot be null");
  }
  mining_pool->set_reward_scheme(std::move(reward_scheme));
  return mining_pool;
}

MiningPool::MiningPool(const std::string& name, uint64_t _difficulty,
                       double _uncle_prob, std::shared_ptr<Random> _random)
  : pool_name(name), difficulty(_difficulty), uncle_prob(_uncle_prob), random(_random) {}

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
    uint8_t flags = share.get_properties();
    if (share.is_network_share() && random->drand48() < uncle_prob) {
        flags |= Share::Property::uncle;
    }
    reward_scheme->handle_share(miner_address, Share(flags));
    if (share.is_valid_block()) {
        BlockEvent block_event {
            .time = 0,
            .pool_name = pool_name,
            .miner_address = miner_address,
            .reward_scheme_data = reward_scheme->get_block_metadata()
        };
        notify(block_event);
    }
}

}
