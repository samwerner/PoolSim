#include "reward_scheme.h"
#include "miner.h"
#include "factory.h"

#include "mining_pool.h"


RewardScheme::~RewardScheme() {}

void RewardScheme::set_mining_pool(std::shared_ptr<MiningPool> _mining_pool) {
  mining_pool = _mining_pool;
}

std::shared_ptr<MiningPool> RewardScheme::get_mining_pool() {
  return mining_pool.lock();
}

PPSRewardScheme::PPSRewardScheme(const nlohmann::json& _args) {}

void PPSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
  // TODO: do some realy cool stuff with this share
}

REGISTER(RewardScheme, PPSRewardScheme, "pps")


PPLNSRewardScheme::PPLNSRewardScheme(const nlohmann::json& _args) {}

void PPLNSRewardScheme::handle_share(const std::string& miner_address, const Share& share) {
  // TODO: do some realy cool stuff with this share
}

REGISTER(RewardScheme, PPLNSRewardScheme, "pplns")
