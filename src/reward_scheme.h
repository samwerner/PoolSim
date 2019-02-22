#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "share.h"
#include "factory.h"


class MiningPool;

class RewardScheme {
public:
  virtual ~RewardScheme();

  virtual void handle_share(const std::string& miner_address, const Share& share) = 0;

  // Set the mining pool for this reward scheme
  // RewardScheme and MiningPool should must a 1 to 1 relationship
  void set_mining_pool(std::shared_ptr<MiningPool> mining_pool);

  // Returns the mining_pool of this reward scheme as a shared_ptr
  // Use this rather than accessing the weak_ptr property
  std::shared_ptr<MiningPool> get_mining_pool();
protected:
  std::weak_ptr<MiningPool> mining_pool;
};

MAKE_FACTORY(RewardSchemeFactory, RewardScheme, const nlohmann::json&);

template <typename T>
class BaseRewardScheme :
  public RewardScheme,
  public Creatable1<RewardScheme, T, const nlohmann::json&> {
};


// Pay-per-share reward scheme
class PPSRewardScheme: public BaseRewardScheme<PPSRewardScheme> {
public:
  explicit PPSRewardScheme(const nlohmann::json& args);

  void handle_share(const std::string& miner_address, const Share& share) override;
};
