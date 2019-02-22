#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "miner.h"
#include "factory.h"


class MinerCreator {
public:
  virtual std::vector<Miner> create_miners(const nlohmann::json& args) = 0;
};

class CSVMinerCreator : public MinerCreator, public Creatable0<MinerCreator, CSVMinerCreator> {
public:
  std::vector<Miner> create_miners(const nlohmann::json& args) override;
};


MAKE_FACTORY(MinerCreator);