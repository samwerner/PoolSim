#include <stdexcept>

#include "mining_pool.h"

namespace poolsim {

std::shared_ptr<MiningPool> MiningPool::create(
    const std::string& name,
    uint64_t difficulty,
    double uncle_prob,
    std::unique_ptr<RewardScheme> reward_scheme,
    std::shared_ptr<Network> network
) {
    return create(name, difficulty, uncle_prob, std::move(reward_scheme), network, SystemRandom::get_instance());
}

std::shared_ptr<MiningPool> MiningPool::create(const std::string& name,
                                               uint64_t difficulty, double uncle_prob,
                                               std::unique_ptr<RewardScheme> reward_scheme,
                                               std::shared_ptr<Network> network,
                                               std::shared_ptr<Random> random) {
    auto mining_pool = std::shared_ptr<MiningPool>(new MiningPool(name, difficulty, uncle_prob, network, random));
    if (reward_scheme == nullptr) {
        throw std::invalid_argument("reward_scheme cannot be null");
    }
    mining_pool->set_reward_scheme(std::move(reward_scheme));
    return mining_pool;
}

MiningPool::MiningPool(const std::string& name,
                       uint64_t _difficulty,
                       double _uncle_prob,
                       std::shared_ptr<Network> _network,
                       std::shared_ptr<Random> _random)
    : pool_name(name), difficulty(_difficulty), uncle_prob(_uncle_prob),
      network(_network), random(_random) {}

void MiningPool::set_reward_scheme(std::unique_ptr<RewardScheme> _reward_scheme) {
    reward_scheme = std::move(_reward_scheme);
    reward_scheme->set_mining_pool(shared_from_this());
}

std::string MiningPool::get_scheme_name() const {
    return reward_scheme->get_scheme_name();
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

std::shared_ptr<Network> MiningPool::get_network() const {
    return network.lock();
}

uint64_t MiningPool::get_difficulty() const {
  return difficulty;
}

size_t MiningPool::get_miners_count() const {
  return miners.size();
}

std::string MiningPool::get_name() const {
  return pool_name;
}

nlohmann::json MiningPool::get_miners_metadata() const {
    nlohmann::json result;
    for (const std::string& address : miners) {
        nlohmann::json miner;
        miner["address"] = address;
        miner["metadata"] = reward_scheme->get_miner_metadata(address);
        result.push_back(miner);
    }
    return result;
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
            .is_uncle = share.is_uncle(),
            .pool_name = pool_name,
            .miner_address = miner_address,
            .reward_scheme_data = reward_scheme->get_block_metadata()
        };
        notify(block_event);
    }
}

void to_json(nlohmann::json& j, const MiningPool& pool) {
    j["name"] = pool.get_name();
    j["difficulty"] = pool.get_difficulty();
    j["reward_scheme"] = pool.get_scheme_name();
    j["miners"] = pool.get_miners_metadata();
}

}
