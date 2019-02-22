#include "miner_creator.h"

using nlohmann::json;


MinerCreationState::MinerCreationState(size_t _miners_count, uint64_t _total_hashrate)
  : miners_count(_miners_count), total_hashrate(_total_hashrate) {}

TotalHashrateStopCondition::TotalHashrateStopCondition(const json& args)
  : max_hashrate(args["value"]) {}

bool TotalHashrateStopCondition::should_stop(const MinerCreationState& state) {
  return state.total_hashrate >= max_hashrate;
}

REGISTER(MinerCreatorStopCondition, TotalHashrateStopCondition, "total_hashrate")

MinersCountStopCondition::MinersCountStopCondition(const json& args)
  : max_count(args["value"]) {}

bool MinersCountStopCondition::should_stop(const MinerCreationState& state) {
  return state.miners_count >= max_count;
}

REGISTER(MinerCreatorStopCondition, MinersCountStopCondition, "miners_count")


MinerCreator::MinerCreator() : MinerCreator(SystemRandom::get_instance()) {}
MinerCreator::MinerCreator(std::shared_ptr<Random> _random)
  : random(_random) {}

std::vector<std::shared_ptr<Miner>> CSVMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  std::string filepath = args["filepath"];
  std::cout << filepath << std::endl;

  return miners;
}


std::vector<std::shared_ptr<Miner>> RandomMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  auto hashrate_distribution = DistributionFactory::create(args["hashrate"]["distribution"],
                                                           args["hashrate"]["params"]);
  auto stop_condition = MinerCreatorStopConditionFactory::create(args["stop_condition"]["type"],
                                                                 args["stop_condition"]["params"]);
  MinerCreationState state;
  while (!stop_condition->should_stop(state)) {
    double hashrate = hashrate_distribution->get();
    std::string address = random->get_address();
    auto miner = std::make_shared<Miner>(address, hashrate);
    miners.push_back(miner);
    state.miners_count++;
    state.total_hashrate += hashrate;
  }
  return miners;
}

REGISTER(MinerCreator, CSVMinerCreator, "csv")
REGISTER(MinerCreator, RandomMinerCreator, "random")
