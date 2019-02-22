#pragma once

#include <vector>
#include <nlohmann/json.hpp>

#include "miner.h"
#include "factory.h"


class MinerCreator {
public:
  virtual std::vector<std::shared_ptr<Miner>> create_miners(const nlohmann::json& args) = 0;
};

class CSVMinerCreator : public MinerCreator, public Creatable0<MinerCreator, CSVMinerCreator> {
public:
  std::vector<std::shared_ptr<Miner>> create_miners(const nlohmann::json& args) override;
};

class DistributionMinerCreator : public MinerCreator,
                                 public Creatable0<MinerCreator, DistributionMinerCreator> {
public:
  std::vector<std::shared_ptr<Miner>> create_miners(const nlohmann::json& args) override;
};


MAKE_FACTORY(MinerCreatorFactory, MinerCreator)
