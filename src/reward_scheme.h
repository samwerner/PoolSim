#pragma once

#include <memory>

#include <nlohmann/json.hpp>

#include "share.h"
#include "factory.h"
#include "miner_record.h"


class MiningPool;

class RewardScheme {
public:
  virtual ~RewardScheme();

  virtual void handle_share(const std::string& miner_address, const Share& share) = 0;

  // Set the mining pool for this reward scheme
  // RewardScheme and MiningPool should be a 1 to 1 relationship
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


// Pay-per-last-n-shares reward scheme
class PPLNSRewardScheme: public BaseRewardScheme<PPLNSRewardScheme> {
public:
  explicit PPLNSRewardScheme(const nlohmann::json& args);

  void handle_share(const std::string& miner_address, const Share& share) override;
};

// Queue-based reward scheme
class QBRewardScheme: public BaseRewardScheme<QBRewardScheme> {
public:
  explicit QBRewardScheme(const nlohmann::json& args);

  void handle_share(const std::string& miner_address, const Share& share) override;

protected:
  // TODO: create derived classes for miner behaviour (e.g. donate shares)
  std::vector<std::shared_ptr<QBRecord>> records;

  // increments mined block and credits stats for a given record
  void update_record(std::shared_ptr<QBRecord> record, const Share& share);

  // increases credits of record for specified miner
  // if miner is not in pool then a new record for the miner is added
  std::shared_ptr<QBRecord> find_record(std::string miner_address);


  // rewards top miner in pool and resets the top miners credits
  void reward_top_miner();


};

