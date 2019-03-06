#include "miner_creator.h"
#include "share_handler.h"

#include "csv.h"

namespace poolsim {

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


MinerCreator::MinerCreator(std::shared_ptr<Network> network)
    : MinerCreator(network, SystemRandom::get_instance()) {}
MinerCreator::MinerCreator(std::shared_ptr<Network> _network, std::shared_ptr<Random> _random)
    : network(_network), random(_random) {}


CSVMinerCreator::CSVMinerCreator(std::shared_ptr<Network> network)
    : MinerCreator(network) {}


std::vector<std::shared_ptr<Miner>> CSVMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  std::string filepath = args["path"];
  io::CSVReader<3> in(filepath);
  in.read_header(io::ignore_missing_column, "address", "hashrate", "behavior");
  std::string address;
  double hashrate;
  std::string behavior_name;
  while (in.read_row(address, hashrate, behavior_name)) {
    if (behavior_name.empty()) {
      behavior_name = args["behavior"]["name"];
    }
    auto behavior_params = args["behavior"].value("params", json());
    if (behavior_params.find(behavior_name) != behavior_params.end()) {
        behavior_params = behavior_params[behavior_name];
    }
    auto share_handler = ShareHandlerFactory::create(behavior_name, behavior_params);
    auto miner = Miner::create(address, hashrate, std::move(share_handler), network);
    miners.push_back(miner);
    behavior_name.clear();
  }

  return miners;
}

REGISTER(MinerCreator, CSVMinerCreator, "csv")


RandomMinerCreator::RandomMinerCreator(std::shared_ptr<Network> network)
    : MinerCreator(network) {}

std::vector<std::shared_ptr<Miner>> RandomMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  auto hashrate_distribution = DistributionFactory::create(args["hashrate"]["distribution"],
                                                           args["hashrate"]["params"]);
  auto stop_condition = MinerCreatorStopConditionFactory::create(args["stop_condition"]["type"],
                                                                 args["stop_condition"]["params"]);
  MinerCreationState state;
  while (!stop_condition->should_stop(state)) {
    double hashrate = -1;
    while (hashrate < 0) {
      hashrate = hashrate_distribution->get();
    }
    std::string address = random->get_address();
    auto behavior_params = args["behavior"].value("params", json());
    auto share_handler = ShareHandlerFactory::create(args["behavior"]["name"], behavior_params);
    auto miner = Miner::create(address, hashrate, std::move(share_handler), network);
    miners.push_back(miner);
    state.miners_count++;
    state.total_hashrate += hashrate;
  }
  return miners;
}

REGISTER(MinerCreator, RandomMinerCreator, "random")

}
