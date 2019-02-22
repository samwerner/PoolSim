#include "miner_creator.h"


std::vector<std::shared_ptr<Miner>> CSVMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  std::string filepath = args["filepath"];
  std::cout << filepath << std::endl;

  return miners;
}


std::vector<std::shared_ptr<Miner>> DistributionMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<std::shared_ptr<Miner>> miners;
  return miners;
}

REGISTER(MinerCreator, CSVMinerCreator, "csv")
REGISTER(MinerCreator, DistributionMinerCreator, "distribution")
