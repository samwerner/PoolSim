#include "miner_creator.h"


std::vector<Miner> CSVMinerCreator::create_miners(const nlohmann::json& args) {
  std::vector<Miner> miners;

  return miners;
}

REGISTER(MinerCreator, CSVMinerCreator, "csv")
