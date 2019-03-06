#include "miner_creator.h"
#include "share_handler.h"

#include "csv.h"

namespace poolsim {

using nlohmann::json;

const double default_min_hashrate = 0.01;

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
    auto behavior_params = args["behavior"].value("params", json::object());
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
    double hashrate = hashrate_distribution->get();

    // do not allow negative hashrate
    if (hashrate < 0) {
      hashrate = args["hashrate"].value("minimum", default_min_hashrate);
    }

    // truncate if maximum hashrate is set
    double max_hashrate = args["hashrate"].value("maximum", -1);
    if (max_hashrate > 0 && hashrate > max_hashrate) {
        hashrate = max_hashrate;
    }

    std::string address = random->get_address();
    auto behavior_params = args["behavior"].value("params", json::object());
    auto share_handler = ShareHandlerFactory::create(args["behavior"]["name"], behavior_params);
    auto miner = Miner::create(address, hashrate, std::move(share_handler), network);
    miners.push_back(miner);
    state.miners_count++;
    state.total_hashrate += hashrate;
  }
  return miners;
}

REGISTER(MinerCreator, RandomMinerCreator, "random")

InlineMinerCreator::InlineMinerCreator(std::shared_ptr<Network> network)
    : MinerCreator(network) {}


std::vector<std::shared_ptr<Miner>> InlineMinerCreator::create_miners(const json& args) {
    std::vector<std::shared_ptr<Miner>> miners;
    for (const json& miner_info : args["miners"]) {
        std::string address = miner_info.value("address", random->get_address());
        double hashrate = miner_info["hashrate"];
        json behavior_info = miner_info.value("behavior", json::object());
        json behavior_params = behavior_info.value("params", json::object());
        std::string behavior_name = behavior_info.value("name", "default");
        auto share_handler = ShareHandlerFactory::create(behavior_name, behavior_params);
        auto miner = Miner::create(address, hashrate, std::move(share_handler), network);
        miners.push_back(miner);
    }
    return miners;
}

REGISTER(MinerCreator, InlineMinerCreator, "inline")


}
